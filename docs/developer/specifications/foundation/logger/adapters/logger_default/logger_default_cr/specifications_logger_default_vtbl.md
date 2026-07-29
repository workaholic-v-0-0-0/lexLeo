@page specifications_logger_default_vtbl logger_default_vtbl() specifications

# Signature

```c
const logger_vtbl_t *logger_default_vtbl(void);
```

# Purpose

Return the `logger` port dispatch table implemented by the `logger_default`
adapter.

# Preconditions

- None.

# Success

- Returns a non-`NULL` pointer to a valid `logger_vtbl_t`.
- The returned dispatch table contains the `logger_default` adapter
  implementations of the `logger` log and destroy operations.
- All mandatory operation pointers in the returned dispatch table are
  non-`NULL`.
- Repeated calls return a dispatch table with the same operation bindings.

# Failure

- None.

# Ownership

- Ownership of the returned `logger_vtbl_t` is not transferred to the caller.
- The caller must not modify or release the returned dispatch table.

# Notes

- The returned dispatch table is intended to bind a `logger_default_t` backend
  to the `logger` port.
- The operation implementations referenced by the returned table are internal
  to the `logger_default` adapter.
- The lifetime of the returned dispatch table is independent of individual
  `logger_default_t` backend instances.
