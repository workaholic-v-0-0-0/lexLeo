@page specifications_dynamic_buffer_stream_write dynamic_buffer_stream_write() specifications

# Signature

```c
static size_t dynamic_buffer_stream_write(
    void *backend,
    const void *buf,
    size_t n,
    stream_status_t *st)
```

# Purpose

Implement the `stream` port write callback for the `dynamic_buffer_stream`
adapter.

This callback appends bytes to the adapter-managed in-memory dynamic buffer.
When needed, it grows the underlying buffer before copying the requested data.

# Relationship to the public port contract

This function is a private backend callback bound into the
`dynamic_buffer_stream` vtable.
It implements the public behavior exposed through `stream_write()` for stream
instances created by the `dynamic_buffer_stream` adapter.

See:
- @ref specifications_stream_write

# Preconditions

- If `backend != NULL`, `backend` must designate a valid `dynamic_buffer_stream_t`.

# Invalid arguments

- `backend == NULL`
- `buf == NULL && n > 0`

# Success

If the arguments are valid and the write can be completed:
- Appends exactly `n` bytes from `buf` to the end of the
  internal dynamic buffer. Bytes are appended at offset `dbuf->len`
- `dbuf->len` is increased by `n`
- `dbuf->read_pos` is unchanged
- `dbuf->cap` may increase if buffer growth was required
- Returns `n`
- If `st != NULL`, `*st` is set to `STREAM_STATUS_OK`

# Failure

## Invalid arguments

- Returns `0`.
- If `st != NULL`, `*st` is set to `STREAM_STATUS_INVALID`.

## Invalid size growth request

If `n > SIZE_MAX - dbuf->len`:
- Returns `0`.
- If `st != NULL`, `*st` is set to `STREAM_STATUS_INVALID`.
- If `backend != NULL`, `backend` is left unchanged.

## Capacity growth failure before reserve

If the required capacity exceeds the representable growth policy, i.e. the
capacity-doubling loop cannot produce a strictly larger valid capacity:
- Returns `0`
- If `st != NULL`, `*st` is set to `STREAM_STATUS_OOM`
- `backend` is left unchanged.

## Reserve failure

If `dynamic_buffer_stream_buffer_reserve()` fails:
- the function returns `0`
- if `st != NULL`, `*st` is set to the status returned by
  `dynamic_buffer_stream_buffer_reserve()`
- `backend` is left unchanged.

# Ownership

- `backend` is borrowed
- `buf` is borrowed
- this function does not take ownership of either pointer

# Notes

- The growth policy is based on repeated capacity doubling.
- The actual reallocation is delegated to
  `dynamic_buffer_stream_buffer_reserve()`.
- This function does not flush or expose data externally; it only mutates the
  adapter-managed in-memory buffer.
- `STREAM_STATUS_OOM` may be returned either because allocation failed during
  reserve or because the growth policy could not represent a sufficient next
  capacity.
