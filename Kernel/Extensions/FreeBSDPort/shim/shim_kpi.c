#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/mutex.h>
#include <sys/mbuf.h>
#include <sys/buf_ring.h>
#include <sys/taskqueue.h>
#include <sys/callout.h>
#include <sys/sglist.h>
#include <sys/sbuf.h>
#include <sys/sysctl.h>
#include <sys/random.h>
#include <sys/bus.h>
#include <sys/rman.h>
#include <sys/errno.h>
#include <net/if_var.h>
#include <net/ethernet.h>
#include <libkern/OSMalloc.h>
#include <IOKit/IOLocks.h>
#include <kern/thread_call.h>

int ticks = 0;
int hz = 100;
int tick = 10000;
int bootverbose = 0;
int physmem = 0;
int time_uptime = 0;
int time_second = 0;
int mp_ncpus = 1;

struct malloc_type M_DEVBUF[1] = { { "devbuf", NULL } };
struct malloc_type M_TEMP[1] = { { "temp", NULL } };
struct malloc_type M_VTNET[1] = { { "vtnet", NULL } };
struct malloc_type M_VTNET_SHM[1] = { { "vtnet shm", NULL } };
struct malloc_type M_MBUF[1] = { { "mbufshim", NULL } };

struct taskqueue *taskqueue_fast;

static volatile uint32_t reported_bits[16];

void
shim_report_unimplemented(const char *what)
{
	unsigned h;
	unsigned i;
	uint32_t bit;

	h = 5381;
	for (i = 0; what[i] != 0; i++)
		h = h * 33 + (unsigned char)what[i];
	h &= 511;
	bit = 1u << (h & 31);
	if (__sync_fetch_and_or(&reported_bits[h >> 5], bit) & bit)
		return;
	printf("FreeBSDPort: UNIMPLEMENTED: %s\n", what);
}

static OSMallocTag
shim_tag(struct malloc_type *type)
{
	OSMallocTag tag;

	if (type == NULL)
		type = M_TEMP;
	tag = (OSMallocTag)__sync_fetch_and_add(&type->ks_tag, (void *)0);
	if (tag != NULL)
		return (tag);
	tag = OSMalloc_Tagalloc(type->ks_shortdesc != NULL ?
	    type->ks_shortdesc : "bsdport", OSMT_DEFAULT);
	if (__sync_bool_compare_and_swap(&type->ks_tag, NULL, tag))
		return (tag);
	OSMalloc_Tagfree(tag);
	return ((OSMallocTag)type->ks_tag);
}

void *
malloc(unsigned long size, struct malloc_type *type, int flags)
{
	struct shim_alloc_hdr {
		OSMallocTag tag;
		uint32_t size;
	} *hdr;
	void *block;
	unsigned long total;

	if (size == 0)
		return (NULL);
	total = size + sizeof(struct shim_alloc_hdr);
	block = OSMalloc((uint32_t)total, shim_tag(type));
	if (block == NULL)
		return (NULL);
	if ((flags & M_ZERO) != 0)
		bzero(block, total);
	hdr = block;
	hdr->tag = (OSMallocTag)type->ks_tag;
	hdr->size = (uint32_t)total;
	return ((char *)block + sizeof(struct shim_alloc_hdr));
}

void
free(void *addr, struct malloc_type *type)
{
	struct shim_alloc_hdr {
		OSMallocTag tag;
		uint32_t size;
	} *hdr;

	(void)type;
	if (addr == NULL)
		return;
	hdr = (void *)((char *)addr - sizeof(struct shim_alloc_hdr));
	OSFree(hdr, hdr->size, hdr->tag);
}

void *
contigmalloc(unsigned long size, struct malloc_type *type, int flags,
    unsigned long low, unsigned long high, unsigned long alignment,
    unsigned long boundary)
{
	struct shim_alloc_hdr {
		OSMallocTag tag;
		uint32_t size;
	} *hdr;
	void *block;
	void *aligned;
	unsigned long total;
	unsigned long align;

	(void)low;
	(void)high;
	(void)boundary;
	if (size == 0)
		return (NULL);
	align = alignment < 8 ? 8 : alignment;
	total = size + align + sizeof(struct shim_alloc_hdr);
	block = OSMalloc((uint32_t)total, shim_tag(type));
	if (block == NULL)
		return (NULL);
	if ((flags & M_ZERO) != 0)
		bzero(block, total);
	aligned = (void *)(((uintptr_t)block + sizeof(struct shim_alloc_hdr) +
	    align - 1) & ~(align - 1));
	hdr = (void *)((char *)aligned - sizeof(struct shim_alloc_hdr));
	hdr->tag = (OSMallocTag)type->ks_tag;
	hdr->size = (uint32_t)total;
	return (aligned);
}

void
contigfree(void *addr, unsigned long size, struct malloc_type *type)
{
	struct shim_alloc_hdr {
		OSMallocTag tag;
		uint32_t size;
	} *hdr;

	(void)size;
	(void)type;
	if (addr == NULL)
		return;
	hdr = (void *)((char *)addr - sizeof(struct shim_alloc_hdr));
	OSFree(hdr, hdr->size, hdr->tag);
}

void *
realloc(void *addr, unsigned long size, struct malloc_type *type, int flags)
{
	struct shim_alloc_hdr {
		OSMallocTag tag;
		uint32_t size;
	} *hdr;
	void *n;

	if (addr == NULL)
		return (malloc(size, type, flags));
	hdr = (void *)((char *)addr - sizeof(struct shim_alloc_hdr));
	if ((unsigned long)hdr->size >= size + sizeof(struct shim_alloc_hdr))
		return (addr);
	n = malloc(size, type, flags);
	if (n == NULL)
		return (NULL);
	bcopy(addr, n, hdr->size - sizeof(struct shim_alloc_hdr));
	free(addr, type);
	return (n);
}

unsigned long
malloc_usable_size(void *addr)
{
	struct shim_alloc_hdr {
		OSMallocTag tag;
		uint32_t size;
	} *hdr;

	if (addr == NULL)
		return (0);
	hdr = (void *)((char *)addr - sizeof(struct shim_alloc_hdr));
	return (hdr->size - sizeof(struct shim_alloc_hdr));
}

int
TUNABLE_INT_FETCH(const char *path, int *val)
{
	(void)path;
	(void)val;
	return (0);
}

int
tsleep(void *ident, int priority, const char *wmesg, int timo)
{
	(void)ident;
	(void)priority;
	(void)wmesg;
	(void)timo;
	shim_report_unimplemented("tsleep");
	return (EOPNOTSUPP);
}

void
mtx_init(struct mtx *m, const char *name, const char *type, int opts)
{
	(void)type;
	(void)opts;
	m->m_name = name;
	m->m_lock = IOLockAlloc();
}

void
mtx_destroy(struct mtx *m)
{
	if (m->m_lock != NULL)
		IOLockFree(m->m_lock);
	m->m_lock = NULL;
}

void
mtx_lock(struct mtx *m)
{
	IOLockLock(m->m_lock);
}

void
mtx_unlock(struct mtx *m)
{
	IOLockUnlock(m->m_lock);
}

int
mtx_trylock(struct mtx *m)
{
	return (IOLockTryLock(m->m_lock) ? 1 : 0);
}

int
mtx_initialized(struct mtx *m)
{
	return (m->m_lock != NULL);
}

void
if_addr_lock(if_t ifp)
{
	(void)ifp;
}

void
if_addr_unlock(if_t ifp)
{
	(void)ifp;
}

void
if_addr_rlock(if_t ifp)
{
	if_addr_lock(ifp);
}

void
if_addr_runlock(if_t ifp)
{
	if_addr_unlock(ifp);
}

