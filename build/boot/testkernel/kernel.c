// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 The ravynOS Authors
#include <stdint.h>
#include "util.h"
#include "boot_args.h"

#define RV_PANIC_MAGIC 0x52564e50u

void *rv_frames[8];
uint32_t rv_frame_count;

static void rv_print(const char *s)
{
	rv_serial_puts(s);
	rv_serial_puts("\n");
}

__attribute__((noinline)) static void rv_panic(const char *reason, struct rv_boot_args *ba)
{
	rv_frames[rv_frame_count++] = __builtin_return_address(0);

	rv_serial_puts("RVPANIC BEGIN magic=");
	rv_serial_hex16(RV_PANIC_MAGIC);
	rv_serial_puts(" rev=1\n");
	rv_serial_puts("RVPSTR ");
	rv_print(reason);
	rv_serial_puts("RVBA ptr=");
	rv_serial_hex64((uint64_t)ba);
	rv_serial_puts(" version=");
	rv_serial_hex16(ba->Version);
	rv_serial_puts(" efiMode=");
	rv_serial_hex16(ba->efiMode);
	rv_serial_puts(" mmap_size=");
	rv_serial_hex16(ba->MemoryMapSize);
	rv_serial_puts(" mmap_dsize=");
	rv_serial_hex16(ba->MemoryMapDescriptorSize);
	rv_serial_puts(" kaddr=");
	rv_serial_hex16(ba->kaddr);
	rv_serial_puts(" ksize=");
	rv_serial_hex16(ba->ksize);
	rv_serial_puts("\n");

	uint32_t entries = 0;
	uint64_t conventional_pages = 0;
	if (ba->MemoryMapDescriptorSize && ba->MemoryMap) {
		entries = ba->MemoryMapSize / ba->MemoryMapDescriptorSize;
		const uint8_t *base = (const uint8_t *)(uint64_t)ba->MemoryMap;
		for (uint32_t i = 0; i < entries; i++) {
			const uint8_t *d = base + (uint64_t)i * ba->MemoryMapDescriptorSize;
			uint32_t type = (uint32_t)d[0] | ((uint32_t)d[1] << 8) | ((uint32_t)d[2] << 16) | ((uint32_t)d[3] << 24);
			uint64_t pages = (uint64_t)d[16] | ((uint64_t)d[17] << 8) | ((uint64_t)d[18] << 16)
				| ((uint64_t)d[19] << 24) | ((uint64_t)d[20] << 32) | ((uint64_t)d[21] << 40)
				| ((uint64_t)d[22] << 48) | ((uint64_t)d[23] << 56);
			if (type == 7)
				conventional_pages += pages;
		}
	}
	rv_serial_puts("RVMEM entries=");
	rv_serial_hex16(entries);
	rv_serial_puts(" conventional_pages=");
	rv_serial_hex16((uint32_t)conventional_pages);
	rv_serial_puts("\n");

	for (uint32_t i = 0; i < rv_frame_count; i++) {
		rv_serial_puts("RVFRAME ");
		rv_serial_hex64((uint64_t)rv_frames[i]);
		rv_serial_puts("\n");
	}
	rv_print("RVPANIC END");

	for (;;)
#if defined(__x86_64__)
		__asm__ volatile("cli; hlt");
#elif defined(__riscv)
		__asm__ volatile("wfi");
#else
		__asm__ volatile("wfe");
#endif
}

__attribute__((noinline)) static void probe_layer3(struct rv_boot_args *ba)
{
	rv_frames[rv_frame_count++] = __builtin_return_address(0);
	rv_panic("probe-reached", ba);
}

__attribute__((noinline)) static void probe_layer2(struct rv_boot_args *ba)
{
	rv_frames[rv_frame_count++] = __builtin_return_address(0);
	probe_layer3(ba);
}

__attribute__((noinline)) static void probe_layer1(struct rv_boot_args *ba)
{
	rv_frames[rv_frame_count++] = __builtin_return_address(0);
	probe_layer2(ba);
}

void kmain(struct rv_boot_args *ba)
{
	rv_serial_init();
	rv_print("RVTEST boot");
	if (ba == 0)
		rv_panic("bootargs-null", ba);
	if (ba->Version != RV_BOOT_ARGS_VERSION)
		rv_panic("bootargs-version-bad", ba);
	if (ba->efiMode != RV_BOOT_ARGS_EFIMODE64)
		rv_panic("bootargs-efimode-bad", ba);
	if (ba->MemoryMap == 0 || ba->MemoryMapSize == 0 || ba->MemoryMapDescriptorSize == 0)
		rv_panic("bootargs-mmap-empty", ba);
	if (ba->kaddr == 0 || ba->ksize == 0)
		rv_panic("bootargs-kaddr-empty", ba);
	rv_print("RVTEST handoff-ok");
	probe_layer1(ba);
	rv_panic("fell-off-end", ba);
}
