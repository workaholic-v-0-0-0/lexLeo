@page specifications_logger_log logger_log() specifications

# Signature

```c
logger_status_t logger_log(logger_t *l, const char *message);
```

# Purpose

Emit the message `message` through the `logger` port.

# Preconditions

- If `l != NULL`, `l` must point to a valid `logger_t` handle created by
  `logger_create()`. 

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

- `logger_log()` exposes the borrower-facing logging operation of the `logger` port.
- This function requires a logger handle whose adapter-facing virtual table has been validated at creation time by `logger_create()`.