void
if_maddr_rlock(if_t ifp)
{
	if_addr_lock(ifp);
}

void
if_maddr_runlock(if_t ifp)
{
	if_addr_unlock(ifp);
}

struct sysctl_ctx_list *
device_get_sysctl_ctx(device_t dev)
{
	return (&dev->dev_sysctl_ctx);
}

struct sysctl_oid *
device_get_sysctl_tree(device_t dev)
{
	return (dev->dev_sysctl_tree);
}

if_t
if_alloc(u_char type)
{
	struct ifnet *ifp;

	ifp = malloc(sizeof(*ifp), M_DEVBUF, M_ZERO | M_WAITOK);
	(void)type;
	ifp->if_dunit = -1;
	return (ifp);
}

void
if_free(if_t ifp)
{
	free(ifp, M_DEVBUF);
}

void
if_initname(if_t ifp, const char *name, int unit)
{
	ifp->if_dname = name;
	ifp->if_dunit = unit;
	if (unit == -1)
		snprintf(ifp->if_xname, IFNAMSIZ, "%s", name);
	else
		snprintf(ifp->if_xname, IFNAMSIZ, "%s%d", name, unit);
}

void
if_attach(if_t ifp)
{
	(void)ifp;
}

void
if_detach(if_t ifp)
{
	(void)ifp;
}

void
if_setsoftc(if_t ifp, void *softc)
{
	ifp->if_softc = softc;
}

void *
if_getsoftc(if_t ifp)
{
	return (ifp->if_softc);
}

void
if_printf(if_t ifp, const char *fmt, ...)
{
	char buf[256];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	printf("%s: %s", ifp->if_xname, buf);
}

void
if_setmtu(if_t ifp, int mtu)
{
	ifp->if_mtu = mtu;
}

void
if_sethwassist(if_t ifp, u_long hwassist)
{
	ifp->if_hwassist = hwassist;
}

void
if_setcapabilities(if_t ifp, int caps)
{
	ifp->if_capabilities = caps;
}

void
if_setcapenable(if_t ifp, int caps)
{
	ifp->if_capenable = caps;
}

void
if_sethdrlen(if_t ifp, int hdrlen)
{
	ifp->if_hdrlen = hdrlen;
}

void
if_setinitfn(if_t ifp, if_init_fn_t fn)
{
	ifp->if_init = fn;
}

void
if_setioctlfn(if_t ifp, if_ioctl_fn_t fn)
{
	ifp->if_ioctl = fn;
}

void
if_setstartfn(if_t ifp, if_start_fn_t fn)
{
	ifp->if_start = fn;
}

void
if_settransmitfn(if_t ifp, if_transmit_fn_t fn)
{
	ifp->if_transmit = fn;
}

void
if_setqflushfn(if_t ifp, if_qflush_fn_t fn)
{
	ifp->if_qflush = fn;
}

void
if_setinputfn(if_t ifp, if_input_fn_t fn)
{
	ifp->if_input = fn;
}

void
if_setifheaderlen(if_t ifp, int len)
{
	ifp->if_hdrlen = len;
}

void
if_setbaudrate(if_t ifp, u_long baudrate)
{
	ifp->if_baudrate = baudrate;
}

void
if_setdev(if_t ifp, void *dev)
{
	ifp->if_llsoftc = dev;
}

void
if_qflush(if_t ifp)
{
	(void)ifp;
}

void
if_input(if_t ifp, struct mbuf *m)
{
	(void)ifp;
	shim_report_unimplemented("if_input: packet delivery");
	m_freem(m);
}

void
if_start(if_t ifp)
{
	if (ifp->if_start != NULL)
		ifp->if_start(ifp);
}

void
if_link_state_change(if_t ifp, int link_state)
{
	ifp->if_link_state = link_state;
}

uint64_t
if_get_counter_default(if_t ifp, ift_counter cnt)
{
	(void)ifp;
	(void)cnt;
	return (0);
}

int
if_ioctl(if_t ifp, u_long cmd, caddr_t data)
{
	(void)ifp;
	(void)cmd;
	(void)data;
	return (ENODEV);
}

int
if_output(if_t ifp, struct mbuf *m)
{
	(void)ifp;
	shim_report_unimplemented("if_output");
	m_freem(m);
	return (ENODEV);
}

int
if_getmtu(if_t ifp)
{
	return (ifp->if_mtu);
}

int
if_getmtu_family(if_t ifp, int family)
{
	(void)family;
	return (ifp->if_mtu);
}

u_long
if_gethwassist(if_t ifp)
{
	return (ifp->if_hwassist);
}

int
if_getcapabilities(if_t ifp)
{
	return ((int)ifp->if_capabilities);
}

int
if_getcapenable(if_t ifp)
{
	return ((int)ifp->if_capenable);
}

int
if_getdunit(if_t ifp)
{
	return (ifp->if_dunit);
}

const char *
if_getdname(if_t ifp)
{
	return (ifp->if_dname);
}

const char *
if_getname(if_t ifp)
{
	return (ifp->if_xname);
}

int
if_getindex(if_t ifp)
{
	return (0);
}

struct ifnet *
ifnet_byindex(int idx)
{
	(void)idx;
	return (NULL);
}

struct ifaddr *
ifaddr_byindex(int idx)
{
	(void)idx;
	return (NULL);
}

int
if_addmulti(if_t ifp, struct sockaddr *sa, struct ifmultiaddr **ifmap)
{
	(void)ifp;
	(void)sa;
	(void)ifmap;
	shim_report_unimplemented("if_addmulti");
	return (EOPNOTSUPP);
}

int
if_delmulti(if_t ifp, struct sockaddr *sa)
{
	(void)ifp;
	(void)sa;
	shim_report_unimplemented("if_delmulti");
	return (EOPNOTSUPP);
}

void
if_vmove(if_t ifp)
{
	(void)ifp;
}

void
bpf_mtap(if_t ifp, struct mbuf *m)
{
	(void)ifp;
	shim_report_unimplemented("bpf_mtap: taps discarded");
	m_freem(m);
}

void
bpfattach(if_t ifp, u_int dlt, u_int hdrlen)
{
	(void)ifp;
	(void)dlt;
	(void)hdrlen;
}

static struct mbuf_ext *
m_ext_alloc(int size)
{
	struct shim_ext_block {
		struct mbuf_ext ext;
	} *blk;
	struct mbuf_ext *ext;
	void *buf;

	buf = OSMalloc((uint32_t)(size + sizeof(struct mbuf_ext)), shim_tag(M_MBUF));
	if (buf == NULL)
		return (NULL);
	ext = buf;
	ext->ext_buf = (char *)buf + sizeof(struct mbuf_ext);
	ext->ext_size = size;
	return (ext);
}

static void
m_ext_free(struct mbuf *m)
{
	OSFree(m->m_ext, m->m_ext->ext_size + sizeof(struct mbuf_ext),
	    shim_tag(M_MBUF));
	m->m_ext = NULL;
}

struct mbuf *
m_get(int how, short type)
{
	struct mbuf *m;

	m = malloc(sizeof(*m), M_MBUF, M_ZERO);
	if (m == NULL)
		return (NULL);
	m->m_type = type;
	m->m_data = m->m_ibuf;
	(void)how;
	return (m);
}

struct mbuf *
m_gethdr(int how, short type)
{
	struct mbuf *m;

	m = m_get(how, type);
	if (m != NULL)
		m->m_flags |= M_PKTHDR;
	return (m);
}

