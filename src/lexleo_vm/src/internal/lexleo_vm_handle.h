/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_vm_handle.h
 * @ingroup lexleo_vm_internal_group
 * @brief Private LexLeo VM handle definition.
 *
 * @details
 * This header defines the internal structure of the opaque `lexleo_vm_t`
 * handle.
 *
 * The handle stores borrowed runtime dependencies provided at creation time
 * and owned runtime resources created during default initialization.
 *
 * This header is private to the `lexleo_vm` implementation and must not be
 * included by public API clients.
 */

#ifndef LEXLEO_LEXLEO_VM_HANDLE_H
#define LEXLEO_LEXLEO_VM_HANDLE_H

#include "osal/mem/osal_mem_types.h"
#include "osal/stdio/osal_stdio_types.h"
#include "osal/file/osal_file_types.h"
#include "osal/str/osal_str_types.h"
#include "osal/time/osal_time_types.h"

#include "stream/common/stream_opaque_type.h"

#include "logger/common/logger_opaque_type.h"

#define LEXLEO_VM_ROM_SIZE 1024
#define LEXLEO_VM_RAM_SIZE 1048576
#define LEXLEO_VM_DISK_SIZE 1048576

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lexleo_vm_owned_resources_t lexleo_vm_owned_resources_t;

/**
 * @brief Private LexLeo VM handle structure.
 *
 * @details
 * This structure stores the borrowed dependencies injected at creation time
 * and the owned runtime resources initialized later by
 * `lexleo_vm_complete_default_init()`.
 */
struct lexleo_vm_t {

	/** Borrowed memory operations. */
	const osal_mem_ops_t *mem_ops;

	/** Borrowed standard I/O operations. */
	const osal_stdio_ops_t *stdio_ops;

	/** Borrowed file operations. */
	const osal_file_ops_t *file_ops;

	/** Borrowed string operations. */
	const osal_str_ops_t *str_ops;

	/** Borrowed time operations. */
	const osal_time_ops_t *time_ops;

	/** Borrowed input stream. */
	stream_t *in;

	/** Borrowed output stream. */
	stream_t *out;

	/** Borrowed logger. */
	logger_t *logger;

	/* Owned runtime resources. */
	lexleo_vm_owned_resources_t *lexleo_vm_owned_resources;

	/* Physical slots */
	uint8_t rom[LEXLEO_VM_ROM_SIZE];
	uint8_t keyboard_control_register;
	uint8_t keyboard_data_register;
	uint8_t screen_control_register;
	uint8_t screen_data_register;
	uint8_t ram[LEXLEO_VM_RAM_SIZE];
	uint8_t disk[LEXLEO_VM_DISK_SIZE];
};

#define LEXLEO_VM_RESET_ADDRESS 0xFFFFFFE0
#define LEXLEO_VM_ISR_ADDRESS 0xFFFFFFE0

// write a driver for vm_in and vm_out
// decoding rules must be established since the beginning

/*
Material specifications:
LEXLEO_VM_RAM_BASE_ADDRESS
LEXLEO_VM_RAM_SIZE
LEXLEO_VM_ROM_BASE_ADDRESS
LEXLEO_VM_ROM_SIZE
LEXLEO_VM_DISK_BASE_ADDRESS
LEXLEO_VM_DISK_SIZE
LEXLEO_VM_RESET_ADDRESS (among ROM addresses)
LEXLEO_VM_ISR_ADDRESS (among ROM addresses)
initial states of the CPU registers
<here>bbbhow to save and restore when interruption?
ISR atomicity?
bbbeffets des lectures et écritures des registres d’entrée-sortie
bbbtraitement des accès invalides, notamment une écriture en ROM.
*/

/* draft:
rom will contain:
firmware.lexleo_bare_metal_exe
disk will contain:
b) bootloader.lexleo_bare_metal_efi
c) kernel.lexleo_bare_metal_efi
and in src in static:
static lexleo_vm_status_t physical_read_u8(
	lexleo_vm_t *vm,
	lexleo_vm_address_t address,
	uint8_t *value);
static lexleo_vm_status_t physical_write_u8(
	lexleo_vm_t *vm,
	lexleo_vm_address_t address,
	uint8_t value);
static void lexleo_vm_update_device(lexleo_vm_t *vm);
static lexleo_vm_cpu_run
in kernel space write meta data of the user process such as:
a) mapping process_id -> (mapping virtual_address -> physical_address)
b) ...
at boot, pc contains physical address of the first octet of firmware.lexleo_bare_metal_exe
a default material cfg
a reset mechanism look at 0xFFFFFFF0 for the first instruction of the firmware ; hence it must be a jump
ISR
registers of CPU has a register where to write the ISR address (in AMD64 it is IDTR (Interrupt Descriptor Table Register))
ISR can write in system input buffer and this buffer corresponds to physical addresses
*/

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LEXLEO_VM_HANDLE_H */
