typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#define COM1 0x3f8

#define MB2_TAG_END 0
#define MB2_TAG_MODULE 3

#define MACHO_MAGIC 0xfeedfacf
#define LC_SEGMENT_64 0x19
#define LC_UNIXTHREAD 0x5

#define PAYLOAD_BASE 0x100000000ULL
#define PTE_PRESENT 0x1ULL
#define PTE_WRITE 0x2ULL
#define PTE_NX (0x1ULL << 63)
#define VM_PROT_READ 1
#define VM_PROT_WRITE 2
#define VM_PROT_EXEC 4

extern u64 pt_payload[];

extern char stack_top[];

static void outb(u16 p, u8 v) { __asm__ volatile("outb %0, %w1" : : "a"(v), "Nd"(p)); }
static u8 inb(u16 p) { u8 r; __asm__ volatile("inb %w1, %0" : "=a"(r) : "Nd"(p)); return r; }

static void serial_init(void) {
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);
    outb(COM1, 0x01);
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);
}

static void putc_(char c) {
    while (!(inb(COM1 + 5) & 0x20))
        ;
    outb(COM1, (u8)c);
}

static void puts_(const char *s) { while (*s) putc_(*s++); }

static void puthex(u64 v) {
    for (int i = 15; i >= 0; i--) {
        u32 d = (v >> (i * 4)) & 0xf;
        putc_(d < 10 ? '0' + d : 'a' + d - 10);
    }
}

static void fail(const char *msg) {
    puts_(msg);
    for (;;)
        ;
}

struct mb2_tag {
    u32 type;
    u32 size;
};

struct mb2_module {
    u32 type;
    u32 size;
    u32 mod_start;
    u32 mod_end;
    u32 cmdline;
    u32 pad;
};

void c_entry(u32 mbi, u32 magic) {
    serial_init();
    puts_("macho-loader: 64-bit C entry, grub magic=0x");
    puthex(magic);
    puts_("\n");
    /*
     * VA 0x100000000 is the high-half identity of physical 0-2MB
     * (pml3_low[4] -> pdt_high[0] -> pt_payload): PTE address = i*4096.
     * Default present|write|nx until segment perms land.
     */
    for (u32 i = 0; i < 512; i++)
        pt_payload[i] = (u64)i * 4096 | PTE_PRESENT | PTE_WRITE | PTE_NX;
    u8 *p = (u8 *)(u64)mbi + 8;
    struct mb2_module *mod = 0;
    for (;;) {
        struct mb2_tag *t = (struct mb2_tag *)p;
        if (t->type == MB2_TAG_END)
            break;
        if (t->type == MB2_TAG_MODULE && !mod)
            mod = (struct mb2_module *)p;
        p += (t->size + 7) & ~7u;
    }
    if (!mod)
        fail("macho-loader: ERROR no module in multiboot2 boot info\n");
    u8 *base = (u8 *)(u64)mod->mod_start;
    u32 total = mod->mod_end - mod->mod_start;
    puts_("macho-loader: module 0x");
    puthex(mod->mod_start);
    puts_(" size 0x");
    puthex(total);
    puts_("\n");
    u32 *hdr = (u32 *)base;
    if (hdr[0] != MACHO_MAGIC)
        fail("macho-loader: ERROR module is not a 64-bit Mach-O\n");
    u32 ncmds = hdr[4];
    u32 off = 32;
    u64 entry = 0;
    u64 entry_rsp = 0;
    for (u32 i = 0; i < ncmds; i++) {
        u32 cmd = *(u32 *)(base + off);
        u32 csz = *(u32 *)(base + off + 4);
        if (csz < 8)
            fail("macho-loader: ERROR load command size < 8 (corrupt module)\n");
        if (cmd == LC_SEGMENT_64) {
            u64 vmaddr = *(u64 *)(base + off + 24);
            u64 vmsize = *(u64 *)(base + off + 32);
            u64 fileoff = *(u64 *)(base + off + 40);
            u64 filesize = *(u64 *)(base + off + 48);
            u32 initprot = *(u32 *)(base + off + 60);
            puts_("macho-loader: segment 0x");
            puthex(vmaddr);
            puts_(" filesz 0x");
            puthex(filesize);
            puts_(" memsz 0x");
            puthex(vmsize);
            puts_(" initprot ");
            putc_('0' + initprot);
            puts_("\n");
            if (vmsize != 0 && vmaddr != 0) {
                if (vmaddr < PAYLOAD_BASE ||
                    vmaddr + vmsize > PAYLOAD_BASE + 2 * 1024 * 1024)
                    fail("macho-loader: ERROR segment outside 2MB payload window\n");
                u8 *dst = (u8 *)vmaddr;
                u8 *src = base + fileoff;
                for (u64 i = 0; i < filesize; i++)
                    dst[i] = src[i];
                for (u64 i = filesize; i < vmsize; i++)
                    dst[i] = 0;
                /* per-segment perms: exec pages lose write, data pages lose exec */
                u32 first = (u32)((vmaddr - PAYLOAD_BASE) >> 12);
                u32 last = (u32)(((vmaddr - PAYLOAD_BASE) + vmsize - 1) >> 12);
                for (u32 i = first; i <= last; i++) {
                    u64 e = (u64)i * 4096 | PTE_PRESENT;
                    if (initprot & VM_PROT_WRITE) {
                        if (initprot & VM_PROT_EXEC)
                            fail("macho-loader: ERROR segment is write+execute (W^X)\n");
                        e |= PTE_WRITE | PTE_NX;
                    } else if (!(initprot & VM_PROT_EXEC)) {
                        e |= PTE_NX;
                    }
                    pt_payload[i] = e;
                }
            }
        } else if (cmd == LC_UNIXTHREAD) {
            u64 *regs = (u64 *)(base + off + 16);
            entry_rsp = regs[7];
            entry = regs[16];
        }
        off += csz;
    }
    if (!entry)
        fail("macho-loader: ERROR no LC_UNIXTHREAD entry point\n");
    puts_("macho-loader: entry 0x");
    puthex(entry);
    puts_(" rsp 0x");
    puthex(entry_rsp);
    puts_("\n");
    {
        volatile u8 *chk = (u8 *)0x100000000;
        volatile u8 *chke = (u8 *)entry;
        puts_("macho-loader: readback @0x100000000: ");
        puthex(chk[0]); puthex(chk[1]); puthex(chk[2]); puthex(chk[3]);
        puts_(" @entry: ");
        puthex(chke[0]); puthex(chke[1]); puthex(chke[2]); puthex(chke[3]);
        puts_("\n");
    }
    puts_("macho-loader: jumping to guest kernel\n");
    if (!entry_rsp)
        entry_rsp = (u64)stack_top;
    __asm__ volatile("mov %1, %%rsp\n\txor %%rbp, %%rbp\n\tjmp *%0" : : "r"(entry), "r"(entry_rsp));
    fail("macho-loader: ERROR jump returned\n");
}