struct mbuf *
m_getcl(int how, short type, int flags)
{
	struct mbuf *m;

	m = m_get(how, type);
	if (m == NULL)
		return (NULL);
	m->m_ext = m_ext_alloc(MCLBYTES);
	if (m->m_ext == NULL) {
		m_free(m);
		return (NULL);
	}
	m->m_data = m->m_ext->ext_buf;
	m->m_flags |= M_EXT;
	(void)flags;
	return (m);
}

struct mbuf *
m_getjcl(int how, short type, int flags, int size)
{
	struct mbuf *m;

	if (size != MCLBYTES && size != MJUMPAGESIZE && size != MJUM9BYTES &&
	    size != MJUM16BYTES)
		return (NULL);
	m = m_get(how, type);
	if (m == NULL)
		return (NULL);
	m->m_ext = m_ext_alloc(size);
	if (m->m_ext == NULL) {
		m_free(m);
		return (NULL);
	}
	m->m_data = m->m_ext->ext_buf;
	m->m_flags |= M_EXT;
	(void)flags;
	return (m);
}

struct mbuf *
m_free(struct mbuf *m)
{
	struct mbuf *next;

	next = m->m_next;
	if ((m->m_flags & M_EXT) != 0)
		m_ext_free(m);
	free(m, M_MBUF);
	return (next);
}

void
m_freem(struct mbuf *m)
{
	struct mbuf *n;

	while (m != NULL) {
		n = m_free(m);
		m = n;
	}
}

void
m_adj(struct mbuf *mp, int len)
{
	struct mbuf *m;
	int count;

	if (len >= 0) {
		m = mp;
		while (m != NULL && len > 0) {
			if (m->m_len <= len) {
				len -= m->m_len;
				m->m_len = 0;
				m = m->m_next;
			} else {
				m->m_len -= len;
				m->m_data += len;
				len = 0;
			}
		}
		if ((mp->m_flags & M_PKTHDR) != 0)
			mp->m_pkthdr.len = m_length(mp, NULL);
		return;
	}
	len = -len;
	count = mp->m_len;
	if (count > len) {
		mp->m_len -= len;
		if ((mp->m_flags & M_PKTHDR) != 0)
			mp->m_pkthdr.len -= len;
		return;
	}
	len -= count;
	m = mp;
	while ((m = m->m_next) != NULL && len > 0) {
		count = m->m_len;
		if (count > len)
			m->m_len -= len;
		else
			m->m_len = 0;
		len -= count;
	}
	if ((mp->m_flags & M_PKTHDR) != 0)
		mp->m_pkthdr.len = m_length(mp, NULL);
}

void
m_cat(struct mbuf *m, struct mbuf *n)
{
	struct mbuf *p;

	if (n == NULL)
		return;
	if ((m->m_flags & M_PKTHDR) != 0) {
		m->m_pkthdr.len += m_length(n, NULL);
	}
	p = m;
	while (p->m_next != NULL)
		p = p->m_next;
	p->m_next = n;
}

struct mbuf *
m_pullup(struct mbuf *m, int len)
{
	struct mbuf *n;
	struct mbuf *p;
	int count;

	if (m->m_len >= len)
		return (m);
	if ((m->m_flags & M_EXT) == 0 && len > (int)MSIZE)
		goto bad;
	if ((m->m_flags & M_PKTHDR) != 0 && len > (int)MCLBYTES)
		goto bad;
	n = (m->m_flags & M_PKTHDR) != 0 ?
	    m_getcl(M_DONTWAIT, m->m_type, M_PKTHDR) :
	    m_get(M_DONTWAIT, m->m_type);
	if (n == NULL)
		goto bad;
	count = 0;
	p = m;
	while (p != NULL && count < len) {
		int take;

		take = p->m_len;
		if (take > len - count)
			take = len - count;
		bcopy(p->m_data, n->m_data + count, take);
		count += take;
		p->m_data += take;
		p->m_len -= take;
		p = p->m_next;
	}
	n->m_len = count;
	n->m_next = p;
	n->m_pkthdr = m->m_pkthdr;
	n->m_flags |= m->m_flags & (M_PKTHDR | M_BCAST | M_MCAST);
	m->m_flags &= ~(M_PKTHDR);
	m->m_len = 0;
	m_freem(m);
	if (count < len) {
		m_freem(n);
		return (NULL);
	}
	return (n);
bad:
	m_freem(m);
	return (NULL);
}

int
m_length(struct mbuf *m, struct mbuf **last)
{
	int len;

	len = 0;
	if (m != NULL && (m->m_flags & M_PKTHDR) != 0 && m->m_pkthdr.len != 0) {
		struct mbuf *p;

		p = m;
		while (p->m_next != NULL)
			p = p->m_next;
		len = m->m_pkthdr.len;
		if (last != NULL)
			*last = p;
		return (len);
	}
	while (m != NULL) {
		len += m->m_len;
		if (last != NULL)
			*last = m;
		m = m->m_next;
	}
	return (len);
}

u_int
m_fixhdr(struct mbuf *m)
{
	u_int len;

	len = (u_int)m_length(m, NULL);
	m->m_pkthdr.len = (int)len;
	return (len);
}

struct buf_ring {
	struct mtx *br_lock;
	int br_size;
	int br_head;
	int br_tail;
	void *br_elems[];
};

struct buf_ring *
buf_ring_alloc(int count, struct malloc_type *type, int flags,
    struct mtx *lock)
{
	struct buf_ring *br;

	(void)flags;
	br = malloc(sizeof(*br) + count * sizeof(void *), type, M_ZERO);
	if (br == NULL)
		return (NULL);
	br->br_size = count;
	br->br_lock = lock;
	return (br);
}

void
buf_ring_free(struct buf_ring *br, struct malloc_type *type)
{
	free(br, type);
}

int
buf_ring_enqueue(struct buf_ring *br, void *buf)
{
	int next;

	next = (br->br_tail + 1) % br->br_size;
	if (next == br->br_head)
		return (ENOBUFS);
	br->br_elems[br->br_tail] = buf;
	br->br_tail = next;
	return (0);
}

void *
buf_ring_dequeue_sc(struct buf_ring *br)
{
	void *buf;

	if (br->br_head == br->br_tail)
		return (NULL);
	buf = br->br_elems[br->br_head];
	br->br_head = (br->br_head + 1) % br->br_size;
	return (buf);
}

void *
buf_ring_peek(struct buf_ring *br)
{
	if (br->br_head == br->br_tail)
		return (NULL);
	return (br->br_elems[br->br_head]);
}

void *
buf_ring_peek_sc(struct buf_ring *br)
{
	return (buf_ring_peek(br));
}

void
buf_ring_putback_sc(struct buf_ring *br, void *new_buf)
{
	br->br_elems[br->br_head] = new_buf;
}

void *
buf_ring_advance_sc(struct buf_ring *br)
{
	if (br->br_head == br->br_tail)
		return (NULL);
	br->br_head = (br->br_head + 1) % br->br_size;
	return (buf_ring_peek(br));
}

int
buf_ring_full(struct buf_ring *br)
{
	return ((br->br_tail + 1) % br->br_size == br->br_head);
}

int
buf_ring_empty(struct buf_ring *br)
{
	return (br->br_head == br->br_tail);
}

int
buf_ring_count(struct buf_ring *br)
{
	int n;

	n = br->br_tail - br->br_head;
	if (n < 0)
		n += br->br_size;
	return (n);
}

int
drbr_enqueue(if_t ifp, struct buf_ring *br, struct mbuf *m)
{
	int err;

	(void)ifp;
	err = buf_ring_enqueue(br, m);
	if (err != 0)
		m_freem(m);
	return (err);
}

struct mbuf *
drbr_peek(if_t ifp, struct buf_ring *br)
{
	(void)ifp;
	return (buf_ring_peek(br));
}

