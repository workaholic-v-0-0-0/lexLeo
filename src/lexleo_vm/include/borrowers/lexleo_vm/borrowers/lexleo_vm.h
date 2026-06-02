/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_vm.h
 * @ingroup lexleo_vm_borrowers_api
 * @brief Runtime API for the LexLeo virtual machine.
 *
 * @details
 * This header declares the main entry point used to execute a previously
 * created LexLeo virtual machine.
 */

#ifndef LEXLEO_VM_H
#define LEXLEO_VM_H

#include "lexleo_vm/borrowers/lexleo_vm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Main runtime entry point of the LexLeo virtual machine.
 *
 * @details
 * Runs a previously created and initialized LexLeo VM instance.
 *
 * @param[in] vm Virtual machine to run.
 *
 * @return Execution status.
 *
 * See contract:
 * - @ref specifications_lexleo_vm_run
 */
lexleo_vm_status_t lexleo_vm_run(lexleo_vm_t *vm);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_VM_H */
