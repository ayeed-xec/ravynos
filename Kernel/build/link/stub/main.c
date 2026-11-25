typedef unsigned char u8;
typedef unsigned short u16;
typedef const char *cstr;

static inline void outb(u16 port, u8 val)
{
	__asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static void com1_write(cstr s)
{
	while (*s != '\0') {
		outb(0x3f8, (u8)*s++);
	}
}

__attribute__((noinline)) void _start(void);

void _start(void)
{
	com1_write("ravynOS WP-A2b kernel stub: Mach-O link OK\r\n");
	for (;;) {
		__asm__ volatile("hlt");
	}
}
