@page specifications_stream_create stream_create() specifications

# Signature

```c
stream_status_t stream_create(
    stream_t **out,
    const stream_env_t *env);
```

# Purpose

Create a partially initialized `stream_t` handle from its external borrowed
dependencies.

# Preconditions

- `out` must not be `NULL` and must point to a valid `stream_t *`.
- `env` must point to a valid `stream_env_t`.

# Success

- Returns `STREAM_STATUS_OK`.
- Stores a newly created `stream_t` handle in `*out`.
- Initializes the handle's external borrowed dependencies from `env`, including
  its memory operations and virtual table.
- Leaves the handle's remaining backend state uninitialized.

# Failure

- Returns `STREAM_STATUS_OOM` on allocation failure.
- Leaves `*out` unchanged.

# Ownership

- Ownership of `env` and the external borrowed dependencies supplied through
  it is not transferred.
- On success, ownership of the produced `stream_t` handle is transferred to
  the caller.
- The handle must later be released via `stream_destroy()`.

# Notes

- The initialization of the produced handle must be completed by
  `stream_complete_default_init()` before it can be used.
- `stream_create()` initializes the handle's external borrowed dependencies.
- Internal borrowed dependencies and owned resources, when applicable, are
  initialized by `stream_complete_default_init()`.
