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
#define IMAGE_BASE 0u
#define SCN_CODE 0x60000020u
#define SCN_DATA 0x40000040u
#define SCN_BSS 0xc0000080u
#define PE_MACHINE_AMD64 0x8664u
#define PE_MACHINE_ARM64 0xaa64u
#define PE_MACHINE_RISCV64 0x5064u

#define DOS_HEADER_SIZE 0x40u
#define DOS_LFANEW 0x3cu
#define PE_SIGNATURE_SIZE 4u
#define COFF_HEADER_SIZE 20u
#define OPTIONAL_HEADER_SIZE 240u
#define SECTION_HEADER_SIZE 40u
#define NUMBER_OF_RVA_AND_SIZES 16u

#define ELF64_EHDR_SIZE 64u
#define ELF64_SHDR_SIZE 64u
#define ELFCLASS64 2
#define ELFDATA2LSB 1
#define ET_EXEC 2
#define EM_X86_64 62
#define EM_AARCH64 183
#define EM_RISCV 243
#define SHT_PROGBITS 1
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_RELA 4
#define SHF_ALLOC 0x2u

struct sec {
	uint32_t va;
	uint32_t size;
	uint64_t off;
	uint32_t raw;
	int nobits;
};

static uint8_t *elf;
static size_t elf_len;
static uint8_t *pe;
static const char *input_path;

static uint16_t erd16(size_t o)
{
	return (uint16_t)(elf[o] | (elf[o + 1] << 8));
}

static uint32_t erd32(size_t o)
{
	return (uint32_t)elf[o] | ((uint32_t)elf[o + 1] << 8) | ((uint32_t)elf[o + 2] << 16) |
	       ((uint32_t)elf[o + 3] << 24);
}

static uint64_t erd64(size_t o)
{
	return (uint64_t)erd32(o) | ((uint64_t)erd32(o + 4) << 32);
}

static void w16(size_t o, uint32_t v)
{
	pe[o] = (uint8_t)(v & 0xff);
	pe[o + 1] = (uint8_t)((v >> 8) & 0xff);
}

static void w32(size_t o, uint32_t v)
{
	w16(o, v & 0xffff);
	w16(o + 2, v >> 16);
}

static uint32_t align_up(uint32_t v, uint32_t a)
{
	return (v + a - 1) & ~(a - 1);
}

static void die(const char *msg, const char *path)
{
	fprintf(stderr, "elf2pe: %s: %s\n", path, msg);
	exit(1);
}

static uint16_t pe_machine(uint16_t em)
{
	switch (em) {
	case EM_X86_64:
		return PE_MACHINE_AMD64;
	case EM_AARCH64:
		return PE_MACHINE_ARM64;
	case EM_RISCV:
		return PE_MACHINE_RISCV64;
	default:
		return 0;
	}
}

static int range_ok(uint64_t off, uint64_t len)
{
	return off <= elf_len && len <= elf_len - off;
}

static void read_input(const char *path)
{
	FILE *f = fopen(path, "rb");
	if (f == NULL)
		die("cannot open input", path);
	if (fseek(f, 0, SEEK_END) != 0)
		die("cannot seek input", path);
	long sz = ftell(f);
	if (sz < 0 || fseek(f, 0, SEEK_SET) != 0)
		die("cannot size input", path);
	if (sz == 0)
		die("empty input", path);
	elf_len = (size_t)sz;
	elf = malloc(elf_len);
	if (elf == NULL || fread(elf, 1, elf_len, f) != elf_len)
		die("cannot read input", path);
	fclose(f);
}

static uint16_t collect_sections(struct sec *secs, uint64_t shoff, uint16_t shnum, uint16_t shentsize,
				 uint32_t *image_end)
{
	uint16_t count = 0;
	*image_end = 0;
	for (uint16_t i = 0; i < shnum; i++) {
		size_t s = (size_t)shoff + (size_t)i * shentsize;
		uint32_t type = erd32(s + 4);
		uint64_t flags = erd64(s + 8);
		uint64_t addr = erd64(s + 16);
		uint64_t off = erd64(s + 24);
		uint64_t size = erd64(s + 32);

		if (type == SHT_REL || type == SHT_RELA)
			die("input carries relocations; link it without them first", input_path);
		if ((flags & SHF_ALLOC) == 0 || size == 0)
			continue;
		if (addr > UINT32_MAX || size > UINT32_MAX || addr > UINT32_MAX - size)
			die("section does not fit a 32-bit image", input_path);
		if (type != SHT_PROGBITS && type != SHT_NOBITS)
			die("allocatable section of unsupported type", input_path);
		if (type == SHT_PROGBITS && !range_ok(off, size))
			die("section data out of range", input_path);

		struct sec *sc = &secs[count++];
		sc->va = (uint32_t)addr;
		sc->size = (uint32_t)size;
		sc->off = off;
		sc->nobits = type == SHT_NOBITS;
		if (sc->va + sc->size > *image_end)
			*image_end = sc->va + sc->size;
	}
	return count;
}

