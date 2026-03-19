@page specifications_stream_factory_create_stream stream_factory_create_stream() specifications

# Signature

    stream_status_t stream_factory_create_stream(
        const stream_factory_t *f,
        stream_key_t key,
        const void *args,
        stream_t **out);

# Purpose

Create a `stream_t` handle from a registered factory adapter key and
adapter-specific creation arguments.

# Preconditions

- `f` must point to a valid factory instance previously created by
  `stream_create_factory()`.
- `key` must point to a registered adapter key in `f`.
- `args` must point to a valid argument object for the adapter selected by
  `key`.

# Invalid arguments

- `f` must not be `NULL`.
- `key` must not be `NULL`.
- `key` must not be empty.
- `args` must not be `NULL`.
- `out` must not be `NULL`.

# Success

- Returns `STREAM_STATUS_OK`.
- Resolves the adapter descriptor registered under `key`.
- Stores a valid newly created `stream_t` handle in `*out`.
- The produced stream handle is ready for normal runtime use.
- The produced stream handle must later be destroyed via `stream_destroy()`.

# Failure

- Returns `STREAM_STATUS_INVALID` for invalid arguments.
- Returns `STREAM_STATUS_NOT_FOUND` if `key` is not registered in the factory.
- Propagates adapter-level creation failures returned by the resolved adapter
  constructor.
- Leaves `*out` unchanged if `out` is not `NULL`.

# Notes

- `stream_factory_create_stream()` delegates stream creation to the constructor
  stored in the descriptor registered under `key`.
- The exact validity rules for `args` beyond non-`NULL` are adapter-specific.
- Adapter-specific integrations, such as `fs_stream`, may impose additional
  validation rules on the object designated by `args`.
