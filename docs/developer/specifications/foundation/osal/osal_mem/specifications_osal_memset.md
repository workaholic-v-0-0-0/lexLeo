@page specifications_osal_memset osal_memset() specifications

# Signature

```c
void *osal_memset(void *s, int c, size_t n);
```

# Purpose

Fills a memory area with the specified byte value using the default OSAL memory primitives.

# Success

- Writes `n` bytes into the memory area designated by `s`.
- Each written byte is set to the converted unsigned-byte value of `c`.
- Returns `s`.

# Failure

- This operation does not report failure.

# Ownership

- Ownership of the memory area designated by `s` is unchanged.
- The caller remains responsible for ensuring that `s` designates a valid writable memory area of at least `n` bytes.

# Notes

- If `n == 0`, the operation performs no byte writes and returns `s`.
- If `s == NULL` while `n > 0`, behavior is undefined.
- The fill operation is byte-oriented, regardless of the destination object type.
