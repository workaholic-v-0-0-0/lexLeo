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

	/* Physical slots */
	uint8_t rom[LEXLEO_VM_ROM_SIZE];
	uint8_t keyboard_irq_pending_register;
	uint8_t keyboard_data_ready;
	uint8_t keyboard_data_register;
	uint8_t screen_data_register;
	uint8_t ram[LEXLEO_VM_RAM_SIZE];
	uint8_t disk[LEXLEO_VM_DISK_SIZE];

	//lexleo_vm_cpu_t cpu;
};

#define LEXLEO_VM_RESET_ADDRESS 0xFFFFFFE0
#define LEXLEO_VM_ISR_ADDRESS 0xFFFFFFE0

// write a driver for vm_in and vm_out
// decoding rules must be established since the beginning

/*
static bool lexleo_vm_cpu_enter_isr(vm, cause) {
	vm.keyboard_irq_pending_register = false
	saves RSP, RIP, CR.translation_enabled, CR.privilege.
	RSP = CR.handler_stack_base_address + CR.handler_stack_capacity
	RIP = CR.isr_address
}
static bool lexleo_vm_cpu_restore_exec_env_after_isr(vm, cause) {
	restore RSP, RIP, CR.translation_enabled, CR.privilege.
}
*/

/*
static bool lexleo_vm_keyboard_read_data(vm);
static bool lexleo_vm_screen_write_data(vm);

lexleo_vm_status_t lexleo_vm_run(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm && <vm is well initialized accordingly with hardware specifications>);

	while (true) {

		if (!lexleo_vm_update_input_controller(vm)) {
			...
			return LEXLEO_VM_STATUS_...;
		}

		if (vm->keyboard_irq_pending_register && !vm->cpu.reg.cr.handler_active) {
			if (!lexleo_vm_cpu_enter_isr(vm, LEXLEO_VM_CAUSE_KEYBOARD_IRQ)) {
				...
				return LEXLEO_VM_STATUS_...;
			}
		}

		lexleo_vm_cpu_execute_next_instruction(vm);

		// Handle errors
	}

	return LEXLEO_VM_STATUS_OK;
}
*/

