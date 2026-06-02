/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_vm_borrower.c
 * @ingroup lexleo_vm_internal_group
 * @brief Runtime implementation for the LexLeo VM module.
 *
 * @details
 * This file implements the borrower-facing runtime functions used to run
 * previously created and initialized LexLeo VM handles.
 */

#include "internal/lexleo_vm_handle.h"

#include "lexleo_vm/borrowers/lexleo_vm.h"

#include "policy/lexleo_assert.h"

lexleo_vm_status_t lexleo_vm_run(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm);

	return LEXLEO_VM_STATUS_OK;
}
