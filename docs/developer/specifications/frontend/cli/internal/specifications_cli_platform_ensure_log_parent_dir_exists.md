@page specifications_cli_platform_ensure_log_parent_dir_exists cli_platform_ensure_log_parent_dir_exists() specifications

# Signature

```c
bool cli_platform_ensure_log_parent_dir_exists(const char *log_path);
```

# Purpose

Ensure that the parent directory of the target CLI log file exists.

# Invalid arguments

- `log_path` must not be `NULL`.
- `log_path[0]` must not be `'\0'`.

# Success

- Returns `true`.
- Ensures that the parent directory of `log_path` exists when the function
  returns.
- May create one or more missing parent-directory components.
- Accepts already existing parent-directory components.

# Failure

- Returns `false` for invalid arguments.
- Returns `false` if `log_path` does not contain a valid parent-directory
  component for the active platform-specific path rules.
- Returns `false` if a required parent-directory component cannot be created.
- Returns `false` if the platform-specific path form is unsupported by the
  current implementation.

# Notes

- `cli_platform_ensure_log_parent_dir_exists()` is a platform-specific helper
  used by the common CLI log-path resolution logic.
- Existing directories are treated as acceptable outcomes and do not cause the
  operation to fail.
- The current Linux implementation expects POSIX-style paths.
- The current Win32 implementation supports drive-letter absolute paths and
  rejects unsupported forms such as relative paths or UNC paths.