/*
Hardware specifications (with physical address map):
LEXLEO_VM_RAM_BASE_ADDRESS
LEXLEO_VM_RAM_SIZE
LEXLEO_VM_ROM_BASE_ADDRESS (write in ROM is forbidden, even with SUPERVISOR privileges)
LEXLEO_VM_ROM_SIZE
LEXLEO_VM_DISK_BASE_ADDRESS
LEXLEO_VM_DISK_SIZE
LEXLEO_VM_KEYBOARD_IRQ_PENDING_ADDRESS
LEXLEO_VM_KEYBOARD_DATA_READY_ADDRESS
LEXLEO_VM_KEYBOARD_DATA_ADDRESS
LEXLEO_VM_SCREEN_DATA_ADDRESS
LEXLEO_VM_SYSTEM_BUFFER (a circular buffer in RAM: read_index, write_index and then data)
LEXLEO_VM_SYSTEM_BUFFER_CAPACITY
LEXLEO_VM_INITIAL_STACK_BASE_ADDRESS
LEXLEO_VM_INITIAL_STACK_CAPACITY
LEXLEO_VM_INITIAL_HANDLER_STACK_BASE_ADDRESS
LEXLEO_VM_INITIAL_HANDLER_STACK_CAPACITY
LEXLEO_VM_RESET_ADDRESS (among ROM addresses)
LEXLEO_VM_ISR_ADDRESS (among ROM addresses but it can be a jump to a "RAM adress")
LEXLEO_VM_EXCEPTION_ENTRY_ADDRESS (among ROM addresses but it can be a jump to a "RAM adress")
LEXLEO_VM_INITIAL_MAP_BASE (entry format: virtual_base, physical_base, size, permissions)
LEXLEO_VM_INITIAL_MAP_COUNT (number of entries)
LEXLEO_VM_INITIAL_TRANSLATION_ENABLED (when false, physical addresses are used directly, no translation table is consulted)
initial states of the CPU registers:
	CR.translation_enabled = LEXLEO_VM_INITIAL_TRANSLATION_ENABLED (this value is false)
	CR.privilege = SUPERVISOR (will correspond to kernel mode during kernel execution)
	CR.map_base = LEXLEO_VM_INITIAL_MAP_BASE
	CR.map_count = LEXLEO_VM_INITIAL_MAP_COUNT
	CR.isr_address = LEXLEO_VM_ISR_ADDRESS
	CR.handler_stack_base_address = LEXLEO_VM_INITIAL_HANDLER_STACK_BASE_ADDRESS
	CR.handler_stack_capacity = LEXLEO_VM_INITIAL_HANDLER_STACK_CAPACITY
	CR.exception_entry_address = LEXLEO_VM_EXCEPTION_ENTRY_ADDRESS
	CR.handler_active = false # in order to avoid nested ISR
	CR.cause = 0
	CR.fault_instruction_address = 0
	CR.fault_accessed_address = 0
	PC = LEXLEO_VM_RESET_ADDRESS
	RSP = LEXLEO_VM_INITIAL_STACK_BASE_ADDRESS + LEXLEO_VM_INITIAL_STACK_CAPACITY
Handler entry (interruptions and exceptions):
	capture the interrupted PC, RSP, privilege and translation state;
	select the physical handler stack;
	save the captured values in a precisely defined return frame;
	enter SUPERVISOR privilege;
	disable translation;
	set handler_active;
	set cause and, when relevant, fault_instruction_address and fault_accessed_address;
	transfer execution to the selected handler.
Before and after handler execution:
	before handler execution:
		cpu saves RSP in <an internal CPU register>
		cpu does RSP = CR.handler_stack_base_address + CR.handler_stack_capacity
		cpu saves in this order <old RSP in an internal CPU register>, RIP, CR.translation_enabled, CR.privilege by pushing them in this order at CR.handler_stack_base_address + CR.handler_stack_capacity.
		cpu does CR.privilege = SUPERVISOR
		cpu does CR.handler_active = true
		cpu does CR.translation_enabled = false
		cpu does RIP = <CR.isr_address or CR.exception_entry_address>
	handler MUST preserve values of all the registers by doing push x ; ... ; pop <a register> ; mov x, <a register> ; for all x register that it uses ; hence RBP is in particular retrievable after handler execution
	handler MUST finish with iretq
iretq:
	cpu restores RSP (and hence it is the interrupted program's), RIP, CR.translation_enabled, CR.privilege.
	handler_active = false
no nested handlers:
	if IRQ during a handler then keep pending until after IRET.
	if exception during a handler then fatal error: log and stop VM execution.
consequences of reading and writing of input-output devices registers:
	keyboard_irq_pending_register:
		Values: 0 or 1.
		Keyboard controller read: allowed; no side effect.
		Keyboard controller write: sets to 1 when a new input byte has been stored in keyboard_data_register and marked as ready.
		CPU read: returns the current value; no side effect.
		CPU write instruction: forbidden.
		Automatically set to 1 when the controller receives an input byte (implemented in lexleo_vm_update_input_controller()).
		Automatically cleared to 0 when the CPU accepts this IRQ (implemented in lexleo_vm_cpu_enter_isr()).
		Accepting the IRQ does not consume the input byte.
	keyboard_data_ready_register:
		Values: 0 or 1.
		Keyboard controller read:
			allowed; no side effect.
			When 1, the controller must not read another byte from vm_in.
		Keyboard controller write:
			sets to 1 after storing a new input byte from vm_in into keyboard_data_register.
		CPU read: returns the current value; no side effect.
		CPU write instruction: forbidden.
		Automatically set to 1 when an input byte is stored.
		Automatically cleared to 0 when that byte is read.
		Accepting the IRQ does not clear this register.
	keyboard_data_register:
		Values: 0 to 255.
		Initial value: 0; not valid input until data_ready == 1.
		Keyboard controller read: allowed; no side effect.
		Keyboard controller write:
			stores a new input byte only when data_ready == 0;
			then sets data_ready to 1 and irq_pending to 1.
			Implemented in lexleo_vm_update_input_controller().
		CPU read when data_ready == 1:
			returns the stored byte;
			clears data_ready to 0;
			clears irq_pending to 0.
			The stored bits need not be erased.
		CPU read when data_ready == 0:
			raises an I/O access exception;
			leaves device registers unchanged.
		CPU write instruction: forbidden.
		Accepting the IRQ does not modify this register or consume the stored byte.
	screen_data_register:
		Values: 0 to 255.
		Initial value: 0; no output is produced by initialization.
		Screen controller read: obtains the byte supplied by a CPU write.
		Screen controller write: not used in this model.
		CPU read: forbidden; raises an I/O access exception.
		CPU write instruction:
			stores the written byte;
			triggers transmission of that byte to vm_out.
			The operation completes when vm_out accepts the byte.
			Each write triggers transmission, even if the value is unchanged.
		Output stream failure: stops VM execution with an I/O error status.
interruption cause: (only one)
	LEXLEO_VM_CAUSE_KEYBOARD_IRQ
exception:
	Exception causes and if fault_accessed_address is applicable:
		EXCEPTION_CAUSE_INVALID_INSTRUCTION (fault_accessed_address is not applicable)
		EXCEPTION_CAUSE_DIVISION_BY_ZERO (fault_accessed_address is not applicable)
		EXCEPTION_CAUSE_UNMAPPED_ADDRESS (fault_accessed_address is applicable)
		EXCEPTION_CAUSE_ACCESS_VIOLATION (fault_accessed_address is applicable)
		EXCEPTION_CAUSE_PRIVILEGED_INSTRUCTION (fault_accessed_address is not applicable)
what to do when isr_enter fails:
	fatal error, logging, stop vm
Assembly code specifications:
	Calling convention / ABI:
		argument passing
		return values
		caller-saved registers
		callee-saved registers
		stack alignment
		stack cleanup
	Instruction set:
		...

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
