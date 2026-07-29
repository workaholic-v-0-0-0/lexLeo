@page specifications_lexleo_vm_create_init lexleo_vm_create_init() specifications

# Signature

```c
lexleo_vm_status_t lexleo_vm_create_init(
    lexleo_vm_t **out,
    const lexleo_vm_env_t *env,
    const lexleo_vm_cfg_t *cfg
);
```

# Purpose

Create and fully initialize a LexLeo VM handle using a borrowed runtime
environment and a configuration value.

# Preconditions

- `out != NULL`.
- `env != NULL`.
- `env` must be supported.
- `env` must be well-formed.
- `cfg != NULL`.
- `cfg` must be supported.
- `cfg` must be well-formed.
- `*out` does not need to be initialized before the call.

# Success

- Returns `LEXLEO_VM_STATUS_OK`.
- Stores a valid, fully initialized `lexleo_vm_t` handle in `*out`.
- Stores the borrowed dependencies provided by `env`.
- Initializes the VM owned runtime resources according to `cfg`.

# Failure

- Returns the failure status reported by `lexleo_vm_create()` or
  `lexleo_vm_complete_default_init()`.
- Leaves `*out` unchanged.

# Ownership

- The created VM handle is owned by the caller.
- The handle must later be destroyed by `lexleo_vm_destroy()`.
- The owned runtime resources are owned by the VM.
- Ownership of `env` and `cfg` is not transferred.
- The dependencies stored from `env` are borrowed by the VM.

# Notes

- Combines VM creation and default initialization in one call.
