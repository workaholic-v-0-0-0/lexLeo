@page specifications_dynamic_buffer_stream_complete_default_init dynamic_buffer_stream_complete_default_init() specifications

# Signature

```c
dynamic_buffer_stream_status_t dynamic_buffer_stream_complete_default_init(
    dynamic_buffer_stream_t *dynamic_buffer_stream,
    const dynamic_buffer_stream_cfg_t *cfg);
```

# Purpose

Complete the initialization of a partially initialized
`dynamic_buffer_stream_t` backend by creating and attaching its owned dynamic
buffer resource from adapter configuration.

# Preconditions

- `dynamic_buffer_stream` must point to a `dynamic_buffer_stream_t` created by
  `dynamic_buffer_stream_create()` whose external borrowed dependencies are
  correctly initialized.
- If the backend's dynamic buffer resource is already initialized, it must
  designate a valid dynamic buffer.
- `cfg` must point to a valid `dynamic_buffer_stream_cfg_t`.
- `cfg->default_cap` must be a valid non-zero capacity.

# Invalid arguments

- None.

# Success

- Returns `DYNAMIC_BUFFER_STREAM_STATUS_OK`.
- If the backend's dynamic buffer resource is already initialized:
    - Leaves `dynamic_buffer_stream` unchanged.
- Otherwise:
    - Creates the dynamic buffer using `cfg->default_cap` and the external
      borrowed memory operations.
    - Attaches the resulting valid dynamic buffer to
      `dynamic_buffer_stream` as an owned resource.
- Leaves all external borrowed dependencies of `dynamic_buffer_stream`
  unchanged.
- The resulting `dynamic_buffer_stream_t` is fully initialized and can be used
  by the `stream_vtbl_t` operations fetchable by
  `dynamic_buffer_stream_vtbl()`.

# Failure

- Returns `DYNAMIC_BUFFER_STREAM_STATUS_OOM` if creation of the owned dynamic
  buffer fails because of an allocation failure.
- Leaves the dynamic buffer resource uninitialized.
- Leaves all external borrowed dependencies of `dynamic_buffer_stream`
  unchanged.
- No dynamic buffer ownership is transferred to `dynamic_buffer_stream`.

# Ownership

- Ownership of `dynamic_buffer_stream` and `cfg` is not transferred.
- The external borrowed dependencies stored in `dynamic_buffer_stream` remain
  borrowed.
- If a new dynamic buffer is created successfully, its ownership is
  transferred to `dynamic_buffer_stream`.
- The owned dynamic buffer resource is later released via
  `dynamic_buffer_stream_t::close()`.

# Notes

- This function completes the initialization started by
  `dynamic_buffer_stream_create()`.
- `dynamic_buffer_stream_create()` initializes the backend's external borrowed
  dependencies.
- `dynamic_buffer_stream_complete_default_init()` initializes the remaining
  backend state, namely internal borrowed dependencies and owned resources when
  applicable.
- In the `dynamic_buffer_stream` adapter, this completion-time state consists
  of the owned dynamic buffer resource.
