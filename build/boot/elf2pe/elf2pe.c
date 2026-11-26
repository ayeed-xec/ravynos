// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 The ravynOS Authors
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define FILE_ALIGNMENT 0x200u
#define SECTION_ALIGNMENT 0x1000u
#define PE32PLUS_MAGIC 0x20bu
#define SUBSYSTEM_EFI_APPLICATION 10u
#define EXECUTABLE_IMAGE 0x0002u
#define IMAGE_BASE 0
#define SCN_CODE 0x60000020u
#define SCN_DATA 0x40000040u
#define SCN_BSS 0xc0000080u

#define ELFCLASS64 2
#define ELFDATA2LSB 1
#define ET_EXEC 2
#define EM_386 3
#define EM_X86_64 62
#define EM_AARCH64 183
#define EM_RISCV 243
#define SHT_PROGBITS 1
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_RELA 4
#define SHF_ALLOC 0x2u

struct sec {
	const char *name;
	uint32_t va;
	uint32_t size;
	uint32_t off;
	uint32_t raw;
	int nobits;
};

static uint8_t *elf;
static size_t elf_len;
static uint8_t *pe;

static uint64_t erd64(size_t o) { uint64_t v = 0; for (int i = 7; i >= 0; i--) v = (v << 8) | elf[o + i]; return v; }
static uint32_t erd32(size_t o) { return (uint32_t)elf[o] | ((uint32_t)elf[o+1] << 8) | ((uint32_t)elf[o+2] << 16) | ((uint32_t)elf[o+3] << 24); }
static uint16_t erd16(size_t o) { return (uint16_t)(elf[o] | (elf[o+1] << 8)); }

static void w16(size_t o, uint16_t v) { pe[o] = (uint8_t)(v & 0xff); pe[o+1] = (uint8_t)(v >> 8); }
static void w32(size_t o, uint32_t v) { pe[o] = (uint8_t)(v & 0xff); pe[o+1] = (uint8_t)((v >> 8) & 0xff); pe[o+2] = (uint8_t)((v >> 16) & 0xff); pe[o+3] = (uint8_t)((v >> 24) & 0xff); }
static uint32_t align_up(uint32_t v, uint32_t a) { return (v + a - 1) & ~(a - 1); }

static void die(const char *msg, const char *path)
{
	fprintf(stderr, "elf2pe: %s: %s\n", path, msg);
	exit(1);
}

