// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
#include "kernelimg.h"

#define RV_MACHO_MAGIC64 0xfeedfacf
#if defined(__x86_64__)
#define RV_MACHO_CPU_HOST 0x01000007
#elif defined(__riscv)
#define RV_MACHO_CPU_HOST 0
#else
#define RV_MACHO_CPU_HOST 0x0100000c
#endif
#define RV_LC_SEGMENT64 0x19
#define RV_LC_UNIXTHREAD 0x5
#define RV_LC_MAIN 0x80000028

#define RV_SEGNAME_TEXT "__TEXT"

static uint32_t rd32(const uint8_t *p)
{
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static uint64_t rd64(const uint8_t *p)
{
	return (uint64_t)rd32(p) | ((uint64_t)rd32(p + 4) << 32);
}

static uint64_t macho_phys(uint64_t vaddr)
{
	if (vaddr >= 0xffffff8000000000ull)
		return vaddr - 0xffffff8000000000ull;
	return vaddr;
}

static int segname_is_text(const uint8_t *seg)
{
	const uint8_t *name = seg + 8;
	for (int i = 0; i < 7; i++) {
		if (name[i] != (uint8_t)RV_SEGNAME_TEXT[i])
			return 0;
	}
	return name[7] == 0;
}

static int segname_is_pagezero(const uint8_t *seg)
{
	const uint8_t *name = seg + 8;
	for (int i = 0; i < 9; i++) {
		if (name[i] != (uint8_t)"__PAGEZERO"[i])
			return 0;
	}
	return 1;
}

int rv_parse_macho(const uint8_t *data, uint64_t size, struct rv_kernel_image *image)
{
	if (size < 32 || rd32(data) != RV_MACHO_MAGIC64)
		return -1;
	if (rd32(data + 4) != RV_MACHO_CPU_HOST)
		return -1;

	uint32_t ncmds = rd32(data + 16);
	uint32_t sizeofcmds = rd32(data + 20);
	if (ncmds == 0 || sizeofcmds == 0)
		return -1;
	if (32ull + sizeofcmds > size)
		return -1;

	uint64_t min_addr = UINT64_MAX;
	uint64_t max_addr = 0;
	uint64_t entry_vaddr = 0;
	uint64_t text_vaddr = 0;
	int have_entry = 0;
	int have_text = 0;
	uint64_t offset = 32;

	for (uint32_t i = 0; i < ncmds; i++) {
		if (offset + 8 > size)
			return -1;
		uint32_t cmd = rd32(data + offset);
		uint32_t cmdsize = rd32(data + offset + 4);
		if (cmdsize < 8 || offset + cmdsize > size)
			return -1;

		if (cmd == RV_LC_SEGMENT64 && cmdsize >= 72) {
			if (segname_is_pagezero(data + offset)) {
				offset += cmdsize;
				continue;
			}
			uint64_t vmaddr = rd64(data + offset + 24);
			uint64_t memsize = rd64(data + offset + 32);
			uint64_t fileoff = rd64(data + offset + 40);
			uint64_t filesize = rd64(data + offset + 48);
			if (memsize == 0) {
				offset += cmdsize;
				continue;
			}
			if (image->segment_count >= RV_MAX_SEGMENTS) {
				offset += cmdsize;
				continue;
			}
			if (fileoff > size || filesize > size - fileoff)
				return -1;
			uint64_t paddr = macho_phys(vmaddr);
			if (paddr + memsize < paddr)
				return -1;
			if (!have_text && segname_is_text(data + offset)) {
				text_vaddr = vmaddr;
				have_text = 1;
			}
			if (paddr < min_addr)
				min_addr = paddr;
			if (paddr + memsize > max_addr)
				max_addr = paddr + memsize;
			struct rv_segment *seg = &image->segments[image->segment_count++];
			seg->paddr = paddr;
			seg->file_offset = fileoff;
			seg->file_size = filesize;
			seg->mem_size = memsize;
		} else if (cmd == RV_LC_UNIXTHREAD && cmdsize >= 184) {
#if defined(__x86_64__)
			uint32_t flavor = rd32(data + offset + 8);
			if (flavor == 4 && !have_entry) {
				entry_vaddr = rd64(data + offset + 16 + 16 * 8);
				have_entry = 1;
			}
#elif !defined(__riscv)
			uint32_t flavor = rd32(data + offset + 8);
			if (flavor == 6 && cmdsize >= 16 + 268 && !have_entry) {
				entry_vaddr = rd64(data + offset + 16 + 32 * 8);
				have_entry = 1;
			}
#endif
		} else if (cmd == RV_LC_MAIN && cmdsize >= 24 && !have_entry) {
			if (!have_text)
				return -1;
			entry_vaddr = text_vaddr + rd64(data + offset + 8);
			have_entry = 1;
		}
		offset += cmdsize;
	}

	if (image->segment_count == 0 || !have_entry)
		return -1;

	uint64_t entry_paddr = macho_phys(entry_vaddr);
	if (entry_paddr < min_addr || entry_paddr >= max_addr)
		return -1;

	image->kind = RV_KERNEL_MACHO;
	image->entry_paddr = entry_paddr;
	image->base_paddr = min_addr;
	image->end_paddr = max_addr;
	return 0;
}
