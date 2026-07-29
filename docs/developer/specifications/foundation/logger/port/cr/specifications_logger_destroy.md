@page specifications_logger_destroy logger_destroy() specifications

# Signature

~~~c
logger_status_t logger_destroy(logger_t **logger);
~~~

# Purpose

Destroy a previously created public `logger_t` handle of the `logger` port.

This function attempts backend cleanup through the logger vtable `destroy`
callback when a backend is bound, then releases the public `logger_t` handle
regardless of the backend cleanup status.

# Success

- If `logger == NULL`, returns `LOGGER_STATUS_OK`.
- If `logger != NULL` and `*logger == NULL`, returns `LOGGER_STATUS_OK`.
- Otherwise:
    - if a backend is bound, delegates backend cleanup to the logger vtable
      `destroy` callback;
    - releases the public `logger_t` handle;
    - stores `NULL` in `*logger`;
    - returns the backend destroy status, or `LOGGER_STATUS_OK` if no backend was
      bound.

# Failure

- If the backend `destroy` callback reports a failure status:
    - releases the public `logger_t` handle;
    - stores `NULL` in `*logger`;
    - returns that status.

# Notes

- `logger_destroy()` reports backend cleanup status.
- A backend destroy failure does not preserve the public handle.
- Passing `NULL` or a pointer to `NULL` is treated as a successful no-op.
