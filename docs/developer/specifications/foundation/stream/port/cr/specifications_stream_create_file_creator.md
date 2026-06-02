@page specifications_stream_create_file_creator stream_create_file_creator() specifications

# Signature

~~~c
stream_status_t stream_create_file_creator(
    stream_file_creator_t **out,
    stream_factory_t *factory,
    stream_key_t key,
    const osal_mem_ops_t *mem);
~~~

# Purpose

Create a file stream creator bound to a `stream_factory_t` and adapter key.

# Preconditions

- `out` must not be `NULL`.
- `factory` must not be `NULL`.
- `key` must not be `NULL`.
- `key` must not be empty.
- `mem` must not be `NULL`.
- `mem->calloc` must not be `NULL`.

# Success

- Returns `STREAM_STATUS_OK`.
- Stores a valid newly created `stream_file_creator_t` handle in `*out`.

# Failure

- Returns `STREAM_STATUS_OOM` if allocation fails.

# Ownership

- Ownership of `factory`, `key`, and `mem` is not transferred.
- On success, ownership of the produced `stream_file_creator_t` handle is
  transferred to the caller.
- The produced handle must later be released via
  `stream_destroy_file_creator()`.
