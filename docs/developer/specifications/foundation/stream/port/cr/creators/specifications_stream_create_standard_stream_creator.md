@page specifications_stream_create_standard_stream_creator stream_create_standard_stream_creator() specifications

# Signature

~~~c
stream_factory_status_t stream_create_standard_stream_creator(
    stream_standard_stream_creator_t **out,
    stream_factory_t *factory,
    stream_adapter_id_t adapter_id,
    const osal_mem_ops_t *mem);
~~~

# Purpose

Create a creator for standard streams.

# Preconditions

- `out` must not be `NULL`.
- `factory` must not be `NULL`.
- `adapter_id` must not be `NULL`.
- `adapter_id` must not be an empty string.
- `adapter_id` must not be registered with `factory`.
- `mem` must point to a valid `osal_mem_ops_t`.

# Success

- Returns `STREAM_FACTORY_STATUS_OK`.
- Stores a valid newly created `stream_standard_stream_creator_t` handle in
  `*out`.
- The returned creator can subsequently be passed to
  `stream_standard_stream_creator_create()` to create standard-stream-backed
  `stream_t` handles.

# Failure

- Returns `STREAM_FACTORY_STATUS_OOM` if allocation fails.
- Leaves `*out` unchanged.

# Ownership

- Ownership of `factory`, `adapter_id`, and `mem` is not transferred.
- On success, ownership of the created `stream_standard_stream_creator_t`
  handle is transferred to the caller.
- The created handle must later be released via
  `stream_destroy_standard_stream_creator()`.
