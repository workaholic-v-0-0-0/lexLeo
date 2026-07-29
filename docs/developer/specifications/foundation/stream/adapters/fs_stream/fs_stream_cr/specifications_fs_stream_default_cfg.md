@page specifications_fs_stream_default_cfg fs_stream_default_cfg() specifications

# Signature

```c
fs_stream_cfg_t fs_stream_default_cfg(void);
```

# Purpose

Return a default configuration for the `fs_stream` adapter.

# Preconditions

- None.

# Invalid arguments

- None.

# Success

- Returns a well-formed default `fs_stream_cfg_t`.
- Returns a value-initialized baseline configuration.
- `ret.reserved == 0`.

# Failure

- None.
