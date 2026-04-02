@page specifications_logger_default_destroy logger_default_destroy() specifications

# Signature

    static void logger_default_destroy(void *backend);

# Purpose

Destroy the private `logger_default` backend object.

# Preconditions

- If `backend != NULL`, it must designate a valid `logger_default_t` backend
  object.
- If `backend != NULL`, the designated backend must contain a valid memory
  operations table with a callable `free` function.
- These preconditions are enforced as internal invariants, not as recoverable
  public argument validation.

# Invalid arguments

- None.

# Success

- If `backend == NULL`, does nothing.
- If `backend != NULL`, releases the private backend object through the
  injected adapter memory operations table.

# Failure

- None.

# Ownership

- Consumes ownership of the private backend object designated by `backend`,
  when non-`NULL`.
- Does not destroy the borrowed target stream.
- Does not destroy the borrowed time operations table.

# Notes

- This function is the private backend destructor wired into the
  `logger_default` adapter vtable.
- Destruction of the public `logger_t` handle is handled separately by the
  `logger` port lifecycle.