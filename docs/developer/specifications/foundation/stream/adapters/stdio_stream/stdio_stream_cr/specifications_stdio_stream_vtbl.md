@page specifications_stdio_stream_vtbl stdio_stream_vtbl() specifications

# Signature

~~~c
const stream_vtbl_t *stdio_stream_vtbl(void);
~~~

# Purpose

Return the `stream` port dispatch table implemented by the `stdio_stream`
adapter.

# Preconditions

- None.

# Invalid arguments

- None.

# Success

- Returns a non-`NULL` pointer to a valid `stream_vtbl_t`.
- The returned dispatch table contains the `stdio_stream` adapter
  implementations of the `stream` read, write, flush, and close operations.
- All mandatory operation pointers in the returned dispatch table are
  non-`NULL`.
- Repeated calls return a dispatch table with the same operation bindings.

# Failure

- None.

# Ownership

- Ownership of the returned `stream_vtbl_t` is not transferred to the caller.
- The caller must not modify or release the returned dispatch table.

# Notes

- The returned dispatch table is intended to bind a `stdio_stream_t` backend
  to the `stream` port.
- The operation implementations referenced by the returned table are internal
  to the `stdio_stream` adapter.
- The lifetime of the returned dispatch table is independent of individual
  `stdio_stream_t` backend instances.
