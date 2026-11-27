// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 The ravynOS Authors
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define SECTOR 512u
#define SECTORS_PER_CLUSTER 8u
#define RESERVED_SECTORS 1u
#define FAT_COUNT 2u
#define ROOT_ENTRIES 512u
#define DIRENT_SIZE 32u
#define SFN_LEN 11u
#define LFN_CHARS_PER_ENTRY 13u
#define LFN_LAST_ENTRY 0x40u
#define MAX_NAME_LEN 255u
#define MAX_PATH_DEPTH 3

#define ATTR_DIRECTORY 0x10u
#define ATTR_ARCHIVE 0x20u
#define ATTR_LFN 0x0fu
#define DIRENT_FREE 0x00u
#define DIRENT_DELETED 0xe5u

#define FAT16_MIN_CLUSTERS 0xff5u
#define FAT16_MAX_CLUSTERS 0xfff4u
#define FAT16_EOC 0xffffu
#define MEDIA_FIXED_DISK 0xf8u
#define FIXED_DATE 0x6a48u

#define MIN_IMAGE_MB 1u
#define MAX_IMAGE_MB 512u
#define MAX_FILE_BYTES (16u * 1024u * 1024u)

static uint8_t *image;
static uint64_t image_size;
static uint32_t bytes_per_cluster;
static uint32_t fat_offset;
static uint32_t root_offset;
static uint32_t data_offset;
static uint32_t total_clusters;
static uint32_t fat_size_sectors;
static uint32_t free_cluster = 2;

static void fail(const char *msg, const char *detail)
{
	fprintf(stderr, "mkfat: %s%s%s\n", msg, detail ? " " : "", detail ? detail : "");
	exit(1);
}

static void wr16(uint8_t *p, uint32_t v)
{
	p[0] = (uint8_t)v;
	p[1] = (uint8_t)(v >> 8);
}

static void wr32(uint8_t *p, uint32_t v)
{
	wr16(p, v & 0xffff);
	wr16(p + 2, v >> 16);
}

static uint64_t cluster_off(uint32_t cluster)
{
	return data_offset + (uint64_t)(cluster - 2) * bytes_per_cluster;
}

static uint32_t alloc_chain(uint64_t bytes)
{
	uint64_t count = (bytes + bytes_per_cluster - 1) / bytes_per_cluster;
	if (count == 0)
		count = 1;
	if (free_cluster + count > (uint64_t)total_clusters + 2)
		fail("image too small", NULL);
	uint32_t first = free_cluster;
	uint32_t last = first + (uint32_t)count - 1;
	for (uint32_t c = first; c < last; c++)
		wr16(&image[fat_offset + 2u * c], c + 1);
	wr16(&image[fat_offset + 2u * last], FAT16_EOC);
	free_cluster = last + 1;
	return first;
}

static uint8_t sfn_checksum(const uint8_t *sfn)
{
	uint8_t sum = 0;
	for (unsigned i = 0; i < SFN_LEN; i++)
		sum = (uint8_t)(((sum & 1) << 7) + (sum >> 1) + sfn[i]);
	return sum;
}

static uint8_t upper(char c)
{
	return (uint8_t)(c >= 'a' && c <= 'z' ? c - ('a' - 'A') : c);
}

static int make_sfn(const char *name, uint8_t sfn[SFN_LEN])
{
	const char *dot = strrchr(name, '.');
	size_t base_len = dot ? (size_t)(dot - name) : strlen(name);
	size_t ext_len = dot ? strlen(dot + 1) : 0;
	int needs_lfn = base_len > 8 || ext_len > 3 || (dot && strchr(name, '.') != dot);

	memset(sfn, ' ', SFN_LEN);
	size_t keep = needs_lfn ? 6 : 8;
	for (size_t i = 0, n = 0; i < base_len && n < keep; i++)
		if (name[i] != '.')
			sfn[n++] = upper(name[i]);
	if (needs_lfn) {
		sfn[6] = '~';
		sfn[7] = '1';
	}
	for (size_t i = 0; i < ext_len && i < 3; i++)
		sfn[8 + i] = upper(dot[1 + i]);
	return needs_lfn;
}