void
drbr_putback(if_t ifp, struct buf_ring *br, struct mbuf *m)
{
	(void)ifp;
	buf_ring_putback_sc(br, m);
}

void
drbr_advance(if_t ifp, struct buf_ring *br)
{
	(void)ifp;
	(void)buf_ring_dequeue_sc(br);
}

struct mbuf *
drbr_dequeue(if_t ifp, struct buf_ring *br)
{
	(void)ifp;
	return (buf_ring_dequeue_sc(br));
}

int
drbr_empty(if_t ifp, struct buf_ring *br)
{
	(void)ifp;
	return (buf_ring_empty(br));
}

void
drbr_flush(if_t ifp, struct buf_ring *br)
{
	struct mbuf *m;

	(void)ifp;
	while ((m = buf_ring_dequeue_sc(br)) != NULL)
		m_freem(m);
}

struct taskqueue {
	struct mtx tq_mtx;
	TAILQ_HEAD(tq_head, task) tq_list;
	const char *tq_name;
	int tq_blocked;
	thread_call_t tq_call;
};

static void
taskqueue_drain_func(void *arg0, thread_call_param_t arg1)
{
	struct taskqueue *tq = arg0;
	struct task *t;

	(void)arg1;
	for (;;) {
		mtx_lock(&tq->tq_mtx);
		t = TAILQ_FIRST(&tq->tq_list);
		if (t != NULL) {
			TAILQ_REMOVE(&tq->tq_list, t, ta_link);
			t->ta_pending = 0;
		}
		mtx_unlock(&tq->tq_mtx);
		if (t == NULL)
			return;
		t->ta_func(t->ta_context, 0);
	}
}

struct taskqueue *
taskqueue_create(const char *name, int mflags, taskqueue_enqueue_fn enqueue,
    void *context)
{
	struct taskqueue *tq;

	(void)enqueue;
	(void)context;
	tq = malloc(sizeof(*tq), M_DEVBUF, M_ZERO | mflags);
	if (tq == NULL)
		return (NULL);
	tq->tq_name = name;
	mtx_init(&tq->tq_mtx, "taskqueue", NULL, MTX_DEF);
	TAILQ_INIT(&tq->tq_list);
	tq->tq_call = thread_call_allocate(taskqueue_drain_func, tq);
	return (tq);
}

void
taskqueue_free(struct taskqueue *queue)
{
	if (queue->tq_call != NULL)
		thread_call_free(queue->tq_call);
	mtx_destroy(&queue->tq_mtx);
	free(queue, M_DEVBUF);
}

int
taskqueue_enqueue(struct taskqueue *queue, struct task *task)
{
	int wake;

	if (queue == NULL || queue->tq_call == NULL)
		return (EOPNOTSUPP);
	mtx_lock(&queue->tq_mtx);
	if (queue->tq_blocked) {
		mtx_unlock(&queue->tq_mtx);
		return (0);
	}
	wake = task->ta_pending == 0;
	if (wake) {
		task->ta_pending = 1;
		TAILQ_INSERT_TAIL(&queue->tq_list, task, ta_link);
	}
	mtx_unlock(&queue->tq_mtx);
	if (wake)
		thread_call_enter(queue->tq_call);
	return (0);
}

void
taskqueue_drain(struct taskqueue *queue, struct task *task)
{
	while (task->ta_pending != 0)
		cpu_pause();
}

int
taskqueue_start_threads(struct taskqueue **queue, int count, int pri,
    const char *name, ...)
{
	(void)queue;
	(void)count;
	(void)pri;
	(void)name;
	shim_report_unimplemented("taskqueue_start_threads: runs on thread_call");
	return (0);
}

void
taskqueue_block(struct taskqueue *queue)
{
	mtx_lock(&queue->tq_mtx);
	queue->tq_blocked = 1;
	mtx_unlock(&queue->tq_mtx);
}

void
taskqueue_unblock(struct taskqueue *queue)
{
	mtx_lock(&queue->tq_mtx);
	queue->tq_blocked = 0;
	mtx_unlock(&queue->tq_mtx);
}

struct callout_impl {
	thread_call_t c_call;
	callout_func_t c_func;
	void *c_arg;
	int c_pending;
	struct callout *c_owner;
};

static void
callout_fire(void *arg0, thread_call_param_t arg1)
{
	struct callout_impl *ci = arg0;
	callout_func_t fn;
	void *carg;

	(void)arg1;
	ci->c_pending = 0;
	fn = ci->c_func;
	carg = ci->c_arg;
	if (fn != NULL)
		fn(carg);
}

void
callout_init(struct callout *c, int mpsafe)
{
	(void)mpsafe;
	c->c_impl = malloc(sizeof(*c->c_impl), M_DEVBUF, M_ZERO);
	c->c_impl->c_owner = c;
	c->c_impl->c_call = thread_call_allocate(callout_fire, c->c_impl);
}

void
callout_init_mtx(struct callout *c, void *mtx, int flags)
{
	(void)mtx;
	(void)flags;
	callout_init(c, 1);
}

int
callout_stop(struct callout *c)
{
	if (c->c_impl == NULL || c->c_impl->c_call == NULL)
		return (0);
	c->c_impl->c_func = NULL;
	return (thread_call_cancel(c->c_impl->c_call) ? 1 : 0);
}

int
callout_drain(struct callout *c)
{
	return (callout_stop(c));
}

int
callout_reset(struct callout *c, int ticks, callout_func_t fn, void *arg)
{
	if (c->c_impl == NULL || c->c_impl->c_call == NULL)
		callout_init(c, 1);
	callout_stop(c);
	c->c_impl->c_func = fn;
	c->c_impl->c_arg = arg;
	if (ticks <= 0) {
		c->c_impl->c_pending = 1;
		thread_call_enter(c->c_impl->c_call);
		return (1);
	}
	c->c_impl->c_pending = 1;
	thread_call_enter_delayed(c->c_impl->c_call,
	    (uint64_t)ticks * 10000000ULL);
	return (1);
}

int
callout_schedule(struct callout *c, int ticks)
{
	return (callout_reset(c, ticks, c->c_impl->c_func, c->c_impl->c_arg));
}

int
callout_active(struct callout *c)
{
	return (c->c_impl != NULL && c->c_impl->c_pending);
}

struct sglist {
	struct sglist_seg *sg_segs;
	uint16_t sg_nseg;
	uint16_t sg_maxseg;
	int sg_refcnt;
};

struct sglist *
sglist_alloc(int nsegs, int mflags)
{
	struct sglist *sg;

	sg = malloc(sizeof(*sg), M_DEVBUF, mflags);
	if (sg == NULL)
		return (NULL);
	sg->sg_segs = malloc(nsegs * sizeof(*sg->sg_segs), M_DEVBUF, mflags);
	if (sg->sg_segs == NULL) {
		free(sg, M_DEVBUF);
		return (NULL);
	}
	sg->sg_maxseg = (uint16_t)nsegs;
	sg->sg_refcnt = 1;
	return (sg);
}

void
sglist_free(struct sglist *sg)
{
	if (sg == NULL)
		return;
	free(sg->sg_segs, M_DEVBUF);
	free(sg, M_DEVBUF);
}

int
sglist_append(struct sglist *sg, void *buf, size_t len)
{
	struct sglist_seg *seg;

	if (sg->sg_nseg >= sg->sg_maxseg)
		return (ENOMEM);
	seg = &sg->sg_segs[sg->sg_nseg++];
	seg->ss_paddr = (uint64_t)(uintptr_t)buf;
	seg->ss_len = len;
	return (0);
}

