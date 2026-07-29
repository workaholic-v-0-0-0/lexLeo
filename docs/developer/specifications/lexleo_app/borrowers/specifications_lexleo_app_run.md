@page specifications_lexleo_app_run lexleo_app_run() specifications

# Signature

```c
lexleo_app_status_t lexleo_app_run(lexleo_app_t *app);
```

# Purpose

Run the application VM and log application startup and shutdown.

# Preconditions

- `app` must point to a valid, fully initialized application handle.

# Success

- Returns `LEXLEO_APP_STATUS_OK`.
- Logs `"LexLeo application started."` through the application logger.
- Calls `lexleo_vm_run()` with the application VM.
- Logs `"LexLeo application shutting down."` after successful VM execution.

# Failure

- Returns `LEXLEO_APP_STATUS_LOGGER_ERROR` if either logging call fails.
- Returns `LEXLEO_APP_STATUS_VM_ERROR` if `lexleo_vm_run()` fails.
- Stops at the first failure:
    - If startup logging fails, the VM is not run.
    - If VM execution fails, shutdown logging is not attempted.
- Does not destroy the application handle or its owned resources.

# Ownership

- Ownership of the application handle is not transferred.
- The caller remains responsible for destroying it through
  `lexleo_app_destroy()`, whether execution succeeds or fails.

# Notes

- Logging messages describe the application lifecycle.
- VM execution behavior is defined by `lexleo_vm_run()`.
- The shutdown message announces shutdown; it does not release resources.