static void write_lfn(uint8_t *ent, const char *name, size_t len, size_t seq, int last, uint8_t sum)
{
	static const uint8_t slot_off[LFN_CHARS_PER_ENTRY] = { 1, 3, 5, 7, 9, 14, 16, 18, 20, 22, 24, 28, 30 };
	memset(ent, 0xff, DIRENT_SIZE);
	ent[0] = (uint8_t)(seq | (last ? LFN_LAST_ENTRY : 0));
	ent[11] = ATTR_LFN;
	ent[12] = 0;
	ent[13] = sum;
	wr16(&ent[26], 0);
	for (size_t s = 0; s < LFN_CHARS_PER_ENTRY; s++) {
		size_t ci = (seq - 1) * LFN_CHARS_PER_ENTRY + s;
		uint32_t ch = 0xffff;
		if (ci < len)
			ch = (uint8_t)name[ci];
		else if (ci == len)
			ch = 0;
		wr16(&ent[slot_off[s]], ch);
	}
}

static void add_entry(uint32_t dir_cluster, const char *name, uint8_t attr, uint32_t first_cluster, uint32_t size)
{
	uint8_t sfn[SFN_LEN];
	int needs_lfn = make_sfn(name, sfn);
	size_t len = strlen(name);
	size_t lfn_count = needs_lfn ? (len + LFN_CHARS_PER_ENTRY - 1) / LFN_CHARS_PER_ENTRY : 0;

	uint64_t off = dir_cluster == 0 ? root_offset : cluster_off(dir_cluster);
	uint64_t end = dir_cluster == 0 ? root_offset + ROOT_ENTRIES * DIRENT_SIZE : off + bytes_per_cluster;
	while (off + DIRENT_SIZE <= end && image[off] != DIRENT_FREE && image[off] != DIRENT_DELETED)
		off += DIRENT_SIZE;
	if (off + (lfn_count + 1) * DIRENT_SIZE > end)
		fail("directory full", NULL);

	uint8_t sum = sfn_checksum(sfn);
	for (size_t i = 0; i < lfn_count; i++)
		write_lfn(&image[off + i * DIRENT_SIZE], name, len, lfn_count - i, i == 0, sum);
	off += lfn_count * DIRENT_SIZE;

	uint8_t *ent = &image[off];
	memcpy(ent, sfn, SFN_LEN);
	ent[11] = attr;
	wr16(&ent[22], FIXED_DATE);
	wr16(&ent[24], FIXED_DATE);
	wr16(&ent[26], first_cluster);
	wr32(&ent[28], size);
}

static uint32_t fat16_mkdir(const char *name, uint32_t parent)
{
	uint32_t first = alloc_chain(bytes_per_cluster);
	uint8_t *dot = &image[cluster_off(first)];
	memset(dot, ' ', SFN_LEN);
	dot[0] = '.';
	dot[11] = ATTR_DIRECTORY;
	wr16(&dot[26], first);
	uint8_t *dotdot = dot + DIRENT_SIZE;
	memset(dotdot, ' ', SFN_LEN);
	dotdot[0] = '.';
	dotdot[1] = '.';
	dotdot[11] = ATTR_DIRECTORY;
	wr16(&dotdot[26], parent);
	add_entry(parent, name, ATTR_DIRECTORY, first, 0);
	return first;
}

static void fat16_write_file(uint32_t dir_cluster, const char *name, const uint8_t *data, uint32_t size)
{
	uint32_t first = alloc_chain(size);
	memcpy(&image[cluster_off(first)], data, size);
	add_entry(dir_cluster, name, ATTR_ARCHIVE, first, size);
}

static void fat16_format(void)
{
	uint32_t total_sectors = (uint32_t)(image_size / SECTOR);
	uint32_t root_sectors = (ROOT_ENTRIES * DIRENT_SIZE + SECTOR - 1) / SECTOR;
	uint32_t data_sectors = 0;
	fat_size_sectors = 1;
	for (;;) {
		data_sectors = total_sectors - RESERVED_SECTORS - FAT_COUNT * fat_size_sectors - root_sectors;
		uint32_t clusters = data_sectors / SECTORS_PER_CLUSTER;
		uint32_t needed = ((clusters + 2) * 2 + SECTOR - 1) / SECTOR;
		if (needed <= fat_size_sectors)
			break;
		fat_size_sectors = needed;
	}
	total_clusters = data_sectors / SECTORS_PER_CLUSTER;
	if (total_clusters < FAT16_MIN_CLUSTERS || total_clusters > FAT16_MAX_CLUSTERS) {
		fprintf(stderr, "mkfat: bad cluster count for FAT16 (%u)\n", total_clusters);
		exit(1);
	}
	fat_offset = RESERVED_SECTORS * SECTOR;
	root_offset = fat_offset + FAT_COUNT * fat_size_sectors * SECTOR;
	data_offset = root_offset + root_sectors * SECTOR;

	wr16(&image[fat_offset], 0xff00u | MEDIA_FIXED_DISK);
	wr16(&image[fat_offset + 2], FAT16_EOC);

	uint8_t *boot = image;
	boot[0] = 0xeb;
	boot[1] = 0x3c;
	boot[2] = 0x90;
	memcpy(&boot[3], "RAVYNOS ", 8);
	wr16(&boot[11], SECTOR);
	boot[13] = (uint8_t)SECTORS_PER_CLUSTER;
	wr16(&boot[14], RESERVED_SECTORS);
	boot[16] = (uint8_t)FAT_COUNT;
	wr16(&boot[17], ROOT_ENTRIES);
	wr16(&boot[19], total_sectors > 0xffff ? 0 : total_sectors);
	boot[21] = MEDIA_FIXED_DISK;
	wr16(&boot[22], fat_size_sectors);
	wr16(&boot[24], 63);
	wr16(&boot[26], 16);
	wr32(&boot[28], 0);
	wr32(&boot[32], total_sectors);
	boot[36] = 0x80;
	boot[38] = 0x29;
	wr32(&boot[39], 0x52564e50);
	memcpy(&boot[43], "RVYNOSESP  ", SFN_LEN);
	memcpy(&boot[54], "FAT16   ", 8);
	boot[510] = 0x55;
	boot[511] = 0xaa;
}

