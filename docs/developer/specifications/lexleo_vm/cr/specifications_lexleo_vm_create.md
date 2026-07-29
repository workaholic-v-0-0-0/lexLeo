@page specifications_lexleo_vm_create lexleo_vm_create() specifications

# Signature

```c
lexleo_vm_status_t lexleo_vm_create(
    lexleo_vm_t **out,
    const lexleo_vm_env_t *env
);
```

# Purpose

Create the public LexLeo VM handle and its internal owned-resources
container from a borrowed runtime environment.

# Preconditions

- `out != NULL`.
- `env != NULL`.
- `env` must be supported.
- `env` must be well-formed.
- `*out` does not need to be initialized before the call.

# Success

- Returns `LEXLEO_VM_STATUS_OK`.
- Stores a valid newly created `lexleo_vm_t` handle in `*out`.
- Stores the borrowed dependencies provided by `env`.
- Creates an empty internal owned-resources container.
- Does not initialize the owned runtime resources completed later by
  `lexleo_vm_complete_default_init()`.

# Failure

- Returns `LEXLEO_VM_STATUS_OOM` on allocation failure.
- Leaves `*out` unchanged.

# Ownership

- The created VM handle is owned by the caller.
- The internal owned-resources container is owned by the VM.
- The created VM handle must later be destroyed by `lexleo_vm_destroy()`,
  which also releases its owned-resources container.
- The dependencies stored from `env` are borrowed by the VM.

# Notes

- `lexleo_vm_create()` creates the VM handle and its owned-resources
  container, and stores the borrowed dependencies provided by `env`.
- Creating the owned-resources container does not create the resources
  that it will hold.
- Internal owned fields are normally initialized only by
  `lexleo_vm_complete_default_init()`.
