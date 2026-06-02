@page specifications_lexleo_vm_complete_default_init lexleo_vm_complete_default_init() specifications

# Signature

```c
lexleo_vm_status_t lexleo_vm_complete_default_init(lexleo_vm_t *vm);
```

# Purpose

Complete the default initialization of a LexLeo VM handle by creating its
internal owned runtime resources.

# Preconditions

- `vm != NULL`.
- `vm` must be a valid handle created successfully by `lexleo_vm_create()`.
- Each internal owned field initialized by this function must be `NULL`.

# Success

- Returns `LEXLEO_VM_STATUS_OK`.
- Initializes the default internal owned runtime resources required by the VM.
- Leaves the borrowed dependencies stored during `lexleo_vm_create()`
  unchanged.

# Failure

- Returns `LEXLEO_VM_STATUS_STREAM_FACTORY_INIT_OOM` if the default stream
  factory cannot be created.
- Returns `LEXLEO_VM_STATUS_STREAM_IO_CREATOR_INIT_OOM` if the default stdio
  stream creator cannot be created.
- Returns `LEXLEO_VM_STATUS_STREAM_FILE_CREATOR_INIT_OOM` if the default file
  stream creator cannot be created.
- Returns `LEXLEO_VM_STATUS_STREAM_BUFFER_CREATOR_INIT_OOM` if the default
  buffer stream creator cannot be created.
- The VM may remain partially initialized.
- The caller must still call `lexleo_vm_destroy()` to release the VM handle and
  any owned runtime resources successfully initialized before the failure.

# Ownership

- Each runtime resource successfully initialized by this function is owned by
  the VM handle.
- The VM handle remains owned by the caller.
- The VM handle must be destroyed by `lexleo_vm_destroy()`, whether default
  initialization succeeds or fails.

# Notes

- This function performs the default owned-resource initialization that is not
  done by `lexleo_vm_create()`.
- This function must be called before using runtime entry points that require
  the default owned resources.
- White-box tests may inject dependencies into internal owned fields. Such
  injected dependencies remain owned by the test. Before calling
  `lexleo_vm_destroy()`, the test must destroy them with their matching
  idempotent destructor so that the corresponding field is reset to `NULL`.
