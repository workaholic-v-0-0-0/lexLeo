@page specifications_osal_isspace osal_isspace() specifications

# Signature

```c
int osal_isspace(int c);
```

# Purpose

Tests whether a character is classified as whitespace.

# Preconditions

- `c` must be representable as an `unsigned char` or be equal to `EOF`.

# Success

- Returns a non-zero value if `c` is a whitespace character.
- Returns `0` if `c` is not a whitespace character.

# Failure

- This operation does not report failure.

# Ownership

- No ownership is transferred.