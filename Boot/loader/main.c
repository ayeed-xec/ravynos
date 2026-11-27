// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
#include "efi.h"
#include "kernelimg.h"
#include "util.h"
#include "boot_args.h"

struct efi_boot_services *gBS;
struct efi_system_table *gST;

static void rv_fail(const char *where, efi_status status)
{
	rv_serial_puts("RVLOADER ERR ");
	rv_serial_puts(where);
	rv_serial_puts(" ");
	rv_serial_hex64((uint64_t)status);
	rv_serial_puts("\n");
}

#define RV_MAX_PATH 128
#define RV_BELOW_4G 0xffffffffull
#define RV_MEMORY_MAP_SLACK 8
#define RV_EXIT_BOOT_ATTEMPTS 8

static int rv_to_utf16(efi_char16 *dst, efi_uintn cap, const char *src)
{
	efi_uintn i = 0;
	for (; src[i] != '\0'; i++) {
		if (i + 1 >= cap)
			return -1;
		dst[i] = (efi_char16)(uint8_t)src[i];
	}
	dst[i] = 0;
	return 0;
}

static int rv_open_file(struct efi_simple_file_system_protocol *sfs, const char *path, void **file_out)
{
	efi_char16 wpath[RV_MAX_PATH];
	if (rv_to_utf16(wpath, RV_MAX_PATH, path) != 0)
		return -1;
	void *root = 0;
	if (sfs->open_volume(sfs, &root) != EFI_SUCCESS)
		return -1;
	struct efi_file_protocol *rootf = root;
	if (rootf->open(root, file_out, wpath, EFI_FILE_MODE_READ, 0) != EFI_SUCCESS) {
		rootf->close(root);
		return -1;
	}
	rootf->close(root);
	return 0;
}

static int rv_read_all(void *file, uint8_t **data_out, uint64_t *size_out)
{
	struct efi_file_protocol *f = file;
	uint64_t end = 0;
	f->set_position(f, UINT64_MAX);
	if (f->get_position(f, &end) != EFI_SUCCESS)
		return -1;
	if (f->set_position(f, 0) != EFI_SUCCESS)
		return -1;
	if (end == 0)
		return -1;
	uint8_t *buf = 0;
	if (gBS->allocate_pool(EfiBootServicesData, end, (void **)&buf) != EFI_SUCCESS)
		return -1;
	efi_uintn remaining = (efi_uintn)end;
	uint8_t *cursor = buf;
	while (remaining) {
		efi_uintn chunk = remaining;
		if (f->read(f, &chunk, cursor) != EFI_SUCCESS) {
			gBS->free_pool(buf);
			return -1;
		}
		if (chunk == 0)
			break;
		cursor += chunk;
		remaining -= chunk;
	}
	if (remaining) {
		gBS->free_pool(buf);
		return -1;
	}
	*data_out = buf;
	*size_out = end;
	return 0;
}

static efi_status rv_alloc_copy(struct rv_kernel_image *image, const uint8_t *data)
{
	if (image->segment_count == 0)
		return EFI_LOAD_ERROR;
	uint64_t page_start = UINT64_MAX;
	uint64_t page_end = 0;
	for (uint32_t i = 0; i < image->segment_count; i++) {
		struct rv_segment *seg = &image->segments[i];
		if (seg->paddr + seg->mem_size < seg->paddr)
			return EFI_LOAD_ERROR;
		uint64_t start = seg->paddr & ~(uint64_t)(EFI_PAGE_SIZE - 1);
		uint64_t end = (seg->paddr + seg->mem_size + EFI_PAGE_SIZE - 1) & ~(uint64_t)(EFI_PAGE_SIZE - 1);
		if (start < page_start)
			page_start = start;
		if (end > page_end)
			page_end = end;
	}
	uint64_t pages = (page_end - page_start) / EFI_PAGE_SIZE;
	uint64_t addr = page_start;
	efi_status status = gBS->allocate_pages(EFI_ALLOCATE_ADDRESS, EfiLoaderCode, pages, &addr);
	if (status != EFI_SUCCESS)
		return status;
	rv_memset((void *)page_start, 0, page_end - page_start);
	for (uint32_t i = 0; i < image->segment_count; i++) {
		struct rv_segment *seg = &image->segments[i];
		rv_memcpy((void *)seg->paddr, data + seg->file_offset, seg->file_size);
	}
	return EFI_SUCCESS;
}

