@page specifications_stream_destroy_factory stream_destroy_factory() specifications

# Signature

    void stream_destroy_factory(stream_factory_t **fact);

# Purpose

Destroy a previously created `stream_factory_t` handle.

# Success

- If `fact == NULL`, the function does nothing.
- If `fact != NULL` and `*fact == NULL`, the function does nothing.
- Otherwise, the function releases the factory object referenced by `*fact`.
- After destruction, `*fact` is set to `NULL`.

# Failure

- None.

# Notes

- `stream_destroy_factory()` is the matching destructor for handles created by
  `stream_create_factory()`.
- This function provides a best-effort idempotent destruction pattern at the
  handle level because passing a `NULL` handle pointer or an already-`NULL`
  handle is accepted as a no-op.
