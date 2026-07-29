@page testing_foundation_stream_regular_file_creator_owner_unit stream_regular_file_creator_owner.c unit tests

---

@anchor testing_foundation_stream_regular_file_creator_create

# `stream_regular_file_creator_create()` unit tests

See:

- @ref specifications_stream_regular_file_creator_create "stream_regular_file_creator_create() specifications"
- @ref specifications_stream_destroy "stream_destroy() specifications"
- @ref specifications_stream_create_regular_file_creator "stream_create_regular_file_creator() specifications"
- @ref specifications_stream_destroy_regular_file_creator "stream_destroy_regular_file_creator() specifications"

## Function under test

```c
stream_status_t stream_regular_file_creator_create(
    const stream_regular_file_creator_t *creator,
    const char *path,
    const char *mode,
    stream_t **out);
```

## Test doubles

- fake adapter
- fake memory operations

## Tested scenarios

| WHEN                                                            | EXPECT                                                                                                                                                                                                     |
| --------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `path == NULL`                                                  | `stream_regular_file_creator_create()` returns `STREAM_STATUS_INVALID` and leaves `*out` unchanged                                                                                                        |
| `path` is an empty string                                       | `stream_regular_file_creator_create()` returns `STREAM_STATUS_INVALID` and leaves `*out` unchanged                                                                                                        |
| the length of `path` exceeds the supported maximum              | `stream_regular_file_creator_create()` returns `STREAM_STATUS_INVALID` and leaves `*out` unchanged                                                                                                        |
| stream creation fails because of an out-of-memory condition     | `stream_regular_file_creator_create()` returns `STREAM_STATUS_OOM`, leaves `*out` unchanged, and causes no memory leak, invalid free, or double free                                                    |
| creation succeeds                                               | `stream_regular_file_creator_create()` returns `STREAM_STATUS_OK`, stores a valid newly created `stream_t` handle in `*out`; `stream_destroy()` sets `*out` to `NULL`; the complete lifecycle causes no memory leak, invalid free, or double free |
