// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 The ravynOS Authors
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define SECTOR 512

static uint8_t *image;
static uint64_t image_size;
static uint32_t bytes_per_cluster;
static uint32_t fat_offset;
static uint32_t root_offset;
static uint32_t data_offset;
static uint32_t total_clusters;
static uint32_t fat_size_sectors;
static uint32_t free_cluster = 2;

static void wr16(uint8_t *p, uint16_t v)
{
	p[0] = (uint8_t)v;
	p[1] = (uint8_t)(v >> 8);
}

static void wr32(uint8_t *p, uint32_t v)
{
	p[0] = (uint8_t)v;
	p[1] = (uint8_t)(v >> 8);
	p[2] = (uint8_t)(v >> 16);
	p[3] = (uint8_t)(v >> 24);
}

static uint32_t cluster_off(uint32_t cluster)
{
	return data_offset + (uint64_t)(cluster - 2) * bytes_per_cluster;
}

static uint32_t alloc_chain(uint64_t bytes)
{
	uint32_t count = (uint32_t)((bytes + bytes_per_cluster - 1) / bytes_per_cluster);
	if (count == 0)
		count = 1;
	if (free_cluster + count > total_clusters + 2) {
		fprintf(stderr, "mkfat: image too small\n");
		exit(1);
	}
	uint32_t first = free_cluster;
	for (uint32_t i = 0; i < count - 1; i++)
		wr16(&image[fat_offset + 2u * (free_cluster + i)], (uint16_t)(free_cluster + i + 1));
	wr16(&image[fat_offset + 2u * (free_cluster + count - 1)], 0xffff);
	free_cluster += count;
	return first;
}

static uint64_t dir_scan_start(uint32_t dir_cluster)
{
	if (dir_cluster == 0)
		return root_offset;
	return cluster_off(dir_cluster);
}

static uint64_t dir_scan_end(uint32_t dir_cluster)
{
	if (dir_cluster == 0)
		return root_offset + 512 * 32;
	return cluster_off(dir_cluster) + bytes_per_cluster;
}

static void add_entry(uint32_t dir_cluster, const char *name, uint8_t attr, uint32_t first_cluster, uint32_t size)
{
	for (uint64_t off = dir_scan_start(dir_cluster); off < dir_scan_end(dir_cluster); off += 32) {
		if (image[off] == 0x00 || image[off] == 0xe5) {
			memset(&image[off], ' ', 11);
			size_t nlen = 0;
			const char *dot = name;
			while (*dot && *dot != '.') {
				if (nlen < 8)
					image[off + nlen] = (uint8_t)(*dot >= 'a' && *dot <= 'z' ? *dot - 32 : *dot);
				nlen++;
				dot++;
			}
			size_t elen = 0;
			while (*dot) {
				if (*dot == '.') {
					elen = 0;
				} else if (elen < 3) {
					char c = *dot;
					image[off + 8 + elen] = (uint8_t)(c >= 'a' && c <= 'z' ? c - 32 : c);
					elen++;
				}
				dot++;
			}
			image[off + 11] = attr;
			wr16(&image[off + 22], 0x6a48);
			wr16(&image[off + 24], 0x6a48);
			wr16(&image[off + 26], (uint16_t)first_cluster);
			wr32(&image[off + 28], size);
			return;
		}
	}
	fprintf(stderr, "mkfat: directory full\n");
	exit(1);
}

static uint32_t fat16_mkdir(const char *name, uint32_t parent)
{
	uint32_t first = alloc_chain(bytes_per_cluster);
	uint8_t *dir = &image[cluster_off(first)];
	memset(dir, ' ', 11);
	dir[0] = '.';
	dir[11] = 0x10;
	wr16(&dir[26], first);
	uint8_t *dotdot = dir + 32;
	memset(dotdot, ' ', 11);
	dotdot[0] = '.';
	dotdot[1] = '.';
	dotdot[11] = 0x10;
	add_entry(parent, name, 0x10, first, 0);
	return first;
}

static void fat16_write_file(uint32_t dir_cluster, const char *name, const uint8_t *data, uint64_t size)
{
	uint32_t first = alloc_chain(size);
	uint64_t off = cluster_off(first);
	memcpy(&image[off], data, size);
	add_entry(dir_cluster, name, 0x20, first, (uint32_t)size);
}

