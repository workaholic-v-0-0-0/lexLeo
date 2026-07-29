@page specifications_logger_default_default_cfg logger_default_default_cfg() specifications

# Signature

```c
logger_default_cfg_t logger_default_default_cfg(void);
```

# Purpose

Return a default configuration for the `logger_default` adapter.

# Preconditions

- None.

# Invalid arguments

- None.

# Success

- Returns a well-formed default `logger_default_cfg_t`.
- Returns a value-initialized baseline configuration.
- `ret.append_newline == true`.

# Failure

- None.
