@page specifications_stream_factory_create_stream stream_factory_create_stream() specifications

# Signature

~~~c
stream_status_t stream_factory_create_stream(
    const stream_factory_t *f,
    stream_key_t key,
    const void *args,
    stream_t **out);
~~~

# Purpose

Create a `stream_t` handle from a registered factory adapter key and
adapter-specific creation arguments.

# Preconditions

- If `f != NULL`, `f` must point to a valid factory instance created by
  `stream_create_factory()`.
- If `args != NULL`, `args` must point to an accessible argument object whose
  type is appropriate for the adapter selected by `key`.

# Invalid arguments

- `f` must not be `NULL`.
- `key` must not be `NULL`.
- `key` must not be empty.
- `out` must not be `NULL`.

# Success

- Returns `STREAM_STATUS_OK`.
- Resolves the adapter descriptor registered under `key`.
- Delegates stream creation to the resolved adapter constructor.
- Stores a valid newly created `stream_t` handle in `*out`.

# Failure

- Returns `STREAM_STATUS_INVALID` for invalid arguments.
- Returns `STREAM_STATUS_NOT_FOUND` if `key` is not registered in the factory.
- Propagates argument-validation and creation failures returned by the resolved
  adapter constructor.
- Leaves `*out` unchanged if `out != NULL`.

# Ownership

- Ownership of `f`, `key`, and `args` is not transferred.
- On success, ownership of the produced `stream_t` handle is transferred to the
  caller.
- The produced handle must later be released via `stream_destroy()`.
- On failure, no stream ownership is transferred.

# Notes

- The exact validity rules for `args` are adapter-specific.
- Some adapters may accept `args == NULL`.
