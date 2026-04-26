@page specifications_osal_realloc osal_realloc() specifications

# Signature

```c
void *osal_realloc(void *ptr, size_t size);
```

# Purpose

Changes the size of a memory block using the default OSAL memory allocator.

# Success

- Returns a non-NULL pointer to a memory block of at least `size` bytes.
- The returned pointer can be passed to `osal_free()`.
- If `ptr != NULL`, the contents of the old block are preserved up to the smaller of the old and new sizes.
- The returned pointer may be different from `ptr`.

# Failure

- Returns NULL if reallocation fails.
- If reallocation fails and `ptr != NULL`, the original block remains valid and owned by the caller.

# Ownership

- On success, ownership of the returned memory block is transferred to the caller.
- On success, if the returned pointer is different from `ptr`, the old block must no longer be used.
- On failure, ownership of the original block remains with the caller.

# Notes

- If `ptr == NULL`, behavior follows allocation semantics.
- If `size == 0`, behavior follows the underlying C runtime allocator.
- `ptr` should designate a block obtained from a compatible OSAL allocation primitive.
