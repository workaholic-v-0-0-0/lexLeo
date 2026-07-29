@page specifications_logger_log logger_log() specifications

# Signature

```c
logger_status_t logger_log(logger_t *logger, const char *message);
```

# Purpose

Emit the message `message` through the `logger` port.

# Preconditions

- If `logger != NULL`, `logger` must point to a fully initialized `logger_t`
  handle.
- The handle must contain a valid backend compatible with its validated virtual
  table.

# Invalid arguments

- `logger` must not be `NULL`.
- `message` must not be `NULL`.

# Success

- Delegates the log operation to the adapter-facing `log` callback stored in
  the logger handle.
- The backend log operation returns `LOGGER_STATUS_OK`:
- returns `LOGGER_STATUS_OK`.

# Failure

- If `logger == NULL`, returns `LOGGER_STATUS_INVALID`.
- If `message == NULL`, returns `LOGGER_STATUS_INVALID`.
- Otherwise, if the backend log operation produces a status other than
  `LOGGER_STATUS_OK`, returns the value produced by the backend log operation.

# Notes

- A handle created by `logger_create()` must first be completed through
  `logger_complete_default_init()`.
