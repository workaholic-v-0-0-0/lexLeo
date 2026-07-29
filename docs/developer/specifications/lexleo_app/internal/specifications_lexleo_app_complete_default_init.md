@page specifications_lexleo_app_complete_default_init lexleo_app_complete_default_init() specifications

# Signature

```c
lexleo_app_cr_status_t lexleo_app_complete_default_init(
    lexleo_app_t *app,
    const lexleo_app_cfg_t *cfg
);
```

# Purpose

Complete application initialization using `cfg` and the borrowed runtime
dependencies stored in the handle.

# Preconditions

- `app` must point to a valid handle created by `lexleo_app_create()`.
- Previously initialized or injected resources must be valid and compatible
  with the application.
- The borrowed dependencies stored in `app` must be supported and well-formed.
- `cfg != NULL`.
- `cfg` must be supported:
    - For each of `cfg->in`, `cfg->out`, and `cfg->err`:
        - `kind` must be `LEXLEO_APP_IO_STDIO`, `LEXLEO_APP_IO_FILE`,
          or `LEXLEO_APP_IO_BUFFER`.
        - For `LEXLEO_APP_IO_STDIO` and `LEXLEO_APP_IO_BUFFER`,
          `path` and `mode` must be `NULL`.
        - For `LEXLEO_APP_IO_FILE`, `path` must point to a nonempty,
          null-terminated file path and `mode` must be `"rb"`, `"wb"`,
          or `"ab"`.

# Success

- Returns `LEXLEO_APP_CR_STATUS_OK`.
- Leaves `app` fully initialized.
- Preserves previously initialized or injected fields.
- Creates missing input, output, and error streams according to `cfg`.
- For standard streams, binds input to stdin, output to stdout,
  and error to stderr.
- Resolves the log path if it is empty.
- Creates a missing logger stream using the resolved or injected log path
  and the `"ab"` opening mode.
- Creates a missing logger borrowing the logger stream.
- Creates a missing VM borrowing the application streams, logger,
  and runtime operations.

# Failure

- Returns:
    - `LEXLEO_APP_CR_STATUS_INPUT_INIT_ERROR` if input stream initialization fails.
    - `LEXLEO_APP_CR_STATUS_OUTPUT_INIT_ERROR` if output stream initialization fails.
    - `LEXLEO_APP_CR_STATUS_ERR_INIT_ERROR` if error stream initialization fails.
    - `LEXLEO_APP_CR_STATUS_LOG_PATH_RESOLUTION_ERROR` if log-path resolution fails.
    - `LEXLEO_APP_CR_STATUS_LOGGER_STREAM_INIT_ERROR` if logger stream initialization fails.
    - `LEXLEO_APP_CR_STATUS_LOGGER_INIT_ERROR` if logger initialization fails.
    - `LEXLEO_APP_CR_STATUS_VM_INIT_ERROR` if VM initialization fails.
- Stops initialization at the first failure.
- Does not destroy the application handle.
- Resources successfully attached before the failure remain attached.
- Leaves the handle valid for destruction by `lexleo_app_destroy()`.

# Ownership

- Ownership of the application handle is not transferred.
- Runtime resources created by this function are owned by the application.
- Previously injected resources remain owned by the application.
- The runtime operations stored in the handle remain borrowed.
- Ownership of `cfg` is not transferred.

# Notes

- This function does not replace existing resources to apply a different
  configuration.
- It is safe to destroy the application after a failed call.
