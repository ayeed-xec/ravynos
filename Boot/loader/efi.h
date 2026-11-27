// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
#ifndef RV_LOADER_EFI_H
#define RV_LOADER_EFI_H

#include <stdint.h>

typedef uint64_t efi_uintn;
typedef void *efi_handle;
typedef uint16_t efi_char16;

#define EFI_SUCCESS ((int64_t)0)
#define EFI_LOAD_ERROR ((int64_t)0x8000000000000001)
#define EFI_INVALID_PARAMETER ((int64_t)0x8000000000000002)
#define EFI_UNSUPPORTED ((int64_t)0x8000000000000003)
#define EFI_BAD_BUFFER_SIZE ((int64_t)0x8000000000000004)
#define EFI_BUFFER_TOO_SMALL ((int64_t)0x8000000000000005)
#define EFI_NOT_FOUND ((int64_t)0x800000000000000e)
#define EFI_NO_MEDIA ((int64_t)0x8000000000000012)

typedef int64_t efi_status;

struct efi_table_header {
	uint64_t signature;
	uint32_t revision;
	uint32_t header_size;
	uint32_t crc32;
	uint32_t reserved;
};

struct efi_simple_text_output {
	void *reset;
	efi_status (*output_string)(struct efi_simple_text_output *, const efi_char16 *);
	void *test_string;
};

struct efi_memory_descriptor {
	uint32_t type;
	uint32_t pad;
	uint64_t physical_start;
	uint64_t virtual_start;
	uint64_t number_of_pages;
	uint64_t attribute;
};

enum efi_memory_type {
	EfiReservedMemoryType = 0,
	EfiLoaderCode = 1,
	EfiLoaderData = 2,
	EfiBootServicesCode = 3,
	EfiBootServicesData = 4,
	EfiRuntimeServicesCode = 5,
	EfiRuntimeServicesData = 6,
	EfiConventionalMemory = 7,
	EfiUnusableMemory = 8,
	EfiACPIReclaimMemory = 9,
	EfiACPIMemoryNVS = 10,
	EfiMemoryMappedIO = 11,
	EfiMemoryMappedIOPortSpace = 12,
	EfiPalCode = 13,
	EfiMaxMemoryType = 14
};

#define EFI_PAGE_SHIFT 12
#define EFI_PAGE_SIZE (1UL << EFI_PAGE_SHIFT)
#define EFI_SIZE_TO_PAGES(a) (((a) + EFI_PAGE_SIZE - 1) >> EFI_PAGE_SHIFT)
#define EFI_ALLOCATE_ANY_PAGES 0
#define EFI_ALLOCATE_MAX_ADDRESS 1
#define EFI_ALLOCATE_ADDRESS 2
#define EFI_FILE_MODE_READ 0x1ull

struct efi_boot_services {
	struct efi_table_header hdr;
	void *raise_tpl;
	void *restore_tpl;
	efi_status (*allocate_pages)(int type, int memory_type, efi_uintn pages, uint64_t *memory);
	efi_status (*free_pages)(uint64_t memory, efi_uintn pages);
	efi_status (*get_memory_map)(efi_uintn *memory_map_size, struct efi_memory_descriptor *memory_map,
		efi_uintn *map_key, efi_uintn *descriptor_size, uint32_t *descriptor_version);
	efi_status (*allocate_pool)(int pool_type, efi_uintn size, void **buffer);
	efi_status (*free_pool)(void *buffer);
	void *create_event;
	void *set_timer;
	void *wait_for_event;
	void *signal_event;
	void *close_event;
	void *check_event;
	void *install_protocol_interface;
	void *reinstall_protocol_interface;
	void *uninstall_protocol_interface;
	efi_status (*handle_protocol)(efi_handle handle, const uint8_t *protocol, void **interface);
	void *reserved;
	void *register_protocol_notify;
	void *locate_handle;
	void *locate_device_path;
	void *install_configuration_table;
	void *load_image;
	void *start_image;
	void *exit;
	void *unload_image;
	efi_status (*exit_boot_services)(efi_handle image_handle, efi_uintn map_key);
	void *get_next_monotonic_count;
	efi_status (*stall)(efi_uintn microseconds);
	efi_status (*set_watchdog_timer)(efi_uintn timeout, uint64_t watchdog_code, efi_uintn data_size, efi_char16 *data);
	void *connect_controller;
	void *disconnect_controller;
	void *open_protocol;
	void *close_protocol;
	void *open_protocol_information;
	void *protocols_per_handle;
	void *locate_handle_buffer;
	efi_status (*locate_protocol)(const uint8_t *protocol, void *registration, void **interface);
	void *install_multiple_protocol_interfaces;
	void *uninstall_multiple_protocol_interfaces;
	void *calculate_crc32;
	void (*copy_mem)(void *destination, const void *source, efi_uintn length);
	void (*set_mem)(void *buffer, efi_uintn size, uint8_t value);
	void *create_event_ex;
};

struct efi_system_table {
	struct efi_table_header hdr;
	efi_char16 *firmware_vendor;
	uint32_t firmware_revision;
	uint32_t pad1;
	efi_handle console_in_handle;
	void *con_in;
	efi_handle console_out_handle;
	struct efi_simple_text_output *con_out;
	efi_handle standard_error_handle;
	void *std_err;
	void *runtime_services;
	struct efi_boot_services *boot_services;
	efi_uintn number_of_table_entries;
	void *configuration_table;
};

struct efi_loaded_image_protocol {
	uint32_t revision;
	efi_handle parent_handle;
	struct efi_system_table *system_table;
	efi_handle device_handle;
	void *file_path;
	void *reserved;
	uint32_t load_options_size;
	void *load_options;
	void *image_base;
	uint64_t image_size;
	void *image_code_type;
	void *image_data_type;
	void *unload;
};

struct efi_simple_file_system_protocol {
	uint64_t revision;
	efi_status (*open_volume)(struct efi_simple_file_system_protocol *, void **root);
};

struct efi_file_protocol {
	uint64_t revision;
	efi_status (*open)(struct efi_file_protocol *, void **new_handle, const efi_char16 *name, uint64_t mode, uint64_t attributes);
	efi_status (*close)(struct efi_file_protocol *);
	efi_status (*delete)(struct efi_file_protocol *);
	efi_status (*read)(struct efi_file_protocol *, efi_uintn *buffer_size, void *buffer);
	efi_status (*write)(struct efi_file_protocol *, efi_uintn *buffer_size, void *buffer);
	efi_status (*get_position)(struct efi_file_protocol *, uint64_t *position);
	efi_status (*set_position)(struct efi_file_protocol *, uint64_t position);
	void *get_info;
	void *set_info;
	void *flush;
};

static const uint8_t EFI_LOADED_IMAGE_PROTOCOL_GUID[16] = {
	0xa1, 0x31, 0x1b, 0x5b, 0x62, 0x95, 0xd2, 0x11,
	0x8e, 0x3f, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b
};

static const uint8_t EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID[16] = {
	0x22, 0x5b, 0x4e, 0x96, 0x59, 0x64, 0xd2, 0x11,
	0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b
};

#endif
