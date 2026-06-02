@page specifications_stream_buffer_creator_create stream_buffer_creator_create() specifications

# Signature

~~~c
stream_status_t stream_buffer_creator_create(
    const stream_buffer_creator_t *creator,
    stream_t **out);
~~~

# Purpose

Create a buffer-backed stream through a buffer stream creator.

# Preconditions

- `creator` must point to a valid `stream_buffer_creator_t`.

# Invalid arguments

- Same as `stream_factory_create_stream()` for the delegated call.

# Success

- Returns `STREAM_STATUS_OK`.
- Stores a valid newly created stream handle in `*out`.

# Failure

- Returns the status reported by `stream_factory_create_stream()`.
- Leaves `*out` unchanged if the delegated factory creation operation leaves it
  unchanged.

# Ownership

- On success, ownership of the newly created stream handle is transferred to the
  caller.
- The produced stream handle must later be destroyed via `stream_destroy()`.
- On failure, no stream ownership is transferred.

# Notes

- This function delegates stream creation to `stream_factory_create_stream()`.
- The creator supplies its configured factory and adapter key.
- Buffer stream creation uses `NULL` creation arguments.