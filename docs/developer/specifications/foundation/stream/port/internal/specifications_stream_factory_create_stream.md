@page specifications_stream_factory_create_stream stream_factory_create_stream() specifications

# Signature

~~~c
stream_factory_status_t stream_factory_create_stream(
    const stream_factory_t *factory,
    stream_adapter_id_t adapter_id,
    const void *args,
    stream_t **out);
~~~

# Purpose

Create a `stream_t` handle using a registered adapter provider and
adapter-specific creation arguments.

# Preconditions

- `factory` must point to a valid `stream_factory_t` instance created by
  `stream_create_factory()`.
- `adapter_id` must not be `NULL`.
- `adapter_id` must not be an empty string.
- If `args != NULL`, `args` must point to a valid `stream_*_creator_args_t`
  appropriate for the adapter identified by `adapter_id`.
- `out` must not be `NULL`.

# Success

- Returns `STREAM_FACTORY_STATUS_OK`.
- Resolves the adapter provider registered in `factory` under `adapter_id`.
- Delegates backend construction to the constructor of the resolved adapter
  provider.
- Creates a valid `stream_t` handle bound to the constructed backend and to the
  virtual table of the resolved adapter provider.
- Stores the valid newly created `stream_t` handle in `*out`.

# Failure

- Returns `STREAM_FACTORY_STATUS_NOT_FOUND` if no adapter provider corresponding to
  `adapter_id` is registered.
- Otherwise, returns the `stream_factory_status_t` error code corresponding to
  the `stream_status_t` error code returned by the constructor of the resolved
  adapter provider.
- Returns `STREAM_FACTORY_STATUS_OOM` if allocation of the `stream_t` handle
  fails.
- Leaves `*out` unchanged.

# Ownership

- Ownership of `factory`, `adapter_id`, and `args` is not transferred.
- On success, ownership of the newly created `stream_t` handle is transferred
  to the caller.
- The produced stream handle must later be destroyed via `stream_destroy()`.
- On failure, no stream ownership is transferred.

# Notes

- The interpretation and validity rules of `args` are adapter-specific.
- Some adapter providers may accept `args == NULL`.
