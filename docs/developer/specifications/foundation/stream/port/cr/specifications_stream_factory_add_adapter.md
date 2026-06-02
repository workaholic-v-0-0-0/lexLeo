@page specifications_stream_factory_add_adapter stream_factory_add_adapter() specifications

# Signature

~~~c
stream_status_t stream_factory_add_adapter(
    stream_factory_t *fact,
    const stream_adapter_desc_t *desc);
~~~

# Purpose

Register a stream adapter descriptor into a stream factory.

# Preconditions

- If `fact != NULL`, `fact` must point to a valid factory instance created by
  `stream_create_factory()`.
- The internal registry state of `fact` must not have been corrupted.

# Invalid arguments and state

- `fact` must not be `NULL`.
- `desc` must not be `NULL`.
- `desc->key` must not be `NULL`.
- `desc->key` must not point to an empty string.
- `desc->ctor` must not be `NULL`.
- If `desc->ud != NULL`, `desc->ud_dtor` must not be `NULL`.
- The factory registry storage must be initialized.
- The factory registry capacity must be greater than zero.

# Success

- Returns `STREAM_STATUS_OK`.
- Registers the descriptor under `desc->key`.
- Stores the descriptor key, constructor, user data, and user-data destructor
  in the next available registry entry.
- Increments the factory registry count by one.
- A later call to `stream_factory_create_stream()` with the registered key can
  resolve the descriptor.

# Failure

- Returns `STREAM_STATUS_INVALID` for invalid arguments or invalid registry
  state.
- Returns `STREAM_STATUS_FULL` if the registry count has reached its capacity.
- Returns `STREAM_STATUS_ALREADY_EXISTS` if the registry is not full and the
  key is already registered.
- Leaves the factory registry unchanged on failure.

# Ownership

- Ownership of `fact` is not transferred.
- Ownership of `desc` itself is not transferred.
- The descriptor structure is not retained.
- The factory stores a borrowed reference to `desc->key`; the referenced string
  must remain valid and unchanged until the factory is destroyed.
- On successful registration, ownership of non-NULL `desc->ud` is transferred
  to the factory registry.
- Successfully transferred user data is later released by
  `stream_destroy_factory()` through `desc->ud_dtor`.
- On failure, ownership of `desc->ud` remains with the caller and
  `desc->ud_dtor` is not called.

# Notes

- Registration does not copy the key string or allocate registry storage.
- If registration fails because the key already exists, the previously
  registered descriptor remains associated with that key.
- If registration fails because the registry is full, the new descriptor is
  not registered.
- Capacity exhaustion is checked before duplicate-key detection. Therefore, a
  call made on a full registry returns `STREAM_STATUS_FULL`, even if the
  supplied key is already registered.