int
sglist_append_mbuf(struct sglist *sg, struct mbuf *m)
{
	struct mbuf *p;
	int err;

	for (p = m; p != NULL; p = p->m_next) {
		if (p->m_len == 0)
			continue;
		if (p->m_ext != NULL && p->m_len == (int)p->m_ext->ext_size &&
		    p->m_data == p->m_ext->ext_buf)
			err = sglist_append(sg, p->m_ext->ext_buf, p->m_len);
		else
			err = sglist_append(sg, p->m_data, p->m_len);
		if (err != 0)
			return (err);
	}
	return (0);
}

int
sglist_count(void *buf, size_t len)
{
	(void)buf;
	(void)len;
	return (1);
}

size_t
sglist_length(struct sglist *sg)
{
	size_t len;
	int i;

	len = 0;
	for (i = 0; i < sg->sg_nseg; i++)
		len += sg->sg_segs[i].ss_len;
	return (len);
}

int
sglist_split(struct sglist *original, struct sglist *head, size_t length)
{
	size_t taken;
	int i;

	taken = 0;
	head->sg_nseg = 0;
	for (i = 0; i < original->sg_nseg; i++) {
		struct sglist_seg s;

		s = original->sg_segs[i];
		if (taken + s.ss_len <= length) {
			head->sg_segs[head->sg_nseg++] = s;
			taken += s.ss_len;
		} else if (length - taken > 0) {
			head->sg_segs[head->sg_nseg++] = s;
			taken += s.ss_len;
		}
	}
	original->sg_nseg = 0;
	return (0);
}

int
sbuf_new(struct sbuf *s, char *buf, int length, int flags)
{
	(void)flags;
	if (buf == NULL || length <= 0)
		return (-1);
	s->s_buf = buf;
	s->s_size = length;
	s->s_len = 0;
	buf[0] = 0;
	return (0);
}

int
sbuf_delete(struct sbuf *s)
{
	(void)s;
	return (0);
}

int
sbuf_cat(struct sbuf *s, const char *str)
{
	while (*str != 0 && s->s_len < s->s_size - 1)
		s->s_buf[s->s_len++] = *str++;
	s->s_buf[s->s_len] = 0;
	return (s->s_len < s->s_size - 1 ? 0 : -1);
}

int
sbuf_printf(struct sbuf *s, const char *fmt, ...)
{
	va_list ap;
	int n;

	if (s->s_len >= s->s_size - 1)
		return (-1);
	va_start(ap, fmt);
	n = vsnprintf(s->s_buf + s->s_len, s->s_size - s->s_len, fmt, ap);
	va_end(ap);
	s->s_len += n > s->s_size - 1 - s->s_len ? s->s_size - 1 - s->s_len : n;
	s->s_buf[s->s_len] = 0;
	return (0);
}

int
sbuf_finish(struct sbuf *s)
{
	s->s_buf[s->s_len] = 0;
	return (0);
}

char *
sbuf_data(struct sbuf *s)
{
	return (s->s_buf);
}

int
sbuf_len(struct sbuf *s)
{
	return (s->s_len);
}

int
sbuf_overflowed(struct sbuf *s)
{
	return (s->s_len >= s->s_size);
}

int
sbuf_clear(struct sbuf *s)
{
	s->s_len = 0;
	s->s_buf[0] = 0;
	return (0);
}

struct sysctl_ctx_entry *
sysctl_ctx_entry_add(struct sysctl_ctx_list *clist, struct sysctl_oid *oidp)
{
	(void)clist;
	(void)oidp;
	return (NULL);
}

struct sysctl_oid *
sysctl_add_oid(struct sysctl_ctx_list *clist, struct sysctl_oid *parent,
    int number, const char *name, int kind, void *arg1, intptr_t arg2,
    int (*handler)(sysctl_req_t), const char *fmt, const char *descr)
{
	struct sysctl_oid *oid;

	(void)clist;
	(void)parent;
	(void)number;
	(void)arg2;
	(void)fmt;
	(void)descr;
	(void)handler;
	shim_report_unimplemented("sysctl_add_oid: nodes not registered");
	oid = malloc(sizeof(*oid), M_TEMP, M_ZERO);
	if (oid == NULL)
		return (NULL);
	oid->oid_name = name;
	oid->oid_number = number;
	oid->oid_kind = kind;
	oid->oid_arg1 = arg1;
	TAILQ_INIT(&oid->oid_children);
	return (oid);
}

int
sysctl_ctx_init(sysctl_ctx_list_t *clist)
{
	TAILQ_INIT(clist);
	return (0);
}

int
sysctl_ctx_free(sysctl_ctx_list_t *clist)
{
	struct sysctl_ctx_entry *e;

	e = TAILQ_FIRST(clist);
	while (e != NULL) {
		free(e, M_TEMP);
		e = TAILQ_FIRST(clist);
	}
	return (0);
}

int
sysctl_handle_int(sysctl_req_t req)
{
	(void)req;
	shim_report_unimplemented("sysctl_handle_int");
	return (0);
}

int
sysctl_handle_string(sysctl_req_t req)
{
	(void)req;
	shim_report_unimplemented("sysctl_handle_string");
	return (0);
}

int
sysctl_handle_quad(sysctl_req_t req)
{
	(void)req;
	shim_report_unimplemented("sysctl_handle_quad");
	return (0);
}

int
sysctl_handle_opaque(sysctl_req_t req)
{
	(void)req;
	shim_report_unimplemented("sysctl_handle_opaque");
	return (0);
}

static uint32_t shim_rand_state = 0x1234abcd;

int
read_random(void *buf, int count)
{
	uint8_t *p;
	int i;

	p = buf;
	for (i = 0; i < count; i++) {
		shim_rand_state ^= shim_rand_state << 13;
		shim_rand_state ^= shim_rand_state >> 17;
		shim_rand_state ^= shim_rand_state << 5;
		p[i] = (uint8_t)shim_rand_state;
	}
	return (count);
}

void
arc4rand(void *buf, unsigned int len, int reseed)
{
	(void)reseed;
	(void)read_random(buf, (int)len);
}

uint32_t
arc4random(void)
{
	uint32_t v;

	arc4rand(&v, sizeof(v), 0);
	return (v);
}

device_t
device_get_parent(device_t dev)
{
	return (dev->dev_parent);
}

void *
device_get_softc(device_t dev)
{
	return (dev->dev_softc);
}

void
device_set_softc(device_t dev, void *softc, size_t size)
{
	dev->dev_softc = softc;
	dev->dev_softcsize = size;
}

const char *
device_get_desc(device_t dev)
{
	return (dev->dev_desc);
}

void
device_set_desc(device_t dev, const char *desc)
{
	snprintf(dev->dev_desc, sizeof(dev->dev_desc), "%s", desc);
}

void
device_set_desc_copy(device_t dev, const char *desc)
{
	device_set_desc(dev, desc);
}

int
device_get_unit(device_t dev)
{
	return (dev->dev_unit);
}

const char *
device_get_name(device_t dev)
{
	return (dev->dev_driver != NULL ? dev->dev_driver->name : NULL);
}

const char *
device_get_nameunit(device_t dev)
{
	return (dev->dev_nameunit);
}

void *
device_get_ivars(device_t dev)
{
	return (dev->dev_ivars);
}

void
device_set_ivars(device_t dev, void *ivars)
{
	dev->dev_ivars = ivars;
}

int
device_printf(device_t dev, const char *fmt, ...)
{
	char buf[256];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	if (dev->dev_nameunit[0] != 0)
		return (printf("%s: %s", dev->dev_nameunit, buf));
	else if (dev->dev_driver != NULL)
		return (printf("%s?: %s", dev->dev_driver->name, buf));
	return (printf("%s", buf));
}

