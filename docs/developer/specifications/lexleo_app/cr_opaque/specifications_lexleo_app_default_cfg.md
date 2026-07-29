@page specifications_lexleo_app_default_cfg lexleo_app_default_cfg() specifications

# Signature

```c
lexleo_app_cfg_t lexleo_app_default_cfg(void);
```

# Purpose

Return the default LexLeo application configuration.

# Preconditions

- None.

# Success

- Returns a fully initialized `lexleo_app_cfg_t` value.
- Sets `in.kind`, `out.kind`, and `err.kind` to `LEXLEO_APP_IO_STDIO`.
- Sets `in.path`, `out.path`, and `err.path` to `NULL`.
- Sets `in.mode`, `out.mode`, and `err.mode` to `NULL`.

# Ownership

- Returns the configuration by value.
- Does not allocate resources or transfer ownership.

# Notes

- The default configuration selects standard input, standard output,
  and standard error for the corresponding application streams.
