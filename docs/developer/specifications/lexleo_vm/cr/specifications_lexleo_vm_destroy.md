@page specifications_lexleo_vm_destroy lexleo_vm_destroy() specifications

# Signature

```c
void lexleo_vm_destroy(lexleo_vm_t **vm);
```

# Purpose

Destroy a LexLeo VM handle and release every owned runtime resource attached
to it.

# Preconditions

- `vm` may be `NULL`.
- `*vm` may be `NULL` if `vm != NULL`.
- If `vm != NULL` and `*vm != NULL`, `*vm` must be either:
    - a valid handle created by `lexleo_vm_create()`, or
    - a valid partially initialized handle left after a failed
      `lexleo_vm_complete_default_init()` call.

# Success

- Releases every owned runtime resource attached to the VM handle.
- Releases the VM handle itself.
- Resets `*vm` to `NULL` if `vm != NULL`.

# Failure

- This function cannot fail.

# Ownership

- After the call, the caller no longer owns the VM handle.
- Borrowed dependencies stored in the VM environment are not destroyed.

# Notes

- This function is idempotent.
- It is safe to call this function after a failed
  `lexleo_vm_complete_default_init()` call.