static void write_headers(uint16_t machine, uint16_t count, uint32_t entry, uint32_t size_of_image,
			  uint32_t size_of_headers)
{
	pe[0] = 'M';
	pe[1] = 'Z';
	w32(DOS_LFANEW, DOS_HEADER_SIZE);
	pe[DOS_HEADER_SIZE] = 'P';
	pe[DOS_HEADER_SIZE + 1] = 'E';

	size_t fh = DOS_HEADER_SIZE + PE_SIGNATURE_SIZE;
	w16(fh + 0, machine);
	w16(fh + 2, count);
	w16(fh + 16, OPTIONAL_HEADER_SIZE);
	w16(fh + 18, EXECUTABLE_IMAGE);

	size_t opt = fh + COFF_HEADER_SIZE;
	w16(opt + 0, PE32PLUS_MAGIC);
	w32(opt + 16, entry);
	w32(opt + 24, IMAGE_BASE);
	w32(opt + 32, SECTION_ALIGNMENT);
	w32(opt + 36, FILE_ALIGNMENT);
	w32(opt + 56, size_of_image);
	w32(opt + 60, size_of_headers);
	w16(opt + 68, SUBSYSTEM_EFI_APPLICATION);
	w32(opt + 108, NUMBER_OF_RVA_AND_SIZES);
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "usage: elf2pe <in-elf> <out-efi>\n");
		return 2;
	}
	input_path = argv[1];
	read_input(input_path);

	if (elf_len < ELF64_EHDR_SIZE || memcmp(elf, "\177ELF", 4) != 0)
		die("not an ELF file", input_path);
	if (elf[4] != ELFCLASS64 || elf[5] != ELFDATA2LSB)
		die("only ELF64 little-endian input is supported", input_path);
	if (erd16(16) != ET_EXEC)
		die("input must be a linked executable", input_path);

	uint16_t machine = pe_machine(erd16(18));
	if (machine == 0)
		die("unsupported machine type", input_path);

	uint64_t entry = erd64(24);
	if (entry == 0 || entry > UINT32_MAX)
		die("entry point out of range", input_path);

	uint64_t shoff = erd64(40);
	uint16_t shentsize = erd16(58);
	uint16_t shnum = erd16(60);
	if (shnum == 0 || shentsize < ELF64_SHDR_SIZE || !range_ok(shoff, (uint64_t)shnum * shentsize))
		die("section table out of range", input_path);

	struct sec *secs = calloc(shnum, sizeof(*secs));
	if (secs == NULL)
		die("out of memory", input_path);

	uint32_t image_end = 0;
	uint16_t count = collect_sections(secs, shoff, shnum, shentsize, &image_end);
	if (count == 0)
		die("no allocatable sections", input_path);
	if (image_end > UINT32_MAX - SECTION_ALIGNMENT)
		die("image does not fit a 32-bit image", input_path);

	uint32_t headers_end = DOS_HEADER_SIZE + PE_SIGNATURE_SIZE + COFF_HEADER_SIZE + OPTIONAL_HEADER_SIZE +
			       (uint32_t)count * SECTION_HEADER_SIZE;
	uint32_t size_of_headers = align_up(headers_end, FILE_ALIGNMENT);
	uint32_t size_of_image = align_up(image_end, SECTION_ALIGNMENT);

	uint64_t raw_cursor = size_of_headers;
	for (uint16_t i = 0; i < count; i++) {
		if (secs[i].va < size_of_headers)
			die("section virtual address below SizeOfHeaders", input_path);
		if (secs[i].nobits)
			continue;
		secs[i].raw = (uint32_t)raw_cursor;
		raw_cursor += align_up(secs[i].size, FILE_ALIGNMENT);
		if (raw_cursor > UINT32_MAX)
			die("output does not fit a 32-bit image", input_path);
	}

	size_t pe_len = (size_t)raw_cursor;
	pe = calloc(1, pe_len);
	if (pe == NULL)
		die("out of memory", input_path);

	write_headers(machine, count, (uint32_t)entry, size_of_image, size_of_headers);

	size_t st = DOS_HEADER_SIZE + PE_SIGNATURE_SIZE + COFF_HEADER_SIZE + OPTIONAL_HEADER_SIZE;
	for (uint16_t i = 0; i < count; i++) {
		size_t s = st + (size_t)i * SECTION_HEADER_SIZE;
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

	printf("elf2pe=%s machine=%#x sections=%u entry=%#x size_of_image=%#x size_of_headers=%#x\n", argv[2],
	       machine, (unsigned)count, (uint32_t)entry, size_of_image, size_of_headers);
	free(secs);
	free(pe);
	free(elf);
	return 0;
}
