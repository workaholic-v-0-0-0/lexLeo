@page specifications_dynamic_buffer_stream_create_desc dynamic_buffer_stream_create_desc() specifications

# Signature

```c
stream_status_t dynamic_buffer_stream_create_desc(
    stream_adapter_desc_t *out,
    stream_key_t key,
    const dynamic_buffer_stream_cfg_t *cfg,
    const dynamic_buffer_stream_env_t *env,
    const osal_mem_ops_t *mem);
```

# Purpose

Build a `stream` adapter descriptor for the `dynamic_buffer_stream` adapter.

# Preconditions

- If `cfg != NULL`, `cfg` must point to a valid
  `dynamic_buffer_stream_cfg_t`.
- If `env != NULL`, `env` must point to a valid
  `dynamic_buffer_stream_env_t`.
- If `mem != NULL`, `mem` must point to a valid allocator suitable for
  descriptor-owned constructor data allocation and destruction.

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
- Sets `out->key` to the provided `key`.
- Sets `out->ctor` to a non-`NULL` constructor compatible with
  `stream_ctor_fn_t`.
- Sets `out->ud` to non-`NULL` descriptor-owned user data.
- Sets `out->ud_dtor` to a non-`NULL` destructor suitable for releasing
  `out->ud`.
- The produced descriptor is suitable for registration into a `stream` factory.
- The produced descriptor must later be released via `out->ud_dtor()`.

# Failure

- Returns `STREAM_STATUS_INVALID` for invalid arguments.
- Returns `STREAM_STATUS_OOM` if allocation of descriptor-owned constructor
  data fails.
- If `out` is not `NULL`, resets `*out` to an empty descriptor.

# Ownership

- Ownership of `cfg` and `env` is not transferred.
- The caller remains responsible for the storage designated by `out`.
- The produced descriptor-owned user data must later be released through
  `out->ud_dtor()`.
- On failure, no descriptor-owned user data ownership is transferred.

# Notes

- The allocator passed in `mem` defines the lifetime management policy for the
  descriptor-owned user data.
