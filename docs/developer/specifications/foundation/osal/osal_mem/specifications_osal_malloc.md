@page specifications_osal_malloc osal_malloc() specifications

# Signature

```c
void *osal_malloc(size_t size);
```

# Purpose

Allocates a block of memory using the default OSAL memory allocator.

# Success

- Returns a non-NULL pointer to a memory block of at least `size` bytes.
- The returned memory block is uninitialized.
- The returned pointer can be passed to `osal_free()`.

# Failure

- Returns NULL if allocation fails.

# Ownership

- Ownership of the returned memory block is transferred to the caller.
- The caller is responsible for releasing the block with `osal_free()`.

# Notes

- If `size == 0`, behavior follows the underlying C runtime allocator.
- The returned block must not be accessed after being released with `osal_free()`.
