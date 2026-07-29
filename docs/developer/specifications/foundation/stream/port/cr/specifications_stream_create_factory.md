@page specifications_stream_create_factory stream_create_factory() specifications

# Signature

```c
stream_factory_status_t stream_create_factory(
    stream_factory_t **out,
    const stream_factory_cfg_t *cfg,
    const osal_mem_ops_t *mem);
```

# Purpose

Create a `stream_factory_t` handle from a factory configuration and borrowed
memory operations.

# Preconditions

- `out` points to a valid `stream_factory_t *`.
- `mem` points to a valid `osal_mem_ops_t`.
- `cfg` points to a valid `stream_factory_cfg_t`.
- `cfg->fact_cap > 0`

# Success

- Returns `STREAM_FACTORY_STATUS_OK`.
- Stores a valid newly created `stream_factory_t` handle in `*out`.
- Initializes the factory registry with capacity `cfg->fact_cap`.

# Failure

- Returns `STREAM_FACTORY_STATUS_OOM` if allocation of the factory handle,
  the stream constructor user data, or the initial registry storage fails.
- Leaves `*out` unchanged.

# Ownership

- Ownership of `cfg` and `mem` is not transferred.
- The factory stores a borrowed reference to `mem`.
- On success, ownership of the produced `stream_factory_t` handle is
  transferred to the caller.
- The produced handle shall later be released with
  `stream_destroy_factory()`.
- On failure, no factory ownership is transferred.
