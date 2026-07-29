@page specifications_fs_stream_create fs_stream_create() specifications

# Signature

```c
fs_stream_status_t fs_stream_create(
    fs_stream_t **out,
    const fs_stream_env_t *env);
```

# Purpose

Create a partially initialized `fs_stream_t` backend from its external
borrowed dependencies.

# Preconditions

- `out` is not `NULL`.
- `env` points to a valid `fs_stream_env_t`.

# Success

- Returns `FS_STREAM_STATUS_OK`.
- Stores a newly created `fs_stream_t` in `*out`.
- Initializes the backend's external borrowed dependencies from `env`,
  including its memory operations and file operations.
- Leaves the backend's owned file resource uninitialized.

# Failure

- Returns `FS_STREAM_STATUS_OOM` on allocation failure.
- Leaves `*out` unchanged.

# Ownership

- Ownership of `env` and the external borrowed dependencies supplied through
  it is not transferred.
- On success, ownership of the produced `fs_stream_t` is transferred to
  the caller.
- The produced `fs_stream_t` must later be released via `fs_stream_t::close()`.

# Notes

- The initialization of the produced `fs_stream_t` must be completed by
  `fs_stream_complete_default_init()` before it can be used.
- `fs_stream_create()` initializes the backend's external borrowed
  dependencies.
- Internal borrowed dependencies and owned resources, when applicable, are
  initialized by `fs_stream_complete_default_init()`.
- In the `fs_stream` adapter, the file is an owned resource and is therefore
  not acquired by this operation.