int
device_is_alive(device_t dev)
{
	return (dev->dev_flags >= DS_ATTACHED);
}

void
device_quiesce(device_t dev)
{
	(void)dev;
}

void
device_busy(device_t dev)
{
	dev->dev_flags++;
}

void
device_unbusy(device_t dev)
{
	dev->dev_flags--;
}

int
device_delete_child(device_t dev, device_t child)
{
	(void)dev;
	(void)child;
	return (0);
}

int
device_detach(device_t dev)
{
	(void)dev;
	return (0);
}

device_t
device_add_child(device_t dev, const char *name, int unit)
{
	device_t child;
	static int next_unit;

	child = malloc(sizeof(struct device), M_DEVBUF, M_ZERO | M_NOWAIT);
	if (child == NULL)
		return (NULL);
	sysctl_ctx_init(&child->dev_sysctl_ctx);
	child->dev_sysctl_tree = sysctl_add_oid(&child->dev_sysctl_ctx,
	    NULL, OID_AUTO, name != NULL ? name : "device", CTLFLAG_RD,
	    NULL, 0, NULL, "N", "");
	child->dev_parent = dev;
	child->dev_flags = DS_NOTPRESENT;
	if (unit < 0)
		unit = next_unit++;
	child->dev_unit = unit;
	if (name != NULL)
		snprintf(child->dev_nameunit, sizeof(child->dev_nameunit),
		    "%s%d", name, unit);
	else
		snprintf(child->dev_nameunit, sizeof(child->dev_nameunit),
		    "%d", unit);
	return (child);
}

device_state_t
device_get_state(device_t dev)
{
	return (dev->dev_flags);
}

int
device_probe(device_t dev)
{
	int (*fn)(device_t);
	int error;

	fn = bus_method_lookup(dev, "device_probe");
	if (fn == NULL)
		return (ENXIO);
	error = fn(dev);
	if (error == 0)
		dev->dev_flags = DS_ALIVE;
	return (error);
}

int
device_attach(device_t dev)
{
	int (*fn)(device_t);
	int error;

	fn = bus_method_lookup(dev, "device_attach");
	if (fn == NULL)
		return (ENXIO);
	error = fn(dev);
	if (error == 0)
		dev->dev_flags = DS_ATTACHED;
	return (error);
}

static void *
bus_method_lookup(device_t dev, const char *name)
{
	device_method_t *m;

	if (dev == NULL || dev->dev_driver == NULL)
		return (NULL);
	for (m = dev->dev_driver->methods; m->name != NULL; m++)
		if (strcmp(m->name, name) == 0)
			return (m->func);
	return (NULL);
}

int
BUS_READ_IVAR(device_t dev, device_t child, int which, uintptr_t *result)
{
	int (*fn)(device_t, device_t, int, uintptr_t *);

	fn = bus_method_lookup(dev, "bus_read_ivar");
	if (fn == NULL)
		return (ENXIO);
	return (fn(dev, child, which, result));
}

int
BUS_WRITE_IVAR(device_t dev, device_t child, int which, uintptr_t value)
{
	int (*fn)(device_t, device_t, int, uintptr_t);

	fn = bus_method_lookup(dev, "bus_write_ivar");
	if (fn == NULL)
		return (ENXIO);
	return (fn(dev, child, which, value));
}

int
bus_generic_suspend(device_t dev)
{
	(void)dev;
	return (0);
}

int
bus_generic_resume(device_t dev)
{
	(void)dev;
	return (0);
}

int
bus_generic_attach(device_t dev)
{
	(void)dev;
	return (0);
}

int
bus_generic_detach(device_t dev)
{
	(void)dev;
	return (0);
}

int
bus_generic_probe(device_t dev)
{
	(void)dev;
	return (0);
}

void
bus_generic_shutdown(device_t dev)
{
	(void)dev;
}

void
bus_topo_lock(void)
{
}

void
bus_topo_unlock(void)
{
}

struct resource {
	rman_res_t r_start;
	rman_res_t r_size;
	int r_flags;
	bus_space_handle_t r_handle;
};

uint64_t
rman_get_start(struct resource *r)
{
	return (r->r_start);
}

uint64_t
rman_get_size(struct resource *r)
{
	return (r->r_size);
}

int
rman_get_flags(struct resource *r)
{
	return (r->r_flags);
}

uint8_t
bus_read_1(struct resource *r, bus_size_t ofs)
{
	return (bus_space_read_1(NULL, r->r_handle, ofs));
}

uint16_t
bus_read_2(struct resource *r, bus_size_t ofs)
{
	return (bus_space_read_2(NULL, r->r_handle, ofs));
}

uint32_t
bus_read_4(struct resource *r, bus_size_t ofs)
{
	return (bus_space_read_4(NULL, r->r_handle, ofs));
}

void
bus_write_1(struct resource *r, bus_size_t ofs, uint8_t value)
{
	bus_space_write_1(NULL, r->r_handle, ofs, value);
}

void
bus_write_2(struct resource *r, bus_size_t ofs, uint16_t value)
{
	bus_space_write_2(NULL, r->r_handle, ofs, value);
}

void
bus_write_4(struct resource *r, bus_size_t ofs, uint32_t value)
{
	bus_space_write_4(NULL, r->r_handle, ofs, value);
}

uint64_t
vtophys(void *va)
{
	return ((uint64_t)(uintptr_t)va);
}

int
rman_activate_resource(struct resource *r)
{
	(void)r;
	return (0);
}

int
rman_deactivate_resource(struct resource *r)
{
	(void)r;
	return (0);
}

int
rman_release_resource(struct resource *r)
{
	free(r, M_DEVBUF);
	return (0);
}

struct resource *
rman_reserve_resource(struct rman *rm, rman_res_t start, rman_res_t end,
    rman_res_t count, uint32_t flags, device_t child, device_t provider,
    void *softc)
{
	(void)rm;
	(void)child;
	(void)provider;
	(void)softc;
	(void)end;
	shim_report_unimplemented("rman_reserve_resource");
	return (NULL);
}

struct resource *
bus_alloc_resource_any(device_t dev, int type, int *rid, u_int flags)
{
	(void)dev;
	(void)type;
	(void)rid;
	(void)flags;
	shim_report_unimplemented("bus_alloc_resource_any: no PCI config space");
	return (NULL);
}

int
bus_release_resource(device_t dev, int type, int rid, struct resource *r)
{
	(void)dev;
	(void)type;
	(void)rid;
	(void)r;
	return (0);
}

int
bus_setup_intr(device_t dev, struct resource *r, int flags,
    void (*filter)(void *), void (*handler)(void *), void *arg,
    void **cookiep)
{
	(void)dev;
	(void)r;
	(void)flags;
	(void)filter;
	(void)handler;
	(void)arg;
	(void)cookiep;
	shim_report_unimplemented("bus_setup_intr: no interrupt routing");
	return (EOPNOTSUPP);
}

int
bus_teardown_intr(device_t dev, struct resource *r, void *cookie)
{
	(void)dev;
	(void)r;
	(void)cookie;
	return (0);
}

bus_dma_tag_t
bus_get_dma_tag(device_t dev)
{
	(void)dev;
	return (NULL);
}

int
bus_child_present(device_t dev)
{
	(void)dev;
	return (1);
}

struct bus_dma_tag {
	bus_size_t alignment;
	bus_size_t boundary;
	bus_addr_t lowaddr;
	bus_size_t maxsize;
	int nsegments;
	bus_size_t maxsegsz;
};

struct bus_dmamap {
	bus_dma_tag_t map_tag;
};

