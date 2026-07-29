@page specifications_stream_dynamic_buffer_creator_create stream_dynamic_buffer_creator_create() specifications

# Signature

~~~c
stream_status_t stream_dynamic_buffer_creator_create(
    const stream_dynamic_buffer_creator_t *creator,
    stream_t **out);
~~~

# Purpose

Create a dynamic-buffer-backed stream through a dynamic buffer stream creator.

# Preconditions

- `creator` must point to a valid `stream_dynamic_buffer_creator_t` created by
  `stream_create_dynamic_buffer_creator()`.
- `out` must not be `NULL`.

# Success

- Returns `STREAM_STATUS_OK`.
- Stores in `*out` a valid newly created stream handle whose backend is a
  dynamic buffer stream created through the adapter associated with the
  creator.

# Failure

- Returns `STREAM_STATUS_NO_BACKEND` if no backend provider corresponding to
  the creator is available.
- Returns `STREAM_STATUS_OOM` if memory allocation fails.
- Leaves `*out` unchanged.

# Ownership

- On success, ownership of the newly created stream handle is transferred to
  the caller.
- The produced stream handle must later be destroyed via `stream_destroy()`.
- On failure, no stream ownership is transferred.
