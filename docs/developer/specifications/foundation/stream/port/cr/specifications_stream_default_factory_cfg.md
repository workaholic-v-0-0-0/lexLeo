@page specifications_stream_default_factory_cfg stream_default_factory_cfg() specifications

# Signature

~~~c
stream_factory_cfg_t stream_default_factory_cfg(void);
~~~

# Purpose

Return a default configuration for a stream factory.

# Success

- Returns a well-formed default `stream_factory_cfg_t`.
- `ret.fact_cap == STREAM_FACTORY_DEFAULT_CAPACITY`.

# Failure

- None.

# Notes

- This helper does not allocate.
