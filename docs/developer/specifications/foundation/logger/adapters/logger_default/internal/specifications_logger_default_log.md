@page specifications_logger_default_log logger_default_log() specifications

# Signature

    static logger_status_t logger_default_log(void *backend, const char *message);

# Purpose

Write one log record through the `logger_default` backend.

# Preconditions

- `backend` must designate a valid `logger_default_t` backend object.
- The designated backend must have been initialized with:
    - a valid target `stream`,
    - a valid `time_ops` table.
- These preconditions are enforced as internal invariants, not as recoverable
  public argument validation.

# Invalid arguments

- `message == NULL`
    - returns `LOGGER_STATUS_INVALID`.

# Success

- Returns `LOGGER_STATUS_OK`.
- Writes a UTC+0 timestamp prefix to the configured target stream.
- The formatted timestamp prefix has the form:
  `[YYYY-MM-DD HH:MM:SS UTC+0] `
- For example, epoch time `0` is formatted as:
  `[1970-01-01 00:00:00 UTC+0] `
- Writes the full contents of `message` immediately after that trailing space.
- If `append_newline == true`, writes one trailing newline character after the
  message.
- If the injected time service cannot provide the current time, a fallback
  timestamp-error prefix is written instead of a formatted UTC+0 timestamp.
- The fallback prefix is:
  `[timestamp error] `
- This fallback prefix also ends with a trailing space character before the
  message payload.

# Failure

- Returns `LOGGER_STATUS_IO_ERROR` if:
    - writing the timestamp prefix fails,
    - writing the fallback timestamp-error prefix fails,
    - writing `message` fails,
    - writing the optional trailing newline fails,
    - or any of these writes is partial.
- Returns any non-`LOGGER_STATUS_OK` status propagated by the private timestamp
  writing helper, if that helper fails before message emission can complete.

# Ownership

- Does not take ownership of `backend`.
- Does not modify or retain ownership of `message`.

# Notes

- `message` is treated as a NUL-terminated string.
- `strlen(message)` determines the number of bytes written for the message
  payload.
- The function validates `message` explicitly, but treats `backend` and backend
  internals as construction-time invariants.