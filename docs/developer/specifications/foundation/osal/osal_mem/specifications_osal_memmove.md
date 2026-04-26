@page specifications_osal_memmove osal_memmove() specifications

# Signature

```c
void *osal_memmove(void *dest, const void *src, size_t n);
```

# Purpose

Moves `n` bytes from a source memory area to a destination memory area using the default OSAL memory primitives.
This operation is intended for memory areas that may overlap.

# Success

- Copies exactly `n` bytes from `src` to `dest`.
- Correctly handles overlapping source and destination memory areas.
- Returns `dest`.

# Failure

- This operation does not report failure.

# Ownership

- Ownership of both memory areas is unchanged.
- The caller remains responsible for ensuring that `dest` and `src` designate valid memory areas of at least `n` bytes.

# Notes

- If `n == 0`, the operation performs no byte moves and returns `dest`.
- If `dest == NULL` or `src == NULL` while `n > 0`, behavior is undefined.
- Use `osal_memmove()` instead of `osal_memcpy()` when source and destination ranges may overlap.
