

#include "lexleo/test/fake_vm.h"

#include "lexleo_cmocka.h"

fake_vm_ctrl_t g_fake_vm_ctrl = {0};
fake_vm_t g_fake_vm = {0};

void fake_vm_reset(void)
{
	g_fake_vm_ctrl = (fake_vm_ctrl_t){0};
	g_fake_vm = (fake_vm_t){0};
	g_fake_vm.next_run_ret = LEXLEO_VM_STATUS_OK;
}

void __wrap_lexleo_vm_destroy(lexleo_vm_t **vm)
{
	g_fake_vm_ctrl.destroy_call_count++;
	g_fake_vm_ctrl.last_destroy_vm = vm;
	if (!vm || !*vm) {
		return;
	}
	g_fake_vm_ctrl.last_destroy_vm_value = vm ? *vm : NULL;
	*vm = NULL;
}

lexleo_vm_status_t __wrap_lexleo_vm_run(lexleo_vm_t *vm)
{
	assert_non_null(vm);
	fake_vm_t *fake_vm = fake_vm_real_to_fake(vm);
	fake_vm->run_call_count++;
	fake_vm->last_run_vm = vm;
	if (fake_vm->next_run_ret != LEXLEO_VM_STATUS_OK) {
		return fake_vm->next_run_ret;
	}
	// ...
	return LEXLEO_VM_STATUS_OK;
}