static efi_status rv_build_boot_args(struct rv_kernel_image *image, uint64_t *bootargs_out)
{
	efi_uintn map_key = 0;
	efi_uintn descriptor_size = 0;
	uint32_t descriptor_version = 0;
	efi_uintn full_size = 0;

	efi_status probe = gBS->get_memory_map(&full_size, 0, &map_key, &descriptor_size, &descriptor_version);
	if (probe != EFI_BUFFER_TOO_SMALL) {
		rv_fail("get-memory-map-probe", probe);
		return EFI_LOAD_ERROR;
	}
	if (descriptor_size == 0)
		return EFI_LOAD_ERROR;
	full_size += descriptor_size * RV_MEMORY_MAP_SLACK;

	uint64_t map_pages = EFI_SIZE_TO_PAGES(full_size);
	uint64_t map_addr = RV_BELOW_4G;
	efi_status status = gBS->allocate_pages(EFI_ALLOCATE_MAX_ADDRESS, EfiRuntimeServicesData, map_pages, &map_addr);
	if (status != EFI_SUCCESS)
		return status;
	rv_memset((void *)map_addr, 0, map_pages * EFI_PAGE_SIZE);

	uint64_t ba_addr = RV_BELOW_4G;
	status = gBS->allocate_pages(EFI_ALLOCATE_MAX_ADDRESS, EfiRuntimeServicesData, 1, &ba_addr);
	if (status != EFI_SUCCESS)
		return status;
	rv_memset((void *)ba_addr, 0, EFI_PAGE_SIZE);

	efi_uintn real_size = full_size;
	status = gBS->get_memory_map(&real_size, (struct efi_memory_descriptor *)map_addr, &map_key,
		&descriptor_size, &descriptor_version);
	if (status != EFI_SUCCESS)
		return status;

	struct rv_boot_args *ba = (struct rv_boot_args *)ba_addr;
	ba->Version = RV_BOOT_ARGS_VERSION;
	ba->Revision = RV_BOOT_ARGS_REVISION;
	ba->efiMode = RV_BOOT_ARGS_EFIMODE64;
	ba->debugMode = 0;
	ba->flags = 0;
	const char cmdline[] = "rvynos w1-harness debug=0x14e";
	for (uint32_t i = 0; i < sizeof(cmdline) && i < RV_BOOT_LINE_LENGTH; i++)
		ba->CommandLine[i] = cmdline[i];
	ba->MemoryMap = (uint32_t)map_addr;
	ba->MemoryMapSize = (uint32_t)real_size;
	ba->MemoryMapDescriptorSize = (uint32_t)descriptor_size;
	ba->MemoryMapDescriptorVersion = descriptor_version;
	ba->kaddr = (uint32_t)image->base_paddr;
	ba->ksize = (uint32_t)(image->end_paddr - image->base_paddr);
	ba->efiSystemTable = (uint32_t)(uint64_t)gST;
	ba->kslide = 0;

	*bootargs_out = ba_addr;
	return EFI_SUCCESS;
}

static int rv_parse_any(const uint8_t *data, uint64_t size, struct rv_kernel_image *image)
{
	if (rv_parse_elf(data, size, image) == 0)
		return 0;
	return rv_parse_macho(data, size, image);
}

