@page specifications_lexleo_vm_create lexleo_vm_create() specifications

# Signature

```c
lexleo_vm_status_t lexleo_vm_create(
    lexleo_vm_t **out,
    const lexleo_vm_cfg_t *cfg,
    const lexleo_vm_env_t *env
);
```

# Purpose

Create the public LexLeo VM handle from a configuration value and a borrowed
runtime environment.

# Preconditions

- `out != NULL`.
- `cfg != NULL`.
- `env != NULL`.
- `env` must be supported.
- `env` must be well-formed.
- `*out` does not need to be initialized before the call.

# Success

- Returns `LEXLEO_VM_STATUS_OK`.
- Stores a valid newly created `lexleo_vm_t` handle in `*out`.
- Stores the borrowed dependencies provided by `env`.
- Does not initialize the owned runtime resources completed later by
  `lexleo_vm_complete_default_init()`.

# Failure

- Returns `LEXLEO_VM_STATUS_OOM` on allocation failure.
- Leaves `*out` unchanged.

# Ownership

- The created VM handle is owned by the caller.
- The created VM handle must later be destroyed by `lexleo_vm_destroy()`.
- The dependencies stored from `env` are borrowed by the VM.
- Owned runtime resources are not created by this function.

# Notes

- `lexleo_vm_create()` only creates the VM handle and stores its configuration
  and borrowed environment.
- Internal owned fields are normally initialized only by
  `lexleo_vm_complete_default_init()`.
