// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 The ravynOS Authors
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define SECTION_ALIGNMENT 0x1000u
#define FILE_ALIGNMENT 0x200u
#define SUBSYSTEM_EFI_APPLICATION 10u
#define RELOCS_STRIPPED 0x0001u
#define EXECUTABLE_IMAGE 0x0002u
#define PE32PLUS_MAGIC 0x20bu

static uint8_t *buf;
static size_t buf_len;

static uint16_t rd16(size_t off)
{
	return (uint16_t)(buf[off] | ((uint16_t)buf[off + 1] << 8));
}

static uint32_t rd32(size_t off)
{
	return (uint32_t)buf[off] | ((uint32_t)buf[off + 1] << 8) |
	       ((uint32_t)buf[off + 2] << 16) | ((uint32_t)buf[off + 3] << 24);
}

static void wr16(size_t off, uint16_t v)
{
	buf[off] = (uint8_t)(v & 0xff);
	buf[off + 1] = (uint8_t)(v >> 8);
}

static void wr32(size_t off, uint32_t v)
{
	buf[off] = (uint8_t)(v & 0xff);
	buf[off + 1] = (uint8_t)((v >> 8) & 0xff);
	buf[off + 2] = (uint8_t)((v >> 16) & 0xff);
	buf[off + 3] = (uint8_t)((v >> 24) & 0xff);
}

static uint32_t align_up(uint32_t v, uint32_t a)
{
	return (v + a - 1) & ~(a - 1);
}

static void die(const char *msg, const char *path)
{
	fprintf(stderr, "pefix: %s: %s\n", path, msg);
	exit(1);
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "usage: pefix <in-pe> <out-pe>\n");
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
	buf_len = (size_t)sz;
	buf = malloc(buf_len);
	if (buf == NULL || fread(buf, 1, buf_len, f) != buf_len)
		die("cannot read input", argv[1]);
	fclose(f);

	if (buf_len < 0x40)
		die("too small for a PE header", argv[1]);
	uint32_t pe = rd32(0x3c);
	if (pe + 24 + 2 > buf_len)
		die("PE header offset out of range", argv[1]);
	if (memcmp(buf + pe, "PE\0\0", 4) != 0)
		die("missing PE signature", argv[1]);

	size_t fh = pe + 4;
	size_t opt = pe + 24;
	uint16_t nsec = rd16(fh + 2);
	uint16_t optsz = rd16(fh + 16);
	if (rd16(opt) != PE32PLUS_MAGIC)
		die("not a PE32+ image", argv[1]);
	if (opt + optsz > buf_len)
		die("optional header out of range", argv[1]);

	size_t so = opt + optsz;
	if (so + (size_t)nsec * 40 > buf_len)
		die("section table out of range", argv[1]);
	if (rd16(opt + 108) < 6)
		die("no base relocation directory slot", argv[1]);

	uint32_t start = rd32(opt + 16);
	if (start == 0)
		die("entry point is zero", argv[1]);

	uint32_t image_end = 0;
	uint32_t min_raw = UINT32_MAX;
	for (uint16_t i = 0; i < nsec; i++) {
		size_t s = so + (size_t)i * 40;
		uint32_t vs = rd32(s + 8);
		uint32_t va = rd32(s + 12);
		uint32_t rawsz = rd32(s + 16);
		uint32_t raw = rd32(s + 20);
		uint32_t end = va + (vs > rawsz ? vs : rawsz);
		if (end > image_end)
			image_end = end;
		if (rawsz != 0 && raw < min_raw)
			min_raw = raw;
	}
	if (image_end == 0)
		die("image has no sections", argv[1]);

	uint32_t header_end = (uint32_t)(so + (size_t)nsec * 40);
	uint32_t size_of_headers = align_up(header_end, FILE_ALIGNMENT);
	if (min_raw != UINT32_MAX && min_raw < size_of_headers)
		size_of_headers = min_raw;
	if (size_of_headers < header_end)
		die("section data overlaps the PE headers", argv[1]);

	uint32_t size_of_image = align_up(image_end, SECTION_ALIGNMENT);

	for (uint16_t i = 0; i < nsec; i++) {
		size_t s = so + (size_t)i * 40;
		uint32_t va = rd32(s + 12);
		uint32_t rawsz = rd32(s + 16);
		uint32_t raw = rd32(s + 20);
		if (rawsz == 0)
			continue;
		if (va < size_of_headers)
			die("section virtual address below SizeOfHeaders", argv[1]);
		if (raw < size_of_headers)
			die("section raw data below SizeOfHeaders", argv[1]);
	}

	wr16(fh + 18, (uint16_t)((rd16(fh + 18) | EXECUTABLE_IMAGE) & ~RELOCS_STRIPPED));
	wr32(opt + 24, 0);
	wr32(opt + 28, 0);
	wr32(opt + 32, SECTION_ALIGNMENT);
	wr32(opt + 36, FILE_ALIGNMENT);
	wr32(opt + 56, size_of_image);
	wr32(opt + 60, size_of_headers);
	wr32(opt + 64, 0);
	wr16(opt + 68, SUBSYSTEM_EFI_APPLICATION);
	wr16(opt + 70, 0);
	wr32(opt + 112 + 5 * 8, 0);
	wr32(opt + 112 + 5 * 8 + 4, 0);

	FILE *o = fopen(argv[2], "wb");
	if (o == NULL)
		die("cannot open output", argv[2]);
	if (fwrite(buf, 1, buf_len, o) != buf_len)
		die("cannot write output", argv[2]);
	if (fclose(o) != 0)
		die("cannot close output", argv[2]);

	printf("pefix=%s size_of_image=%#x size_of_headers=%#x entry=%#x sections=%u\n",
	       argv[2], size_of_image, size_of_headers, start, (unsigned)nsec);
	free(buf);
	return 0;
}
