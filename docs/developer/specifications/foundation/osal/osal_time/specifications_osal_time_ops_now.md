@page specifications_osal_time_ops_now osal_time_ops_t::now() specifications

# Signature

```c
osal_time_status_t (*now)(osal_time_t *out);
```

# Purpose

Retrieves the current system time as epoch seconds.

# Preconditions

- If `out != NULL`, `out` must point to a valid writable `osal_time_t`.

# Invalid arguments

- `out` must not be `NULL`.

# Success

- Returns `OSAL_TIME_STATUS_OK`.
- Stores the current time in `out->epoch_seconds`.
- The stored value represents seconds since the Unix epoch.

# Failure

- Returns `OSAL_TIME_STATUS_INVALID` for invalid arguments.
- Returns `OSAL_TIME_STATUS_ERROR` if the underlying system time query fails.

# Ownership

- No ownership is transferred.
- The caller retains ownership of `out`.
