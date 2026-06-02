@page specifications_lexleo_vm_run lexleo_vm_run() specifications

# Signature

```c
lexleo_vm_status_t lexleo_vm_run(lexleo_vm_t *vm);
```

# Purpose

Run a previously created and initialized LexLeo VM handle.

# Preconditions

- The VM must have been created successfully with `lexleo_vm_create()`.
- The VM must have been successfully initialized by
  `lexleo_vm_complete_default_init()`.

# Success

- Returns `LEXLEO_VM_STATUS_OK`.

# Failure

- No runtime failure status is specified yet.

# Ownership

- `lexleo_vm_run()` does not destroy the VM handle, which must be destroyed
  later by `lexleo_vm_destroy()`.

# Notes

- `lexleo_vm_run()` is currently a placeholder runtime entry point.
