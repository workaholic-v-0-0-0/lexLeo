@page specifications_stream_create_init stream_create_init() specifications

# Signature

```c
stream_status_t stream_create_init(
    stream_t **out,
    const stream_env_t *env,
    void *backend
);
```

# Purpose

Create and fully initialize a stream handle using a borrowed runtime
environment and an adapter backend.

# Preconditions

- `out != NULL`.
- `env != NULL`.
- `env` must be supported.
- `env` must be well-formed.
- `backend` must be a valid adapter backend compatible with `env`.
- `*out` does not need to be initialized before the call.

# Success

- Returns `STREAM_STATUS_OK`.
- Stores a valid, fully initialized `stream_t` handle in `*out`.
- Stores the borrowed dependencies provided by `env`.
- Binds `backend` to the created stream.

# Failure

- Returns the failure status reported by `stream_create()` or
  `stream_complete_default_init()`.
- Leaves `*out` unchanged.
- Ownership of `backend` remains with the caller.

# Ownership

- The created stream handle is owned by the caller.
- The handle must later be destroyed by `stream_destroy()`.
- On success, ownership of `backend` is transferred to the stream.
- Ownership of `env` is not transferred.
- The dependencies stored from `env` are borrowed by the stream.

# Notes

- Combines stream creation and default initialization in one call.
- The adapter backend is created outside the `stream` port.
