@page specifications_stdio_stream_create_desc stdio_stream_create_desc() specifications

# Signature

~~~c
stream_status_t stdio_stream_create_desc(
    stream_adapter_desc_t *out,
    stream_key_t key,
    const stdio_stream_cfg_t *cfg,
    const stdio_stream_env_t *env,
    const osal_mem_ops_t *mem);
~~~

# Purpose

Initialize a caller-provided adapter descriptor for registering `stdio_stream`
in a factory.

# Preconditions

- If `cfg != NULL`, `cfg` must point to a valid
  `stdio_stream_cfg_t`.
- If `env != NULL`, `env` must point to a valid
  `stdio_stream_env_t`.
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
- Sets `out->ctor` to a non-`NULL` `stdio_stream` constructor compatible with
  `stream_ctor_fn_t`.
- Sets `out->ud` to non-`NULL` descriptor-owned user data.
- Sets `out->ud_dtor` to a non-`NULL` destructor suitable for releasing
  `out->ud`.
- The produced descriptor is suitable for registration into the `stream`
  factory.

# Failure

- Returns `STREAM_STATUS_INVALID` for invalid arguments.
- Returns `STREAM_STATUS_OOM` if allocation of descriptor-owned constructor
  data fails.
- If `out` is not `NULL`, resets `*out` to an empty descriptor.

# Ownership

- On success, ownership of the descriptor-owned constructor data is transferred
  to the produced descriptor.
- The caller remains responsible for the storage designated by `out`.
- The descriptor-owned constructor data must later be released via
  `out->ud_dtor()`.
- On failure, no descriptor-owned data ownership is transferred.

# Notes

- This helper prepares a `stream_adapter_desc_t` suitable for registration,
  for example via `stream_factory_add_adapter()`.
- The constructor user data allocated by this function is owned by the
  descriptor lifecycle.
- The allocator passed in `mem` defines the lifetime management policy for the
  descriptor-owned constructor data.
- On failure, the descriptor output is normalized to an empty descriptor when
  possible.
