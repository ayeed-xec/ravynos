// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
#ifndef RV_BOOT_ARGS_H
#define RV_BOOT_ARGS_H

#include <stdint.h>

#define RV_BOOT_ARGS_VERSION 2
#define RV_BOOT_ARGS_REVISION 0
#define RV_BOOT_ARGS_EFIMODE64 64
#define RV_BOOT_LINE_LENGTH 1024

struct rv_boot_video_v1 {
	uint32_t v_baseAddr;
	uint32_t v_display;
	uint32_t v_rowBytes;
	uint32_t v_width;
	uint32_t v_height;
	uint32_t v_depth;
};

struct rv_boot_video {
	uint32_t v_display;
	uint32_t v_rowBytes;
	uint32_t v_width;
	uint32_t v_height;
	uint32_t v_depth;
	uint8_t v_rotate;
	uint8_t v_resv_byte[3];
	uint32_t v_resv[6];
	uint64_t v_baseAddr;
};

struct rv_boot_args {
	uint16_t Revision;
	uint16_t Version;
	uint8_t efiMode;
	uint8_t debugMode;
	uint16_t flags;
	char CommandLine[RV_BOOT_LINE_LENGTH];
	uint32_t MemoryMap;
	uint32_t MemoryMapSize;
	uint32_t MemoryMapDescriptorSize;
	uint32_t MemoryMapDescriptorVersion;
	struct rv_boot_video_v1 VideoV1;
	uint32_t deviceTreeP;
	uint32_t deviceTreeLength;
	uint32_t kaddr;
	uint32_t ksize;
	uint32_t efiRuntimeServicesPageStart;
	uint32_t efiRuntimeServicesPageCount;
	uint64_t efiRuntimeServicesVirtualPageStart;
	uint32_t efiSystemTable;
	uint32_t kslide;
	uint32_t performanceDataStart;
	uint32_t performanceDataSize;
	uint32_t keyStoreDataStart;
	uint32_t keyStoreDataSize;
	uint64_t bootMemStart;
	uint64_t bootMemSize;
	uint64_t PhysicalMemorySize;
	uint64_t FSBFrequency;
	uint64_t pciConfigSpaceBaseAddress;
	uint32_t pciConfigSpaceStartBusNumber;
	uint32_t pciConfigSpaceEndBusNumber;
	uint32_t csrActiveConfig;
	uint32_t csrCapabilities;
	uint32_t boot_SMC_plimit;
	uint16_t bootProgressMeterStart;
	uint16_t bootProgressMeterEnd;
	struct rv_boot_video Video;
	uint32_t apfsDataStart;
	uint32_t apfsDataSize;
	uint64_t KC_hdrs_vaddr;
	uint64_t arvRootHashStart;
	uint64_t arvRootHashSize;
	uint64_t arvManifestStart;
	uint64_t arvManifestSize;
	uint64_t bsARVRootHashStart;
	uint64_t bsARVRootHashSize;
	uint64_t bsARVManifestStart;
	uint64_t bsARVManifestSize;
	uint32_t __reserved4[692];
};

_Static_assert(sizeof(struct rv_boot_args) == 4096, "boot_args must be 4096 bytes");

#endif
