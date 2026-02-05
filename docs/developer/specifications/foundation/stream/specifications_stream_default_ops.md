@page specifications_stream_default_ops stream_default_ops() specifications

# Signature

```c
const stream_ops_t *stream_default_ops(void);
```

# Purpose

Return the default borrower-facing operations table for the `stream` port.

# Success

- Returns a non-`NULL` pointer `ret` to a well-formed `stream_ops_t`.
- `ret->read`, `ret->write`, and `ret->flush` are non-`NULL`.

# Failure

- None.

# Notes

- This function exposes the default borrower-facing operations of the
  `stream` port.
- This function establishes the “non-`NULL` ops pointer implies well-formed”
  invariant for the stream port.