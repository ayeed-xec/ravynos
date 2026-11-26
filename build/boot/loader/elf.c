// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 The ravynOS Authors
#include "kernelimg.h"

static uint16_t rd16(const uint8_t *p)
{
	return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p)
{
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static uint64_t rd64(const uint8_t *p)
{
	return (uint64_t)rd32(p) | ((uint64_t)rd32(p + 4) << 32);
}

int rv_parse_elf(const uint8_t *data, uint64_t size, struct rv_kernel_image *image)
{
	if (size < 64 || data[0] != 0x7f || data[1] != 'E' || data[2] != 'L' || data[3] != 'F')
		return -1;
	if (data[4] != 2 || data[5] != 1)
		return -1;
#if defined(__x86_64__)
	if (rd16(data + 18) != 62)
#else
	if (rd16(data + 18) != 183)
#endif
		return -1;

	uint64_t phoff = rd64(data + 32);
	uint16_t phentsize = rd16(data + 54);
	uint16_t phnum = rd16(data + 56);
	uint64_t entry = rd64(data + 24);

	if (phoff == 0 || phnum == 0 || phnum > 128 || phentsize < 56)
		return -1;
	if (phoff + (uint64_t)phnum * phentsize > size)
		return -1;

	uint64_t min_addr = UINT64_MAX;
	uint64_t max_addr = 0;

	for (uint16_t i = 0; i < phnum; i++) {
		const uint8_t *ph = data + phoff + (uint64_t)i * phentsize;
		uint32_t p_type = rd32(ph);
		if (p_type != 1)
			continue;
		uint64_t p_offset = rd64(ph + 8);
		uint64_t p_vaddr = rd64(ph + 16);
		uint64_t p_paddr = rd64(ph + 24);
		uint64_t p_filesz = rd64(ph + 32);
		uint64_t p_memsz = rd64(ph + 40);

		if (p_memsz == 0)
			continue;
		uint64_t target = p_paddr ? p_paddr : p_vaddr;
		if (p_filesz > p_memsz)
			return -1;
		if (p_offset + p_filesz > size)
			return -1;
		if (image->segment_count >= RV_MAX_SEGMENTS)
			return -1;
		if (target < min_addr)
			min_addr = target;
		if (target + p_memsz > max_addr)
			max_addr = target + p_memsz;
		struct rv_segment *seg = &image->segments[image->segment_count++];
		seg->paddr = target;
		seg->file_offset = p_offset;
		seg->file_size = p_filesz;
		seg->mem_size = p_memsz;
	}

	if (image->segment_count == 0 || entry < min_addr || entry >= max_addr)
		return -1;

	image->kind = RV_KERNEL_ELF;
	image->entry_paddr = entry;
	image->base_paddr = min_addr;
	image->end_paddr = max_addr;
	return 0;
}
