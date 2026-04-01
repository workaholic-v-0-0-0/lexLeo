@page specifications_dynamic_buffer_stream_default_cfg dynamic_buffer_stream_default_cfg() specifications

# Signature

```c
dynamic_buffer_stream_cfg_t dynamic_buffer_stream_default_cfg(void);
```

# Purpose

Return a default configuration for the `dynamic_buffer_stream` adapter.

# Preconditions

- None.

# Invalid arguments

- None.

# Success

- Returns a well-formed default `dynamic_buffer_stream_cfg_t`.
- Returns a value-initialized baseline configuration.
- `ret.default_cap` is initialized to a valid non-zero capacity suitable for
  immediate use by the adapter.

# Failure

- None.

# Notes

- `DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY` defines the initial capacity of the
  underlying dynamic buffer.
- This helper provides a baseline configuration that the Composition Root may
  override.
- This helper does not allocate.