static uint16_t pe_machine(uint16_t em)
{
	switch (em) {
	case EM_X86_64: return 0x8664;
	case EM_AARCH64: return 0xaa64;
	case EM_RISCV: return 0x5064;
	default: return 0;
	}
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "usage: elf2pe <in-elf> <out-efi>\n");
		return 2;
	}
	FILE *f = fopen(argv[1], "rb");
	if (f == NULL)
		die("cannot open input", argv[1]);
	fseek(f, 0, SEEK_END);
	long sz = ftell(f);
	fseek(f, 0, SEEK_SET);
	if (sz <= 0)
		die("empty input", argv[1]);
	elf_len = (size_t)sz;
	elf = malloc(elf_len);
	if (elf == NULL || fread(elf, 1, elf_len, f) != elf_len)
		die("cannot read input", argv[1]);
	fclose(f);

	if (elf_len < 64 || memcmp(elf, "\177ELF", 4) != 0)
		die("not an ELF file", argv[1]);
	if (elf[4] != ELFCLASS64 || elf[5] != ELFDATA2LSB)
		die("only ELF64 little-endian input is supported", argv[1]);
	if (erd16(16) != ET_EXEC)
		die("input must be a linked executable", argv[1]);

	uint16_t machine = pe_machine(erd16(18));
	if (machine == 0)
		die("unsupported machine type", argv[1]);

	uint64_t entry = erd64(24);
	if (entry == 0 || entry > 0xffffffffu)
		die("entry point out of range", argv[1]);

	uint64_t shoff = erd64(40);
	uint16_t shentsize = erd16(58);
	uint16_t shnum = erd16(60);
	if (shnum == 0 || shentsize < 64 || shoff + (uint64_t)shnum * shentsize > elf_len)
		die("section table out of range", argv[1]);

	struct sec *secs = calloc(shnum, sizeof(*secs));
	if (secs == NULL)
		die("out of memory", argv[1]);

	uint16_t count = 0;
	uint32_t image_end = 0;
	for (uint16_t i = 0; i < shnum; i++) {
		size_t s = (size_t)shoff + (size_t)i * shentsize;
		uint32_t type = erd32(s + 4);
		uint64_t flags = erd64(s + 8);
		uint64_t addr = erd64(s + 16);
		uint64_t off = erd64(s + 24);
		uint64_t size = erd64(s + 32);

		if (type == SHT_REL || type == SHT_RELA)
			die("input carries relocations; link it without them first", argv[1]);
		if ((flags & SHF_ALLOC) == 0 || size == 0)
			continue;
		if (addr > 0xffffffffu || size > 0xffffffffu)
			die("section does not fit a 32-bit image", argv[1]);
		if (type != SHT_PROGBITS && type != SHT_NOBITS)
			die("allocatable section of unsupported type", argv[1]);
		if (type == SHT_PROGBITS && off + size > elf_len)
			die("section data out of range", argv[1]);

		secs[count].va = (uint32_t)addr;
		secs[count].size = (uint32_t)size;
		secs[count].off = (uint32_t)off;
		secs[count].nobits = (type == SHT_NOBITS);
		secs[count].raw = secs[count].nobits ? 0 : align_up((uint32_t)size, FILE_ALIGNMENT);
		if (addr > 0xffffffffu - size)
			die("section end overflows the image", argv[1]);
		if ((uint32_t)addr + (uint32_t)size > image_end)
			image_end = (uint32_t)addr + (uint32_t)size;
		count++;
	}
	if (count == 0)
		die("no allocatable sections", argv[1]);

	uint32_t optsz = 240;
	uint32_t headers_end = 0x40 + 4 + 20 + optsz + (uint32_t)count * 40;
	uint32_t size_of_headers = align_up(headers_end, FILE_ALIGNMENT);
	uint32_t size_of_image = align_up(image_end, SECTION_ALIGNMENT);

	for (uint16_t i = 0; i < count; i++)
		if (secs[i].va < size_of_headers)
			die("section virtual address below SizeOfHeaders", argv[1]);

	uint32_t raw_cursor = size_of_headers;
	for (uint16_t i = 0; i < count; i++) {
		if (secs[i].nobits)
			continue;
		secs[i].raw = raw_cursor;
		raw_cursor += align_up(secs[i].size, FILE_ALIGNMENT);
	}

	size_t pe_len = raw_cursor;
	pe = calloc(1, pe_len);
	if (pe == NULL)
		die("out of memory", argv[1]);

	pe[0] = 'M';
	pe[1] = 'Z';
	w32(0x3c, 0x40);

	size_t fh = 0x40 + 4;
	pe[0x40] = 'P';
	pe[0x41] = 'E';
	w16(fh + 0, machine);
	w16(fh + 2, count);
	w16(fh + 16, (uint16_t)optsz);
	w16(fh + 18, EXECUTABLE_IMAGE);

	size_t opt = fh + 20;
	w16(opt + 0, PE32PLUS_MAGIC);
	w32(opt + 16, (uint32_t)entry);
	w32(opt + 24, IMAGE_BASE);
	w32(opt + 28, 0);
	w32(opt + 32, SECTION_ALIGNMENT);
	w32(opt + 36, FILE_ALIGNMENT);
	w32(opt + 56, size_of_image);
	w32(opt + 60, size_of_headers);
	w32(opt + 64, 0);
	w16(opt + 68, SUBSYSTEM_EFI_APPLICATION);
	w16(opt + 70, 0);
	w32(opt + 108, 16);

	size_t st = opt + optsz;
	for (uint16_t i = 0; i < count; i++) {
		size_t s = st + (size_t)i * 40;
		uint32_t chars = secs[i].nobits ? SCN_BSS : (i == 0 ? SCN_CODE : SCN_DATA);
		w32(s + 8, secs[i].size);
		w32(s + 12, secs[i].va);
		w32(s + 16, secs[i].nobits ? 0 : align_up(secs[i].size, FILE_ALIGNMENT));
		w32(s + 20, secs[i].raw);
		w32(s + 36, chars);
		if (!secs[i].nobits)
			memcpy(pe + secs[i].raw, elf + secs[i].off, secs[i].size);
	}

	FILE *o = fopen(argv[2], "wb");
	if (o == NULL)
		die("cannot open output", argv[2]);
	if (fwrite(pe, 1, pe_len, o) != pe_len)
		die("cannot write output", argv[2]);
	if (fclose(o) != 0)
		die("cannot close output", argv[2]);

	printf("elf2pe=%s machine=%#x sections=%u entry=%#x size_of_image=%#x size_of_headers=%#x\n",
	       argv[2], machine, (unsigned)count, (uint32_t)entry, size_of_image, size_of_headers);
	free(secs);
	free(pe);
	free(elf);
	return 0;
}