efi_status efi_main(efi_handle image_handle, struct efi_system_table *system_table)
{
	gST = system_table;
	gBS = system_table->boot_services;

	rv_serial_init();
	rv_serial_puts("RVLOADER start\n");
	gBS->set_watchdog_timer(0, 0, 0, 0);

	struct efi_loaded_image_protocol *loaded = 0;
	if (gBS->handle_protocol(image_handle, EFI_LOADED_IMAGE_PROTOCOL_GUID, (void **)&loaded) != EFI_SUCCESS) {
		rv_fail("loaded-image", EFI_NOT_FOUND);
		return EFI_LOAD_ERROR;
	}

	struct efi_simple_file_system_protocol *sfs = 0;
	if (gBS->handle_protocol(loaded->device_handle, EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID, (void **)&sfs) != EFI_SUCCESS) {
		rv_fail("filesystem", EFI_NOT_FOUND);
		return EFI_LOAD_ERROR;
	}

	static const char paths[] = "\\ravynos\\kernel\0\\ravynos\\kernel.elf\0"
				    "\\kernel.elf\0\\kernel";
	void *file = 0;
	const char *opened = 0;
	for (const char *p = paths; *p != 0; p += rv_strlen(p) + 1) {
		if (rv_open_file(sfs, p, &file) == 0) {
			opened = p;
			break;
		}
	}
	if (!file) {
		rv_fail("kernel-not-found", EFI_NOT_FOUND);
		return EFI_LOAD_ERROR;
	}
	rv_serial_puts("RVLOADER kernel ");
	rv_serial_puts(opened);
	rv_serial_puts("\n");

	uint8_t *data = 0;
	uint64_t size = 0;
	if (rv_read_all(file, &data, &size) != 0) {
		rv_fail("read", EFI_LOAD_ERROR);
		return EFI_LOAD_ERROR;
	}
	file = 0;

	struct rv_kernel_image image;
	rv_memset(&image, 0, sizeof(image));
	if (rv_parse_any(data, size, &image) != 0) {
		rv_fail("parse", EFI_UNSUPPORTED);
		return EFI_LOAD_ERROR;
	}
	rv_serial_puts("RVLOADER parsed kind=");
	rv_serial_hex16((uint32_t)image.kind);
	rv_serial_puts(" entry=");
	rv_serial_hex64(image.entry_paddr);
	rv_serial_puts(" base=");
	rv_serial_hex64(image.base_paddr);
	rv_serial_puts("\n");

	efi_status status = rv_alloc_copy(&image, data);
	if (status != EFI_SUCCESS) {
		rv_fail("alloc-copy", status);
		return status;
	}

	uint64_t bootargs = 0;
	status = rv_build_boot_args(&image, &bootargs);
	if (status != EFI_SUCCESS) {
		rv_fail("boot-args", status);
		return status;
	}

	efi_uintn map_key = 0;
	efi_uintn descriptor_size = 0;
	uint32_t descriptor_version = 0;
	int exited = 0;
	efi_uintn map_size = 0;
	for (int attempt = 0; attempt < RV_EXIT_BOOT_ATTEMPTS; attempt++) {
		map_size = 0;
		if (gBS->get_memory_map(&map_size, 0, &map_key, &descriptor_size, &descriptor_version) != EFI_BUFFER_TOO_SMALL)
			break;
		map_size += descriptor_size * RV_MEMORY_MAP_SLACK;
		uint8_t *stack_map = 0;
		if (gBS->allocate_pool(EfiBootServicesData, map_size, (void **)&stack_map) != EFI_SUCCESS)
			break;
		efi_uintn key = 0;
		if (gBS->get_memory_map(&map_size, (struct efi_memory_descriptor *)stack_map, &key,
			&descriptor_size, &descriptor_version) != EFI_SUCCESS) {
			gBS->free_pool(stack_map);
			continue;
		}
		if (gBS->exit_boot_services(image_handle, key) == EFI_SUCCESS) {
			exited = 1;
			break;
		}
		gBS->free_pool(stack_map);
	}
	if (!exited) {
		rv_fail("exit-boot-services", EFI_INVALID_PARAMETER);
		return EFI_INVALID_PARAMETER;
	}

	rv_serial_puts("RVLOADER ebs-ok entry=");
	rv_serial_hex64(image.entry_paddr);
	rv_serial_puts(" bootargs=");
	rv_serial_hex64(bootargs);
	rv_serial_puts("\n");

	if (image.kind == RV_KERNEL_MACHO)
		rv_macho_handoff(image.entry_paddr, bootargs);
	rv_elf_handoff(image.entry_paddr, bootargs);

	for (;;)
#if defined(__x86_64__)
		__asm__ volatile("cli; hlt");
#elif defined(__riscv)
		__asm__ volatile("wfi");
#else
		__asm__ volatile("wfi");
#endif
	return EFI_LOAD_ERROR;
}
