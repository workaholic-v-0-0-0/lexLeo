@page specifications_stream_create stream_create() specifications

# Signature

```c
stream_status_t stream_create(
    stream_t **out,
    const stream_vtbl_t *vtbl,
    void *backend,
    const stream_env_t *env);
```

# Purpose

Create the public `stream_t` handle of the `stream` port from a borrower-facing
virtual table, a backend pointer, and injected runtime dependencies.

# Invalid arguments

- `out` must not be `NULL`.
- `vtbl` must not be `NULL`.
- `env` must not be `NULL`.
- `env->mem` must not be `NULL`.
- `vtbl->read` must not be `NULL`.
- `vtbl->write` must not be `NULL`.
- `vtbl->flush` must not be `NULL`.
- `vtbl->close` must not be `NULL`.

# Success

- Returns `STREAM_STATUS_OK`.
- Stores a valid newly created `stream_t` handle in `*out`.
- The produced stream handle must later be destroyed via `stream_destroy()`.

# Failure

- Returns `STREAM_STATUS_INVALID` for invalid arguments.
- Returns `STREAM_STATUS_OOM` on allocation failure.
- Leaves `*out` unchanged if `out` is not `NULL`.

# Notes

- `stream_create()` validates the structural well-formedness of the borrower-
  facing virtual table at creation time.
- The `backend` pointer is stored as provided in the created stream handle.
- The created handle is owned by the caller.
- `stream_destroy()` is the matching destructor for handles created by
  `stream_create()`.
