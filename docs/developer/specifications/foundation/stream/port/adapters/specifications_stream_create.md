@page specifications_stream_create stream_create() specifications

# Signature

```c
stream_status_t stream_create(
    stream_t **out,
    const stream_env_t *env);
```

# Purpose

Create a `stream_t` handle by initializing its borrowed fields from the
provided environment.

# Preconditions

- `out` must point to a valid `stream_t *`.
- `env` must point to a valid `stream_env_t`.

# Success

- Returns `STREAM_STATUS_OK`.
- Stores a newly created `stream_t` handle in `*out`.
- Initializes all borrowed handle fields (i.e. its memory operations and
  virtual table).
- Initializes all owned handle fields to `NULL`.

# Failure

- Returns `STREAM_STATUS_OOM` on allocation failure.
- Stores `NULL` in `*out`.

# Ownership

- Ownership of `env` and its borrowed dependencies is not transferred.
- On success, ownership of the produced `stream_t` handle is transferred to
  the caller.
- Only the borrowed fields of the produced handle are initialized. Its
  initialization must therefore be completed by
  `stream_complete_default_init()` before the handle can be used.
- The handle must later be released via `stream_destroy()`.

# Notes

- `stream_create()` validates the structural well-formedness of the borrowed
  virtual table.
- This function initializes only the borrowed portion of the handle.