int
bus_dma_tag_create(bus_dma_tag_t parent, bus_size_t alignment,
    bus_size_t boundary, bus_addr_t lowaddr, bus_addr_t highaddr,
    bus_dma_filter_t *filter, void *filterarg, bus_size_t maxsize,
    int nsegments, bus_size_t maxsegsz, int flags, bus_dma_lock_t *lockfn,
    void *lockarg, bus_dma_tag_t *dmat)
{
	bus_dma_tag_t tag;

	(void)parent;
	(void)highaddr;
	(void)filter;
	(void)filterarg;
	(void)flags;
	(void)lockfn;
	(void)lockarg;
	tag = malloc(sizeof(*tag), M_DEVBUF, M_ZERO);
	if (tag == NULL)
		return (ENOMEM);
	tag->alignment = alignment;
	tag->boundary = boundary;
	tag->lowaddr = lowaddr;
	tag->maxsize = maxsize;
	tag->nsegments = nsegments;
	tag->maxsegsz = maxsegsz;
	*dmat = tag;
	return (0);
}

int
bus_dma_tag_destroy(bus_dma_tag_t tag)
{
	free(tag, M_DEVBUF);
	return (0);
}

int
bus_dmamap_create(bus_dma_tag_t tag, int flags, bus_dmamap_t *mapp)
{
	bus_dmamap_t map;

	(void)flags;
	map = malloc(sizeof(*map), M_DEVBUF, M_ZERO);
	if (map == NULL)
		return (ENOMEM);
	map->map_tag = tag;
	*mapp = map;
	return (0);
}

int
bus_dmamap_destroy(bus_dma_tag_t tag, bus_dmamap_t map)
{
	(void)tag;
	free(map, M_DEVBUF);
	return (0);
}

int
bus_dmamap_load(bus_dma_tag_t tag, bus_dmamap_t map, void *buf,
    bus_size_t buflen, bus_dmamap_callback_t *callback, void *arg,
    int flags)
{
	(void)tag;
	(void)map;
	(void)buf;
	(void)buflen;
	(void)flags;
	shim_report_unimplemented("bus_dmamap_load: no DMA translation");
	callback(arg, NULL, 0, EOPNOTSUPP);
	return (EOPNOTSUPP);
}

int
bus_dmamap_load_mbuf(bus_dma_tag_t tag, bus_dmamap_t map,
    struct mbuf *mb, bus_dmamap_callback_t *callback, void *arg,
    int flags)
{
	(void)tag;
	(void)map;
	(void)mb;
	(void)flags;
	shim_report_unimplemented("bus_dmamap_load_mbuf: no DMA translation");
	callback(arg, NULL, 0, EOPNOTSUPP);
	return (EOPNOTSUPP);
}

int
bus_dmamap_load_mbuf_sg(bus_dma_tag_t tag, bus_dmamap_t map,
    struct mbuf *mb, bus_dma_segment_t *segs, int *nsegs, int flags)
{
	(void)tag;
	(void)map;
	(void)mb;
	(void)flags;
	shim_report_unimplemented("bus_dmamap_load_mbuf_sg: no DMA translation");
	*nsegs = 0;
	return (EOPNOTSUPP);
}

void
bus_dmamap_unload(bus_dma_tag_t tag, bus_dmamap_t map)
{
	(void)tag;
	(void)map;
}

void
bus_dmamap_sync(bus_dma_tag_t tag, bus_dmamap_t map, int op)
{
	(void)tag;
	(void)map;
	(void)op;
}

int
bus_dmamem_alloc(bus_dma_tag_t tag, void **vaddr, int flags,
    bus_dmamap_t *mapp)
{
	void *v;

	(void)tag;
	v = malloc(tag != NULL ? (unsigned long)tag->maxsize : PAGE_SIZE,
	    M_DEVBUF, M_ZERO | (flags & (M_WAITOK | M_NOWAIT)));
	if (v == NULL)
		return (ENOMEM);
	*vaddr = v;
	shim_report_unimplemented("bus_dmamem_alloc: memory is not DMA-safe");
	return (bus_dmamap_create(tag, flags, mapp));
}

void
bus_dmamem_free(bus_dma_tag_t tag, void *vaddr, bus_dmamap_t map)
{
	(void)tag;
	bus_dmamap_destroy(tag, map);
	free(vaddr, M_DEVBUF);
}

static void *
vtpci_bus_ops(device_t dev, const char *ifname)
{
	(void)ifname;
	return (dev->dev_bus_ops);
}

void *
device_bus_ops(device_t dev, const char *ifname)
{
	return (vtpci_bus_ops(dev, ifname));
}

uint16_t
pci_get_vendor(device_t dev)
{
	(void)dev;
	shim_report_unimplemented("pci_get_vendor");
	return (0);
}

uint16_t
pci_get_device(device_t dev)
{
	(void)dev;
	shim_report_unimplemented("pci_get_device");
	return (0);
}

uint16_t
pci_get_subvendor(device_t dev)
{
	(void)dev;
	return (0);
}

uint16_t
pci_get_subdevice(device_t dev)
{
	(void)dev;
	return (0);
}

uint8_t
pci_get_revid(device_t dev)
{
	(void)dev;
	return (0);
}

uint8_t
pci_get_class(device_t dev)
{
	(void)dev;
	return (0);
}

uint8_t
pci_get_subclass(device_t dev)
{
	(void)dev;
	return (0);
}

uint32_t
pci_read_config(device_t dev, int reg, int width)
{
	(void)dev;
	(void)reg;
	(void)width;
	shim_report_unimplemented("pci_read_config");
	return (0xffffffffu);
}

void
pci_write_config(device_t dev, int reg, uint32_t value, int width)
{
	(void)dev;
	(void)reg;
	(void)value;
	(void)width;
	shim_report_unimplemented("pci_write_config");
}

int
pci_enable_busmaster(device_t dev)
{
	(void)dev;
	shim_report_unimplemented("pci_enable_busmaster");
	return (EOPNOTSUPP);
}

int
pci_disable_busmaster(device_t dev)
{
	(void)dev;
	return (0);
}

int
pci_enable_io(device_t dev, int space)
{
	(void)dev;
	(void)space;
	return (0);
}

int
pci_disable_io(device_t dev, int space)
{
	(void)dev;
	(void)space;
	return (0);
}

int
pci_find_cap(device_t dev, int capability, int *startreg)
{
	(void)dev;
	(void)startreg;
	if (capability == PCIY_MSI || capability == PCIY_MSIX)
		return (0);
	shim_report_unimplemented("pci_find_cap");
	return (ENXIO);
}

int
pci_alloc_msi(device_t dev, int *count)
{
	(void)dev;
	(void)count;
	shim_report_unimplemented("pci_alloc_msi");
	return (ENODEV);
}

int
pci_alloc_msix(device_t dev, int *count)
{
	(void)dev;
	(void)count;
	shim_report_unimplemented("pci_alloc_msix");
	return (ENODEV);
}

int
pci_msi_count(device_t dev)
{
	(void)dev;
	return (1);
}

int
pci_msix_count(device_t dev)
{
	(void)dev;
	return (1);
}

int
pci_release_msi(device_t dev)
{
	(void)dev;
	return (0);
}

int
pci_get_max_read_req(device_t dev)
{
	(void)dev;
	return (512);
}

int
pci_set_max_read_req(device_t dev, int size)
{
	(void)dev;
	(void)size;
	return (0);
}

int
pci_get_powerstate(device_t dev)
{
	(void)dev;
	return (0);
}

int
pci_set_powerstate(device_t dev, int state)
{
	(void)dev;
	(void)state;
	return (0);
}

