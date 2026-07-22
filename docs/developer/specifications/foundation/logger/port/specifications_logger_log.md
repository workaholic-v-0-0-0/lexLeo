@page specifications_logger_log logger_log() specifications

# Signature

```c
logger_status_t logger_log(logger_t *l, const char *message);
```

# Purpose

Emit the message `message` through the `logger` port.

# Preconditions

- If `l != NULL`, `l` must point to a fully initialized `logger_t` handle.
- The handle must contain a valid backend compatible with its validated virtual
  table.

# Invalid arguments

- `l` must not be `NULL`.
- `message` must not be `NULL`.

# Success

- Delegates the log operation to the adapter-facing `log` callback stored in
  the logger handle.
- Returns the value produced by the underlying `log` callback.

# Failure

- If `l == NULL`, returns `LOGGER_STATUS_INVALID`.
- If `message == NULL`, returns `LOGGER_STATUS_INVALID`.

# Notes

- A handle created by `logger_create()` must first be completed through
  `logger_complete_default_init()`.
