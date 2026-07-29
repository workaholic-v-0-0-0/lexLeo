@page testing_foundation_stream_integration_stream_fs_stream_no_double stream / fs_stream integration tests without test doubles

---

@anchor testing_foundation_stream_integration_stream_fs_stream_no_double

# `stream` / `fs_stream` integration tests without test doubles

See:

- @ref specifications_stream_create "stream_create() specifications"
- @ref specifications_stream_complete_default_init "stream_complete_default_init() specifications"
- @ref specifications_stream_create_factory "stream_create_factory() specifications"
- @ref specifications_stream_factory_add_adapter "stream_factory_add_adapter() specifications"
- @ref specifications_stream_create_regular_file_creator "stream_create_regular_file_creator() specifications"
- @ref specifications_stream_regular_file_creator_create "stream_regular_file_creator_create() specifications"
- @ref specifications_fs_stream_create "fs_stream_create() specifications"
- @ref specifications_fs_stream_complete_default_init "fs_stream_complete_default_init() specifications"
- @ref specifications_fs_stream_create_adapter_provider "fs_stream_create_adapter_provider() specifications"
- @ref specifications_stream_write "stream_write() specifications"
- @ref specifications_stream_flush "stream_flush() specifications"
- @ref specifications_stream_read "stream_read() specifications"
- @ref specifications_stream_destroy "stream_destroy() specifications"

## Test doubles

None.

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| a writable stream is constructed through a stream factory and regular-file creator using the registered `fs_stream` adapter provider | creation succeeds; writing and flushing a message to the real file succeed; destroying the output stream closes it; reopening the same file through the same creator in read mode succeeds; reading returns the exact previously written message; all owned stream, creator, factory, adapter-provider, backend, and file resources are released correctly |
| a writable stream is constructed directly through the stream and `fs_stream` Composition Root APIs | `stream_create()`, `fs_stream_create()`, `fs_stream_complete_default_init()`, and `stream_complete_default_init()` succeed; writing and flushing a message to the real file succeed; destroying the output stream closes it; reconstructing the stream and `fs_stream` backend in read mode succeeds; reading returns the exact previously written message; all owned stream, backend, and file resources are released correctly |
