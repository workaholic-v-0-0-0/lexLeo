@page specifications_logger_destroy logger_destroy() specifications

# Signature

    void logger_destroy(logger_t **l);

# Purpose

Destroy a previously created public `logger_t` handle of the `logger` port.

# Success

- If `l == NULL`, the function does nothing.
- If `l != NULL` and `*l == NULL`, the function does nothing.
- Otherwise, the function releases the logger object referenced by `*l`.
- After destruction, `*l` is set to `NULL`.

# Failure

- None.

# Notes

- `logger_destroy()` is the matching destructor for handles created by
  `logger_create()`.
- This function provides a best-effort idempotent destruction pattern at the
  handle level because passing a `NULL` handle pointer or an already-`NULL`
  handle is accepted as a no-op.
