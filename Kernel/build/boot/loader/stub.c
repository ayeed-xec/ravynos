#include <stdint.h>

#define MB2_MAGIC 0xE85250D6u
#define MB2_HDR_LEN 28u
#define MB2_CHECKSUM (uint32_t)(-(MB2_MAGIC + 0u + MB2_HDR_LEN))

__attribute__((section(".mboot2"), used, aligned(8)))
static const uint32_t mb2_header[] = {
    MB2_MAGIC,
    0u,
    MB2_HDR_LEN,
    MB2_CHECKSUM,
    0u,
    8u,
};

#define COM1 0x3F8

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t v;
    __asm__ volatile ("inb %1, %0" : "=a"(v) : "Nd"(port));
    return v;
}

static void uart_init(void)
{
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);
    outb(COM1 + 0, 0x01);
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x03);
}

static void uart_putc(char c)
{
    while (!(inb(COM1 + 5) & 0x20))
        ;
    outb(COM1, (uint8_t)c);
}

static void uart_puts(const char *s)
{
    while (*s)
        uart_putc(*s++);
}

void kmain(uint32_t magic, uint32_t mbaddr)
{
    uart_init();
    uart_puts("RAVYNOS-BOOT-OK WP-A2c multiboot2 COM1\n");
    uart_puts("grub magic=");
    static const char hex[] = "0123456789abcdef";
    char buf[11];
    buf[0] = '0';
    buf[1] = 'x';
    for (int i = 0; i < 8; i++)
        buf[2 + i] = hex[(magic >> (28 - i * 4)) & 0xF];
    buf[10] = '\n';
    uart_puts(buf);
    (void)mbaddr;

    for (;;)
        __asm__ volatile ("hlt");
}
