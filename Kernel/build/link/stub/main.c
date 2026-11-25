typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef const char *cstr;

static inline void outb(u16 port, u8 val)
{
	__asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline u64 rdtsc(void)
{
	u32 lo, hi;

	__asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
	return ((u64)hi << 32) | lo;
}

static u64 rdrand_or_tsc(void)
{
	u32 r;
	u8 ok;
	u32 eax, ebx, ecx, edx;

	__asm__ volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(1u));
	if (ecx & (1u << 30)) {
		for (int tries = 0; tries < 16; tries++) {
			__asm__ volatile("rdrand %0\n\tsetc %1" : "=r"(r), "=r"(ok));
			if (ok)
				return ((u64)r << 32) | rdtsc();
		}
	}
	return rdtsc() ^ (u64)(unsigned long)&outb;
}

unsigned long __stack_chk_guard;

__attribute__((noreturn)) void __stack_chk_fail(void);

static void com1_write(cstr s)
{
	while (*s != '\0') {
		outb(0x3f8, (u8)*s++);
	}
}

__attribute__((noinline)) static void canary_probe(void)
{
	volatile char buf[64];

	for (int i = 0; i < 64; i++)
		buf[i] = (char)i;
}

__attribute__((noinline)) void _start(void);

__attribute__((noreturn)) void __stack_chk_fail(void)
{
	com1_write("ravynOS KERNEL PANIC: stack smashing detected\r\n");
	for (;;)
		__asm__ volatile("cli; hlt");
}

void _start(void)
{
	__stack_chk_guard = rdrand_or_tsc();
	if (__stack_chk_guard == 0)
		__stack_chk_guard = 0x726176796e4f5330UL;
	canary_probe();
	com1_write("ravynOS WP-A2b kernel stub: Mach-O link OK, canary on\r\n");
	for (;;) {
		__asm__ volatile("hlt");
	}
}
