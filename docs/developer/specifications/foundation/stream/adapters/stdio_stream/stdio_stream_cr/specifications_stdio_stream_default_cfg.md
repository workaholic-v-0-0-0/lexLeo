@page specifications_stdio_stream_default_cfg stdio_stream_default_cfg() specifications

# Signature

~~~c
stdio_stream_cfg_t stdio_stream_default_cfg(void);
~~~

# Purpose

Return a default configuration for the `stdio_stream` adapter.

# Preconditions

- None.

# Invalid arguments

- None.

# Success

- Returns a well-formed default `stdio_stream_cfg_t`.
- Returns a value-initialized baseline configuration.
- `ret.reserved == 0`.

# Failure

- None.
