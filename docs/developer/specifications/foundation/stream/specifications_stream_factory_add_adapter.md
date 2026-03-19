@page specifications_stream_factory_add_adapter stream_factory_add_adapter() specifications

# Signature

    stream_status_t stream_factory_add_adapter(
        stream_factory_t *fact,
        const stream_adapter_desc_t *desc);

# Purpose

Register a stream adapter descriptor into an existing `stream_factory_t`
instance.

# Preconditions

- `fact` must point to a valid factory instance previously created by
  `stream_create_factory()`.

# Invalid arguments

- `fact` must not be `NULL`.
- `desc` must not be `NULL`.
- `desc->key` must not be `NULL`.
- `desc->key` must not be empty.
- `desc->ctor` must not be `NULL`.

# Success

- Returns `STREAM_STATUS_OK`.
- Registers the provided descriptor under `desc->key`.
- A later call to `stream_factory_create_stream()` with the registered key can
  resolve the descriptor.

# Failure

- Returns `STREAM_STATUS_INVALID` for invalid arguments.
- Returns `STREAM_STATUS_ALREADY_EXISTS` if the key is already registered.
- Returns `STREAM_STATUS_FULL` if the factory registration capacity is
  exhausted.

# Notes

- Successful registration makes the descriptor reachable through
  `stream_factory_create_stream()`.
- If registration fails because the key already exists, the previously
  registered descriptor remains the one associated with that key.
- If registration fails because the factory capacity is exhausted, the new
  descriptor is not registered.
