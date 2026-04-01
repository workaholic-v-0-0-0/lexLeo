@page specifications_dynamic_buffer_stream_read dynamic_buffer_stream_read() specifications

# Signature

```c
static size_t dynamic_buffer_stream_read(
    void *backend,
    void *buf,
    size_t n,
    stream_status_t *st)
```

# Purpose

Implement the `stream` port read callback for the `dynamic_buffer_stream`
adapter.

This callback reads bytes from the adapter-managed in-memory dynamic buffer,
starting at the current read cursor.

# Relationship to the public port contract

This function is a private backend callback bound into the
`dynamic_buffer_stream` vtable.
It implements the public behavior exposed through `stream_read()` for stream
instances created by the `dynamic_buffer_stream` adapter.

See:
- @ref specifications_stream_read

# Preconditions

- If `backend != NULL`, `backend` must designate a valid
  `dynamic_buffer_stream_t`.

# Invalid arguments

- `backend == NULL`
- `buf == NULL && n > 0`

# Success

- If `n == 0`:
    - Returns `0`.
    - `backend` is left unchanged.
    - If `st != NULL`, `*st` is set to `STREAM_STATUS_OK`.

- If `0 < n` and unread data is available:
    - Reads exactly `min(n, dbuf->len - dbuf->read_pos)` bytes from the internal
      buffer starting at offset `dbuf->read_pos`.
    - Copies the read bytes into `buf`.
    - Advances `dbuf->read_pos` by the number of bytes returned.
    - Leaves `dbuf->len` unchanged.
    - Returns the number of bytes read.
    - If `st != NULL`, `*st` is set to `STREAM_STATUS_OK`.

# Failure

## Invalid arguments

- Returns `0`.
- If `st != NULL`, `*st` is set to `STREAM_STATUS_INVALID`.

## End of buffer

If `dbuf->read_pos >= dbuf->len`:

- Returns `0`.
- Leaves `backend` unchanged.
- If `st != NULL`, `*st` is set to `STREAM_STATUS_EOF`.

# Ownership

- `backend` is borrowed.
- `buf` is borrowed.
- This function does not take ownership of either pointer.

# Notes

- This callback reads from the current read cursor only; it does not compact
  or shrink the underlying dynamic buffer.
- The amount returned is bounded by both the caller-requested size `n` and the
  number of unread bytes remaining in the buffer.
- `dbuf->len` represents the number of valid bytes currently stored in the
  buffer.
- `dbuf->read_pos` represents the current read cursor within those valid bytes.
