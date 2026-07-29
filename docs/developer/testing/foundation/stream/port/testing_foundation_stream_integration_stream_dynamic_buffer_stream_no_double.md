@page testing_foundation_stream_integration_stream_dynamic_buffer_stream_no_double stream / dynamic_buffer_stream integration tests without test doubles

---

@anchor testing_foundation_stream_integration_stream_dynamic_buffer_stream_no_double_write_read
# stream / dynamic_buffer_stream write-read integration

See:
- @ref specifications_stream
- @ref specifications_dynamic_buffer_stream

## Functions under test

~~~c
stream_status_t stream_write(
    stream_t *stream,
    const void *buf,
    size_t n,
    stream_status_t *st
);

size_t stream_read(
    stream_t *stream,
    void *buf,
    size_t n,
    stream_status_t *st
);
~~~

The test also exercises the construction and destruction paths required to
obtain and release a `stream_t` backed by a `dynamic_buffer_stream`.

## Test doubles

None.

The test uses the default OSAL memory operations and the real `stream` and
`dynamic_buffer_stream` implementations.

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| a `stream_t` backed by a `dynamic_buffer_stream` is created directly through the Composition Root, a null-terminated message is written through `stream_write()`, and the same number of bytes is read through `stream_read()` | stream and backend construction succeed, `stream_write()` returns the number of bytes written with `STREAM_STATUS_OK`, `stream_read()` returns the same number of bytes with `STREAM_STATUS_OK`, and the bytes read are identical to the bytes written |
| a `dynamic_buffer_stream` adapter provider is registered in a `stream_factory_t`, a `stream_dynamic_buffer_creator_t` creates the `stream_t`, a null-terminated message is written through `stream_write()`, and the same number of bytes is read through `stream_read()` | factory, provider, creator, and stream construction succeed, `stream_write()` returns the number of bytes written with `STREAM_STATUS_OK`, `stream_read()` returns the same number of bytes with `STREAM_STATUS_OK`, the bytes read are identical to the bytes written, and the stream, creator, and factory can then be destroyed through their normal lifecycle |