static void mirror_fat(void)
{
	memcpy(&image[fat_offset + fat_size_sectors * SECTOR], &image[fat_offset], fat_size_sectors * SECTOR);
}

static int split_path(const char *path, char comps[MAX_PATH_DEPTH][MAX_NAME_LEN + 1])
{
	int depth = 0;
	const char *p = path;
	for (;;) {
		const char *slash = strchr(p, '/');
		size_t len = slash ? (size_t)(slash - p) : strlen(p);
		if (len == 0 || len > MAX_NAME_LEN || depth == MAX_PATH_DEPTH)
			return 0;
		if (len <= 2 && p[0] == '.' && (len == 1 || p[1] == '.'))
			return 0;
		memcpy(comps[depth], p, len);
		comps[depth][len] = '\0';
		depth++;
		if (slash == NULL)
			return depth;
		p = slash + 1;
	}
}

static uint32_t read_input(const char *path, uint8_t *buf)
{
	FILE *f = fopen(path, "rb");
	if (f == NULL)
		fail("cannot open", path);
	size_t n = fread(buf, 1, MAX_FILE_BYTES, f);
	if (ferror(f) || n == 0 || (n == MAX_FILE_BYTES && fgetc(f) != EOF))
		fail("too large, empty or unreadable:", path);
	fclose(f);
	return (uint32_t)n;
}

int main(int argc, char **argv)
{
	if (argc < 5 || (argc % 2) != 1) {
		fprintf(stderr, "usage: mkfat <image> <size_mb> <name> <file> ...\n");
		return 2;
	}
	char *end = NULL;
	unsigned long mb = strtoul(argv[2], &end, 10);
	if (*argv[2] == '\0' || *end != '\0' || mb < MIN_IMAGE_MB || mb > MAX_IMAGE_MB) {
		fprintf(stderr, "mkfat: size out of range\n");
		return 2;
	}
	image_size = (uint64_t)mb * 1024 * 1024;
	image = calloc(1, image_size);
	uint8_t *buf = malloc(MAX_FILE_BYTES);
	if (image == NULL || buf == NULL) {
		fprintf(stderr, "mkfat: out of memory\n");
		return 2;
	}
	bytes_per_cluster = SECTOR * SECTORS_PER_CLUSTER;
	fat16_format();

	uint32_t boot_dir = 0;
	for (int i = 3; i + 1 < argc; i += 2) {
		const char *path = argv[i];
		char comps[MAX_PATH_DEPTH][MAX_NAME_LEN + 1];
		int depth = split_path(path, comps);
		uint32_t dir_cluster = 0;
		if (depth == 3 && strcmp(comps[0], "EFI") == 0 && strcmp(comps[1], "BOOT") == 0) {
			if (boot_dir == 0)
				boot_dir = fat16_mkdir("BOOT", fat16_mkdir("EFI", 0));
			dir_cluster = boot_dir;
		}
		else if (depth != 1) {
			fail("unsupported path", path);
		}
		uint32_t size = read_input(argv[i + 1], buf);
		fat16_write_file(dir_cluster, comps[depth - 1], buf, size);
	}
	mirror_fat();

	FILE *out = fopen(argv[1], "wb");
	if (out == NULL)
		fail("cannot create", argv[1]);
	if (fwrite(image, 1, image_size, out) != image_size || fclose(out) != 0)
		fail("short write to", argv[1]);
	free(buf);
	free(image);
	return 0;
}
