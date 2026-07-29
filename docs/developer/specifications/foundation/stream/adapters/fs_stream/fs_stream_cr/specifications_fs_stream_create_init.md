@page specifications_fs_stream_create_init fs_stream_create_init() specifications

# Signature

```c
fs_stream_status_t fs_stream_create_init(
    fs_stream_t **out,
    const fs_stream_env_t *env,
    const fs_stream_cfg_t *cfg,
    const char *path,
    const char *mode
);
```

# Purpose

Create and fully initialize a file-stream backend using a borrowed runtime
environment, a configuration value, a file path, and an opening mode.

# Preconditions

- `out != NULL`.
- `env != NULL`.
- `env` must be supported.
- `env` must be well-formed.
- `cfg != NULL`.
- `cfg` must be supported.
- `cfg` must be well-formed.
- `path` must point to a valid, null-terminated file path.
- `mode` must point to a valid, null-terminated opening mode supported by
  `fs_stream_complete_default_init()`.
- `*out` does not need to be initialized before the call.

# Success

- Returns `FS_STREAM_STATUS_OK`.
- Stores a valid, fully initialized `fs_stream_t` backend in `*out`.
- Stores the borrowed dependencies provided by `env`.
- Initializes the backend according to `cfg`, `path`, and `mode`.

# Failure

- Returns the failure status reported by `fs_stream_create()` or
  `fs_stream_complete_default_init()`.
- Leaves `*out` unchanged.

# Ownership

- The created backend is owned by the caller.
- The backend must later be released through `fs_stream_vtbl()->close()`
  or transferred to a stream handle.
- Ownership of `env`, `cfg`, `path`, and `mode` is not transferred.
- The dependencies stored from `env` are borrowed by the backend.

# Notes

- Combines backend creation and default initialization in one call.
