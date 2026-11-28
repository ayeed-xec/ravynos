// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
#ifndef RV_LOADER_KERNELIMG_H
#define RV_LOADER_KERNELIMG_H

#include <stdint.h>

#define RV_MAX_SEGMENTS 32

// mach-o/loader.h: the two header fields this loader checks.
#define RV_MACHO_FILETYPE_EXEC 2u

enum rv_kernel_kind { RV_KERNEL_ELF = 1, RV_KERNEL_MACHO = 2 };

struct rv_segment {
    uint64_t paddr;
    uint64_t file_offset;
    uint64_t file_size;
    uint64_t mem_size;
};

struct rv_kernel_image {
    int kind;
    uint64_t entry_paddr;
    uint64_t base_paddr;
    uint64_t end_paddr;
    uint32_t segment_count;
    struct rv_segment segments[RV_MAX_SEGMENTS];
};

int rv_parse_elf(const uint8_t *data, uint64_t size, struct rv_kernel_image *image);
int rv_parse_macho(const uint8_t *data, uint64_t size, struct rv_kernel_image *image);

#endif
