
#ifndef LEXLEO_FAKE_VM_H
#define LEXLEO_FAKE_VM_H

#include "lexleo_vm/borrowers/lexleo_vm_borrowers_types.h"
#include "lexleo_vm/cr/lexleo_vm_cr_types.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// FAKE API

typedef struct fake_vm_ctrl_t {

	/* spy */
	size_t destroy_call_count;
	lexleo_vm_t **last_destroy_vm;
	lexleo_vm_t *last_destroy_vm_value;

} fake_vm_ctrl_t;

extern fake_vm_ctrl_t g_fake_vm_ctrl;

typedef struct fake_vm_t {

	/* cfg */
	lexleo_vm_status_t next_run_ret;

	/* spy */
	size_t run_call_count;
	lexleo_vm_t *last_run_vm;

} fake_vm_t;

extern fake_vm_t g_fake_vm;

static inline fake_vm_t *fake_vm_real_to_fake(lexleo_vm_t *vm) { return (fake_vm_t *)vm; }
static inline lexleo_vm_t *fake_vm_fake_to_real(fake_vm_t *fake) { return (lexleo_vm_t *)fake; }

void fake_vm_reset(void);

void __wrap_lexleo_vm_destroy(lexleo_vm_t **vm);

lexleo_vm_status_t __wrap_lexleo_vm_run(lexleo_vm_t *vm);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_FAKE_VM_H */
