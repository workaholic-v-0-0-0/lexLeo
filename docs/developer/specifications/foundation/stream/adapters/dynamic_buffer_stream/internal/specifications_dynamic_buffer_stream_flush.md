@page specifications_dynamic_buffer_stream_flush dynamic_buffer_stream_flush() specifications

# Signature

```c
static stream_status_t dynamic_buffer_stream_flush(void *backend)
```

# Purpose

Implement the `stream` port flush callback for the `dynamic_buffer_stream`
adapter.

This callback reports flush success for the adapter-managed in-memory dynamic
buffer.

# Relationship to the public port contract

This function is a private backend callback bound into the
`dynamic_buffer_stream` vtable.
It implements the public behavior exposed through `stream_flush()` for stream
instances created by the `dynamic_buffer_stream` adapter.

See:
- @ref specifications_stream_flush

# Preconditions

- None.

# Invalid arguments

- None.

# Success

- Returns `STREAM_STATUS_OK`.

# Failure

- None.

# Ownership

- `backend` is borrowed.
- This function does not take ownership of `backend`.

# Notes

- This callback does not validate `backend`.
- This callback does not modify the adapter-managed dynamic buffer.
- This callback performs no I/O and no synchronization with any external
  resource.
- For this adapter, flushing is a no-op that always succeeds.
