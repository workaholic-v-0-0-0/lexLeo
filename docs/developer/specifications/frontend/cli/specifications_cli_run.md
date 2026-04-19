@page specifications_cli_run cli_run() specifications

# Signature

```c
int cli_run(void);
```

# Purpose

Run the `lexleo` command-line interface.

# Success

- Returns `0`.
- Resolves the effective log-file path.
- Ensures that the parent directory of the target log file exists.
- Opens or creates the target log file in append mode.
- Writes a probe entry to the log file.
- Flushes and closes the log file.
- Reports the resolved log-file path on standard output.

# Failure

- Returns a non-zero value if the effective log-file path cannot be resolved.
- Returns a non-zero value if the target log file cannot be opened or created.
- Returns a non-zero value if the probe entry cannot be written.
- Returns a non-zero value if the target log file cannot be flushed.
- Returns a non-zero value if the target log file cannot be closed.

# Notes

- `cli_run()` is the public runtime entry point of the `lexleo` command-line
  interface module.
- The current implementation provides a minimal bootstrap behavior centered on
  log-path resolution and probe log creation.
- The effective log-file path is resolved through `cli_resolve_log_path()`.
