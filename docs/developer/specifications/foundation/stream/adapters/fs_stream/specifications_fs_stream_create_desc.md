@page specifications_fs_stream_create_desc fs_stream_create_desc() specifications

# Signature

```c
stream_status_t fs_stream_create_desc(
    stream_adapter_desc_t *out,
    stream_key_t key,
    const fs_stream_cfg_t *cfg,
    const fs_stream_env_t *env,
    const osal_mem_ops_t *mem);
```

# Purpose

Build an adapter descriptor for registering `fs_stream` in a factory.

# Preconditions

- If `cfg != NULL`, `cfg` must point to a valid `fs_stream_cfg_t`.
- If `env != NULL`, `env` must point to a valid `fs_stream_env_t`.
- If `mem != NULL`, `mem` must point to a valid allocator suitable for later
  destruction of descriptor-owned user data through `ud_dtor()`.

# Invalid arguments

- `out` must not be `NULL`.
- `key` must not be `NULL`.
- `key` must not be empty.
- `cfg` must not be `NULL`.
- `env` must not be `NULL`.
- `mem` must not be `NULL`.

# Success

- Returns `STREAM_STATUS_OK`.
- Stores a valid adapter descriptor in `*out`.
- Binds the provided `key`, `cfg`, and `env` into the produced descriptor.
- The produced descriptor is suitable for registration into the `stream`
  factory.
- The produced descriptor must later be released via `out->ud_dtor()`.

# Failure

- Returns `STREAM_STATUS_INVALID` for invalid arguments.
- Returns `STREAM_STATUS_OOM` if allocation of descriptor-owned user data
  fails.
- If `out` is not `NULL`, resets `*out` to an empty descriptor.

# Ownership

- On success, ownership of the descriptor-owned user data is transferred to the
  produced descriptor.
- The caller remains responsible for the storage designated by `out`.
- The produced descriptor-owned user data must later be released through
  `out->ud_dtor()`.
- On failure, no descriptor-owned user data ownership is transferred.

# Notes

- The Composition Root uses this function to prepare a
  `stream_adapter_desc_t` suitable for registration, for example via
  `stream_factory_add_adapter()`.
- The allocator passed in `mem` defines the lifetime management policy for the
  descriptor-owned user data.
- On failure, the descriptor output is normalized to an empty descriptor when
  possible.
