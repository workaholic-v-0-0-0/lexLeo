@page specifications_cli_resolve_log_path cli_resolve_log_path() specifications

# Signature

```c
bool cli_resolve_log_path(char *out, size_t out_size);
```

# Purpose

Resolve the effective log-file path for the `lexleo` command-line interface.

# Invalid arguments

- `out` must not be `NULL`.
- `out_size` must not be `0`.

# Success

- Returns `true`.
- Stores a valid null-terminated log-file path in `out`.
- Uses the configured `log_path` value when the application configuration
  provides a valid non-empty one.
- Falls back to the platform-default log-file path when no valid configured
  path is available.
- Ensures that the parent directory of the resulting log-file path exists.

# Failure

- Returns `false` for invalid arguments.
- Returns `false` if the platform-default configuration path cannot be
  obtained.
- Returns `false` if no valid configured log-file path is available and the
  platform-default log-file path cannot be resolved.
- Returns `false` if the parent directory of the resulting log-file path
  cannot be ensured.
- Leaves `out` as an empty string if `out` is valid.

# Notes

- `cli_resolve_log_path()` implements the common log-path resolution policy of
  the CLI module.
- The configuration file is consulted first through the internal
  configuration-reading logic.
- Platform-specific defaults are obtained through
  `cli_platform_default_config_path()` and
  `cli_platform_resolve_default_log_path()`.
- Parent-directory preparation is delegated to
  `cli_platform_ensure_log_parent_dir_exists()`.
