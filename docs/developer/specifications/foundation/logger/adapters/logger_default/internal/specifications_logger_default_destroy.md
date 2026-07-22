@page specifications_logger_default_destroy logger_default_destroy() specifications

# Signature

```c
static logger_status_t logger_default_destroy(void *backend);
```

# Purpose

Destroy the private `logger_default` backend object.

# Preconditions

- `backend` must designate a valid `logger_default_t` backend object.
- The designated backend must contain a valid memory operations table with a
  callable `free` function.
- These preconditions are enforced as internal invariants, not as recoverable
  public argument validation.

# Invalid arguments

- None.

# Success

- Releases the private backend object through the injected adapter memory
  operations table.
- Returns `LOGGER_STATUS_OK`.

# Failure

- None.

# Ownership

- Consumes ownership of the private backend object designated by `backend`.
- Does not destroy the borrowed target stream.
- Does not destroy the borrowed time operations table.

# Notes

- This function is the private backend destructor wired into
  `logger_vtbl_t::destroy()`.
- It is normally invoked indirectly through `logger_destroy()`.
- Destruction of the public `logger_t` handle is handled separately by the
  `logger` port lifecycle.
