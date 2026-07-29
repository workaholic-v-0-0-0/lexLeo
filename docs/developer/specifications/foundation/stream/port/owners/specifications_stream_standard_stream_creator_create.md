@page specifications_stream_standard_stream_creator_create stream_standard_stream_creator_create() specifications

# Signature

~~~c
stream_status_t stream_standard_stream_creator_create(
    const stream_standard_stream_creator_t *creator,
    stream_standard_stream_kind_t kind,
    stream_t **out);
~~~

# Purpose

Create a standard-stream-backed stream through a standard stream creator.

# Preconditions

- `creator` must point to a valid `stream_standard_stream_creator_t` created by
  `stream_create_standard_stream_creator()`.
- `kind` must equal one of the following values:
  `STREAM_STANDARD_STREAM_KIND_STDIN`, `STREAM_STANDARD_STREAM_KIND_STDOUT`, or
  `STREAM_STANDARD_STREAM_KIND_STDERR`.
- `out` must not be `NULL`.

# Success

- Returns `STREAM_STATUS_OK`.
- Stores in `*out` a valid newly created stream handle whose backend is bound
  to the standard stream designated by `kind`.

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