static void fat16_format(void)
{
	uint32_t total_sectors = (uint32_t)(image_size / SECTOR);
	uint32_t root_sectors = (512 * 32 + SECTOR - 1) / SECTOR;
	fat_size_sectors = 1;
	for (;;) {
		uint32_t data_sectors = total_sectors - 1 - 2 * fat_size_sectors - root_sectors;
		uint32_t clusters = data_sectors / (bytes_per_cluster / SECTOR);
		uint32_t needed = ((clusters + 2) * 2 + SECTOR - 1) / SECTOR;
		if (needed == fat_size_sectors)
			break;
		if (needed > fat_size_sectors)
			fat_size_sectors = needed;
		else
			break;
	}
	uint32_t data_sectors = total_sectors - 1 - 2 * fat_size_sectors - root_sectors;
	total_clusters = data_sectors / (bytes_per_cluster / SECTOR);
	if (total_clusters < 0xff5 || total_clusters > 0xfff4) {
		fprintf(stderr, "mkfat: bad cluster count for FAT16 (%u)\n", total_clusters);
		exit(1);
	}
	fat_offset = SECTOR;
	root_offset = fat_offset + 2u * fat_size_sectors * SECTOR;
	data_offset = root_offset + root_sectors * SECTOR;

	wr16(&image[fat_offset + 2u * 1], 0xffff);
	wr16(&image[fat_offset + 2u * 2], 0xffff);
	uint32_t second_fat = fat_offset + fat_size_sectors * SECTOR;
	memcpy(&image[second_fat], &image[fat_offset], fat_size_sectors * SECTOR);

	uint8_t boot[SECTOR];
	memset(boot, 0, SECTOR);
	boot[0] = 0xeb;
	boot[1] = 0x3c;
	boot[2] = 0x90;
	memcpy(&boot[3], "RAVYNOS ", 8);
	wr16(&boot[11], SECTOR);
	boot[13] = (uint8_t)(bytes_per_cluster / SECTOR);
	wr16(&boot[14], 1);
	boot[16] = 2;
	wr16(&boot[17], (uint16_t)(root_sectors * SECTOR / 32));
	if (total_sectors > 0xffff)
		wr16(&boot[19], 0);
	else
		wr16(&boot[19], (uint16_t)total_sectors);
	wr32(&boot[32], total_sectors);
	boot[21] = 0xf8;
	wr16(&boot[22], (uint16_t)fat_size_sectors);
	wr16(&boot[24], 63);
	wr16(&boot[26], 16);
	wr32(&boot[28], 0);
	boot[36] = 0x80;
	boot[38] = 0x29;
	wr32(&boot[39], 0x52564e50);
	memcpy(&boot[43], "RVYNOSESP  ", 11);
	memcpy(&boot[54], "FAT16   ", 8);
	boot[510] = 0x55;
	boot[511] = 0xaa;
	memcpy(image, boot, SECTOR);
}

int main(int argc, char **argv)
{
	if (argc < 5 || (argc % 2) != 1) {
		fprintf(stderr, "usage: mkfat <image> <size_mb> <name> <file> ...\n");
		return 2;
	}
	image_size = (uint64_t)strtoul(argv[2], 0, 10) * 1024 * 1024;
	if (image_size < 1024u * 1024 || image_size > 512u * 1024 * 1024) {
		fprintf(stderr, "mkfat: size out of range\n");
		return 2;
	}
	image = malloc(image_size);
	if (!image) {
		fprintf(stderr, "mkfat: out of memory\n");
		return 2;
	}
	bytes_per_cluster = SECTOR * 8;
	memset(image, 0, image_size);
	fat16_format();
	uint32_t boot_dir = 0;
	int have_boot_dir = 0;
	for (int i = 3; i + 1 < argc; i += 2) {
		static uint8_t buf[16 * 1024 * 1024];
		const char *path = argv[i];
		uint32_t dir_cluster = 0;
		char comps[8][12];
		int depth = 0;
		const char *p = path;
		while (*p) {
			const char *slash = strchr(p, '/');
			size_t len = slash ? (size_t)(slash - p) : strlen(p);
			if (len >= 12 || depth == 8) {
				fprintf(stderr, "mkfat: bad path %s\n", path);
				return 1;
			}
			memcpy(comps[depth], p, len);
			comps[depth][len] = 0;
			depth++;
			if (!slash)
				break;
			p = slash + 1;
		}
		if (!depth) {
			fprintf(stderr, "mkfat: bad path %s\n", path);
			return 1;
		}
		if (depth == 3 && strcmp(comps[0], "EFI") == 0 && strcmp(comps[1], "BOOT") == 0) {
			if (!have_boot_dir) {
				uint32_t efi = fat16_mkdir("EFI", 0);
				boot_dir = fat16_mkdir("BOOT", efi);
				have_boot_dir = 1;
			}
			dir_cluster = boot_dir;
			comps[0][0] = 0;
		} else if (depth != 1) {
			fprintf(stderr, "mkfat: unsupported path %s\n", path);
			return 1;
		}
		const char *name = depth > 1 ? comps[depth - 1] : path;
		FILE *f = fopen(argv[i + 1], "rb");
		if (!f) {
			fprintf(stderr, "mkfat: cannot open %s\n", argv[i + 1]);
			return 1;
		}
		uint64_t n = fread(buf, 1, sizeof(buf), f);
		if (ferror(f) || n == 0 || (n == sizeof(buf) && fgetc(f) != EOF)) {
			fprintf(stderr, "mkfat: %s too large or unreadable\n", argv[i + 1]);
			return 1;
		}
		fclose(f);
		fat16_write_file(dir_cluster, name, buf, n);
	}
	FILE *out = fopen(argv[1], "wb");
	if (!out) {
		fprintf(stderr, "mkfat: cannot create %s\n", argv[1]);
		return 1;
	}
	if (fwrite(image, 1, image_size, out) != image_size) {
		fprintf(stderr, "mkfat: short write\n");
		return 1;
	}
	fclose(out);
	return 0;
}
