@page testing_foundation_stream_factory_fs_stream_integration Stream factory / fs_stream integration tests

It covers:
- `stream_factory_create_stream()` with a registered `fs_stream` adapter

The `fs_stream` adapter is registered through the Composition Root API, and the
resulting stream is validated through the public borrower API.

@anchor testing_foundation_stream_integration_stream_factory_create_stream_fs_stream
# stream_factory_create_stream() / fs_stream integration

## Function under test

```c
stream_status_t stream_factory_create_stream(
    const stream_factory_t *f,
    stream_key_t key,
    const void *args,
    stream_t **out);
```

## Precondition

- Unless stated otherwise by the scenario, `f` designates a valid factory
  instance previously created by `stream_create_factory()`.
- Unless stated otherwise by the scenario, the factory contains a valid
  `fs_stream` adapter descriptor registered under the key `"fs"`.
- Unless stated otherwise by the scenario, `args` designates a valid
  `fs_stream_args_t` object prepared by the fixture.

## Invalid arguments

- `f`, `key`, `args`, and `out` must not be `NULL`.
- `key` must not be `NULL` and must not be an empty string.
- `args->path` must not be `NULL` and must not be an empty string.
- `args->flags` must not be zero.

## Success

- Returns `STREAM_STATUS_OK`.
- Stores a valid stream handle in `*out`.
- The produced stream is ready for normal runtime use.
- The produced stream must be destroyed via `stream_destroy()`.

## Failure

- Returns:
    - `STREAM_STATUS_INVALID` for invalid arguments
    - `STREAM_STATUS_NOT_FOUND` if `key` is not registered
    - `STREAM_STATUS_IO_ERROR` when OSAL file operations fail
- Leaves `*out` unchanged if `out` is not `NULL`.

## Test doubles

- `fake_file`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `stream_factory_create_stream(f, key, args, out)` is called with a valid factory, a registered `fs_stream` key, valid `fs_stream` arguments, and backend file open succeeds | returns `STREAM_STATUS_OK`;<br>stores a non-`NULL` stream handle in `*out`;<br>the produced stream is ready for normal runtime use |
| `args == NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `out == NULL` | returns `STREAM_STATUS_INVALID`;<br>no stream handle is produced |
| `args != NULL` but `args->path == NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `args != NULL` but `args->path` is an empty string | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `args != NULL` but `args->flags == 0` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `stream_factory_create_stream(f, key, args, out)` is called with a valid factory, a registered `fs_stream` key, valid `fs_stream` arguments, and backend file open fails | returns `STREAM_STATUS_IO_ERROR`;<br>leaves `*out` unchanged |
| `f == NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `key == NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `key` is an empty string | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `f` designates a valid factory instance but `key` is not registered in that factory | returns `STREAM_STATUS_NOT_FOUND`;<br>leaves `*out` unchanged |

## Notes

- The `fs_stream` adapter descriptor is created through `fs_stream_create_desc()`
  and registered into the factory before the call under test.
- Backend file-open failures are injected through `fake_file`.
- In the nominal success scenario, the produced stream is additionally validated
  through the public borrower API:
    - `stream_write()`
    - `stream_flush()`
- Output-handle preservation on failure is part of the tested behavioral
  contract.
