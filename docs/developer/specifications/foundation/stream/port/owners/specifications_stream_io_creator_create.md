@page specifications_stream_io_creator_create stream_io_creator_create() specifications

# Signature

~~~c
stream_status_t stream_io_creator_create(
    const stream_io_creator_t *creator,
    stream_io_kind_t kind,
    stream_t **out);
~~~

# Purpose

Create a standard-I/O-backed stream through an I/O stream creator.

# Preconditions

- `creator` must point to a valid `stream_io_creator_t`.

# Invalid arguments

- Same as `stream_factory_create_stream()` and the registered I/O adapter for
  the delegated call.

# Success

- Returns `STREAM_STATUS_OK`.
- Stores a valid newly created stream handle in `*out`.

# Failure

- Returns the status reported by `stream_factory_create_stream()`.
- Leaves `*out` unchanged if the delegated creation operation leaves it
  unchanged.

# Ownership

- On success, ownership of the newly created stream handle is transferred to the
  caller.
- The produced stream handle must later be destroyed via `stream_destroy()`.
- On failure, no stream ownership is transferred.

# Notes

- This function delegates stream creation to `stream_factory_create_stream()`.
- The creator supplies its configured factory and adapter key.
- `kind` is packed into a `stream_io_creator_args_t` object and forwarded as
  adapter-specific creation arguments.
