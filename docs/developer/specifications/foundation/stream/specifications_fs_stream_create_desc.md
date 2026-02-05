@page specifications_fs_stream_create_desc fs_stream_create_desc() specifications

# Signature

    stream_status_t fs_stream_create_desc(
        stream_adapter_desc_t *out,
        stream_key_t key,
        const fs_stream_cfg_t *cfg,
        const fs_stream_env_t *env,
        const osal_mem_ops_t *mem);

# Purpose

Build an adapter descriptor for registering `fs_stream` in a factory.

# Preconditions

- `cfg` must designate a valid `fs_stream_cfg_t`.
- `env` must designate a valid `fs_stream_env_t`.
- `mem` must designate the allocator that will later be used to destroy the
  descriptor-owned user data through `ud_dtor`.

# Invalid arguments

- `out` must not be `NULL`.
- `key` must not be `NULL`.
- `key` must not be empty.
- `cfg` must not be `NULL`.
- `env` must not be `NULL`.
- `mem` must not be `NULL`.
- `env->file_env.mem` must not be `NULL`.
- `env->file_ops` must not be `NULL`.

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

# Notes

- The Composition Root uses this function to prepare a
  `stream_adapter_desc_t` suitable for registration, for example via
  `stream_factory_add_adapter()`.
- The allocator passed in `mem` defines the lifetime management policy for the
  descriptor-owned user data.
- On failure, the descriptor output is normalized to an empty descriptor when
  possible.
