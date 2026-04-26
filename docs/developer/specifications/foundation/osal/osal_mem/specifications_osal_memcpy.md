@page specifications_osal_memcpy osal_memcpy() specifications

# Signature

```c
void *osal_memcpy(void *dest, const void *src, size_t n);
```

# Purpose

Copies `n` bytes from a source memory area to a destination memory area using the default OSAL memory primitives.
This operation is intended for non-overlapping memory areas.

# Success

- Copies exactly `n` bytes from `src` to `dest`.
- Returns `dest`.

# Failure

- This operation does not report failure.

# Ownership

- Ownership of both memory areas is unchanged.
- The caller remains responsible for ensuring that `dest` and `src` designate valid memory areas of at least `n` bytes.

# Notes

- If `n == 0`, behavior follows the underlying C runtime.
- If `dest == NULL` or `src == NULL` while `n > 0`, behavior is undefined.
- If the source and destination memory areas overlap, behavior is undefined; use `osal_memmove()` for overlapping areas.
