@page specifications_osal_calloc osal_calloc() specifications

# Signature

```c
void *osal_calloc(size_t nmemb, size_t size);
```

# Purpose

Allocates a zero-initialized block of memory using the default OSAL memory allocator.

# Success

- Returns a non-NULL pointer to a memory block large enough to contain `nmemb` elements of `size` bytes each.
- The returned memory block is initialized to zero.
- The returned pointer can be passed to `osal_free()`.

# Failure

- Returns NULL if allocation fails.

# Ownership

- Ownership of the returned memory block is transferred to the caller.
- The caller is responsible for releasing the block with `osal_free()`.

# Notes

- If `nmemb == 0` or `size == 0`, behavior follows the underlying C runtime allocator.
- If `nmemb * size` overflows, behavior follows the underlying C runtime allocator.
- The returned block must not be accessed after being released with `osal_free()`.
