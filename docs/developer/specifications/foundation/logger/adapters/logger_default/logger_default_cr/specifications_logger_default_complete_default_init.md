@page specifications_logger_default_complete_default_init logger_default_complete_default_init() specifications

# Signature

```c
logger_default_status_t logger_default_complete_default_init(
    logger_default_t *logger_default,
    const logger_default_cfg_t *cfg);
```

# Purpose

Complete the initialization of a partially initialized `logger_default_t`
backend from its adapter configuration.

# Preconditions

- `logger_default` must point to a `logger_default_t` created by
  `logger_default_create()` whose external borrowed dependencies are correctly
  initialized.
- `cfg` must point to a valid `logger_default_cfg_t`.

# Invalid arguments

- None.

# Success

- Returns `LOGGER_DEFAULT_STATUS_OK`.
- Initializes the backend's configuration-dependent state from `cfg`.
- Sets `logger_default->append_newline` to `cfg->append_newline`.
- Leaves all external borrowed dependencies of `logger_default` unchanged.
- The resulting `logger_default_t` is fully initialized and can be used by the
  `logger_vtbl_t` operations fetchable by `logger_default_vtbl()`.

# Failure

- None.

# Ownership

- Ownership of `logger_default` and `cfg` is not transferred.
- The external borrowed dependencies stored in `logger_default` remain
  borrowed.
- No additional resource ownership is acquired by this operation.

# Notes

- This function completes the initialization started by
  `logger_default_create()`.
- `logger_default_create()` initializes the backend's external borrowed
  dependencies.
- `logger_default_complete_default_init()` initializes the remaining backend
  state from the adapter configuration.
- In the `logger_default` adapter, this completion-time state consists of the
  `append_newline` behavior flag.
- No owned resource is created or acquired during completion.
