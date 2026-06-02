@page specifications_stream_create_factory stream_create_factory() specifications

# Signature

~~~c
stream_status_t stream_create_factory(
    stream_factory_t **out,
    const stream_factory_cfg_t *cfg,
    const osal_mem_ops_t *mem);
~~~

# Purpose

Create a `stream_factory_t` handle from factory configuration and memory
operations.

# Preconditions

- If `mem != NULL`, `mem` points to a valid `osal_mem_ops_t` object.

# Invalid arguments

- `out` must not be `NULL`.
- `cfg` must not be `NULL`.
- `mem` must not be `NULL`.

# Success

- Returns `STREAM_STATUS_OK`.
- Stores a valid newly created `stream_factory_t` handle in `*out`.
- Initializes the factory registry with capacity `cfg->fact_cap`.
- If `cfg->fact_cap == 0`, creates an empty registry without preallocating
  registry entries.

# Failure

- Returns `STREAM_STATUS_INVALID` for invalid arguments.
- Returns `STREAM_STATUS_OOM` if allocation of the factory handle or its initial
  registry storage fails.
- Leaves `*out` unchanged if `out != NULL`.

# Ownership

- Ownership of `cfg` and `mem` is not transferred.
- The factory stores a borrowed reference to `mem`.
- On success, ownership of the produced `stream_factory_t` handle is
  transferred to the caller.
- The produced handle must later be released via `stream_destroy_factory()`.
- On failure, no factory ownership is transferred.