void
link_init_sdl(if_t ifp, struct sockaddr_dl *sdl, u_char iftype)
{
	(void)ifp;
	(void)iftype;
	bzero(sdl, sizeof(*sdl));
	sdl->sdl_family = AF_LINK;
	sdl->sdl_len = sizeof(*sdl);
}

struct sockaddr_dl *
if_lladdr(if_t ifp)
{
	return (&ifp->if_lladdr);
}

void
shim_vlan_capab(struct ifnet *ifp)
{
	(void)ifp;
}

static uint32_t shim_ifmedia_cur = IFM_ETHER | IFM_1000_T | IFM_FDX;

int
ifmedia_init(struct ifmedia *ifm, int dontcare_mask, ifm_change_cb_t change,
    ifm_status_cb_t status)
{
	(void)dontcare_mask;
	(void)change;
	(void)status;
	TAILQ_INIT(&ifm->ifm_list);
	ifm->ifm_cur = NULL;
	return (0);
}

void
ifmedia_removeall(struct ifmedia *ifm)
{
	ifm->ifm_cur = NULL;
}

int
ifmedia_add(struct ifmedia *ifm, int mword, int data, void *rsp)
{
	struct ifmedia_entry *e;

	(void)data;
	(void)rsp;
	e = malloc(sizeof(*e), M_DEVBUF, M_ZERO);
	if (e == NULL)
		return (ENOMEM);
	e->ifm_media = mword;
	TAILQ_INSERT_TAIL(&ifm->ifm_list, e, ifm_list);
	ifm->ifm_cur = e;
	return (0);
}

void
ifmedia_set(struct ifmedia *ifm, int target)
{
	(void)ifm;
	(void)target;
}

int
ifmedia_ioctl(if_t ifp, void *ifr, struct ifmedia *ifm, u_long cmd)
{
	struct ifmediareq *req = ifr;

	(void)ifp;
	(void)cmd;
	if (req == NULL)
		return (EINVAL);
	req->ifm_current = shim_ifmedia_cur;
	req->ifm_active = shim_ifmedia_cur;
	req->ifm_status = IFM_AVALID | IFM_ACTIVE;
	req->ifm_count = 0;
	return (0);
}

void
ether_ifattach(if_t ifp, const uint8_t *lladdr)
{
	ifp->if_hdrlen = ETHER_HDR_LEN;
	ifp->if_mtu = ETHERMTU;
	if (lladdr != NULL)
		bcopy(lladdr, LLADDR(&ifp->if_lladdr), ETHER_ADDR_LEN);
	if_attach(ifp);
}

void
ether_ifdetach(if_t ifp)
{
	if_detach(ifp);
}

int
ether_ioctl(if_t ifp, u_long cmd, caddr_t data)
{
	struct ifreq *ifr = (struct ifreq *)data;
	int error;

	(void)cmd;
	error = 0;
	return (error);
	(void)ifr;
}

struct mbuf *
ether_vlanencap(struct mbuf *m, uint16_t vid)
{
	struct mbuf *n;
	uint8_t *p;

	n = m_get(M_NOWAIT, MT_HEADER);
	if (n == NULL) {
		m_freem(m);
		return (NULL);
	}
	n->m_len = ETHER_HDR_LEN + ETHER_VLAN_ENCAP_LEN;
	p = mtod(n, uint8_t *);
	bcopy(mtod(m, caddr_t), p, ETHER_ADDR_LEN * 2);
	p[ETHER_ADDR_LEN * 2] = 0x81;
	p[ETHER_ADDR_LEN * 2 + 1] = 0x00;
	p[ETHER_ADDR_LEN * 2 + 2] = (uint8_t)(vid >> 8);
	p[ETHER_ADDR_LEN * 2 + 3] = (uint8_t)vid;
	bcopy(mtod(m, caddr_t) + ETHER_ADDR_LEN * 2,
	    p + ETHER_HDR_LEN + ETHER_VLAN_ENCAP_LEN, 2);
	n->m_next = m;
	m_adj(m, ETHER_ADDR_LEN * 2);
	if ((m->m_flags & M_PKTHDR) != 0) {
		n->m_flags |= M_PKTHDR;
		n->m_pkthdr = m->m_pkthdr;
		n->m_pkthdr.len += ETHER_VLAN_ENCAP_LEN;
	}
	return (n);
}

uint64_t
uma_zone_get_cur(uma_zone_t zone)
{
	(void)zone;
	shim_report_unimplemented("uma_zone_get_cur");
	return (0);
}

int
sglist_init(struct sglist *sg, int nsegs, struct sglist_seg *segs)
{
	sg->sg_segs = (struct sglist_seg *)segs;
	sg->sg_nseg = 0;
	sg->sg_maxseg = (u_int)nsegs;
	sg->sg_refcnt = 1;
	return (0);
}

int
sglist_reset(struct sglist *sg)
{
	sg->sg_nseg = 0;
	return (0);
}

int
sglist_append_sglist(struct sglist *sg, struct sglist *src, size_t offset,
    size_t length)
{
	size_t taken;
	int i;

	taken = 0;
	for (i = 0; i < src->sg_nseg; i++) {
		struct sglist_seg *s;

		s = &src->sg_segs[i];
		if (taken + s->ss_len <= offset) {
			taken += s->ss_len;
			continue;
		}
		if (length == 0)
			break;
		if (sglist_append(sg, (void *)(uintptr_t)s->ss_paddr,
		    s->ss_len) != 0)
			return (ENOMEM);
		length -= s->ss_len;
		taken += s->ss_len;
	}
	return (0);
}

int
device_is_attached(device_t dev)
{
	return (dev->dev_flags >= DS_ATTACHED);
}

struct mbuf *
m_defrag(struct mbuf *m, int how)
{
	struct mbuf *head;
	struct mbuf *tail;
	int off;
	int len;
	int want;

	if (m == NULL || m->m_next == NULL)
		return (m);
	len = 0;
	for (tail = m; tail != NULL; tail = tail->m_next)
		len += tail->m_len;
	head = NULL;
	tail = NULL;
	off = 0;
	while (off < len) {
		struct mbuf *c;

		c = m_getcl(how, m->m_type, 0);
		if (c == NULL) {
			m_freem(head);
			return (NULL);
		}
		if (head == NULL)
			head = c;
		else
			tail->m_next = c;
		tail = c;
		while (off < len && c->m_len < MCLBYTES) {
			want = len - off < MCLBYTES - c->m_len ?
			    len - off : MCLBYTES - c->m_len;
			m_copydata(m, off, want, mtod(c, caddr_t) + c->m_len);
			off += want;
			c->m_len += want;
		}
	}
	if ((m->m_flags & M_PKTHDR) != 0) {
		head->m_flags |= M_PKTHDR;
		head->m_pkthdr = m->m_pkthdr;
	}
	m_freem(m);
	return (head);
}

struct mbuf *
m_collapse(struct mbuf *m, int how, int maxfrags)
{
	struct mbuf *n;
	int frags;

	if (m == NULL)
		return (NULL);
	frags = 0;
	for (n = m; n != NULL; n = n->m_next)
		frags++;
	if (frags <= maxfrags)
		return (m);
	return (m_defrag(m, how));
}

void
m_copydata(struct mbuf *m, int offset, int len, caddr_t dst)
{
	while (m != NULL && len > 0) {
		if (m->m_len == 0) {
			m = m->m_next;
			continue;
		}
		if (offset >= m->m_len) {
			offset -= m->m_len;
			m = m->m_next;
			continue;
		}
		{
			int take;

			take = m->m_len - offset;
			if (take > len)
				take = len;
			bcopy(m->m_data + offset, dst, take);
			dst += take;
			len -= take;
			offset = 0;
			m = m->m_next;
		}
	}
}
