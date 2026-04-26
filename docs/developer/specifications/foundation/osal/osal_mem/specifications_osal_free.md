@page specifications_osal_free osal_free() specifications

# Signature

```c
void osal_free(void *ptr);
```

# Purpose

Releases a memory block previously allocated by the default OSAL memory allocator.

# Success

- Releases the memory block referenced by `ptr`.
- After the call, the released block must no longer be used by the caller.

# Failure

- This operation does not report failure.

# Ownership

- Ownership of the memory block referenced by `ptr` is relinquished by the caller.
- After the call, the caller no longer owns the released block.

# Notes

- If `ptr == NULL`, behavior follows the underlying C runtime deallocator.
- `ptr` should designate a block obtained from a compatible OSAL allocation primitive.
- Passing an invalid pointer or a pointer already released results in undefined behavior
  unless a dedicated test double specifies otherwise.
