@page specifications_stream_factory_add_adapter stream_factory_add_adapter() specifications

# Signature

```c
stream_factory_status_t stream_factory_add_adapter(
	stream_factory_t *fact,
	stream_adapter_id_t adapter_id,
	stream_adapter_provider_t *adapter_provider);
```

# Purpose

Register `adapter_provider` with `fact` under `adapter_id`.

# Preconditions

- `fact` must point to a valid `stream_factory_t` created by
  `stream_create_factory()`.
- `adapter_id` must be a valid adapter identifier.
- `adapter_provider` must point to a valid `stream_adapter_provider_t` created
  by an adapter-specific Composition Root service.

# Success

- Returns `STREAM_FACTORY_STATUS_OK`.
- Registers `adapter_provider` with `fact` under `adapter_id`.
- Transfers ownership of `adapter_provider` to `fact`.
- A specialized creator can subsequently be constructed by passing `fact` and
  `adapter_id` to the corresponding `stream_create_*_creator()`
  function.
- A client of the owner-facing `stream` API can subsequently use that creator
  and the corresponding creation arguments, described by the fields of the
  associated `stream_*_creator_args_t`, to construct `stream_t` handles at
  runtime using the associated `stream_*_creator_create()` function.

# Failure

- Returns `STREAM_FACTORY_STATUS_FULL` if `fact` has reached its registration
  capacity.
- Returns `STREAM_FACTORY_STATUS_ALREADY_EXISTS` if an adapter provider is
  already registered under `adapter_id`.
- Leaves `fact` unchanged.
- Does not transfer ownership of `adapter_provider`.

# Ownership

- Ownership of `fact` is not transferred.
- On success, ownership of `adapter_provider` is transferred to `fact`.
- On failure, ownership of `adapter_provider` remains with the caller.
- `adapter_id` is borrowed by `fact` and must remain valid for as long as the
  adapter provider remains registered.
