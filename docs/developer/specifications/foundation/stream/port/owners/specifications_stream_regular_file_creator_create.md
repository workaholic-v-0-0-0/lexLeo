@page specifications_stream_regular_file_creator_create stream_file_creator_create() specifications

# Signature

~~~c
stream_status_t stream_file_creator_create(
    const stream_regular_file_creator_t *creator,
    const char *path,
    const char *mode,
    stream_t **out);
~~~

# Purpose

Create a file-backed stream through a file stream creator.

# Preconditions

- `creator` must point to a valid `stream_regular_file_creator_t` created by
  `stream_create_regular_file_creator()`.
- `mode` must equal one of the following strings: `"rb"`, `"wb"`, or `"ab"`.
- `out` must not be `NULL`.

# Invalid arguments

- `path` must not be `NULL`.
- `path` must not be an empty string.
- The length of `path` must be smaller than `255`.
- `path` must be a valid pathname on the target operating system.

# Success

- Returns `STREAM_STATUS_OK`.
- Stores in `*out` a valid newly created stream handle whose backend is bound
  to the regular file designated by `path` and opened according to `mode`.

# Failure

- Returns `STREAM_STATUS_INVALID` if an invalid argument is supplied.
- Returns `STREAM_STATUS_NO_BACKEND` if no backend provider corresponding to
  the creator is available.
- Returns `STREAM_STATUS_NOT_FOUND` if the requested resource cannot be found.
- Returns `STREAM_STATUS_IO_ERROR` if the requested resource cannot be opened
  because of an input/output error.
- Returns `STREAM_STATUS_OOM` if memory allocation fails.
- Leaves `*out` unchanged.

# Ownership

- On success, ownership of the newly created stream handle is transferred to
  the caller.
- The produced stream handle must later be destroyed via `stream_destroy()`.
- On failure, no stream ownership is transferred.
