@page specifications_logger_complete_default_init logger_complete_default_init() specifications

# Signature

```c
logger_status_t logger_complete_default_init(
    logger_t *logger,
    void *backend);
```

# Purpose

Complete the normal initialization lifecycle of a `logger_t` handle by
ensuring that a backend is attached to it.

# Preconditions

* `logger` must point to a valid, partially initialized `logger_t` handle
  created by `logger_create()`.
* If `logger->backend == NULL`, `backend` must point to a valid backend
  instance compatible with `logger->vtbl`.
* If `logger->backend != NULL`, `backend` must be `NULL`.

# Success

* Returns `LOGGER_STATUS_OK`.
* If `logger->backend == NULL`, attaches `backend` to the logger handle and
  transfers ownership of the backend to the handle.
* Otherwise leaves the logger handle unchanged.
* On return, the logger handle is fully initialized.

# Failure

* None.

# Ownership

* When `backend` is not `NULL`, ownership of the backend is transferred to the
  logger handle.
* When `backend` is `NULL`, no ownership is transferred.
* The attached backend is later released by `logger_destroy()` through
  `logger_vtbl_t::destroy()`.

# Notes

* Passing `backend == NULL` explicitly indicates that the backend has already
  been injected, for example by white-box test helpers.
* This function completes the standard initialization path of a `logger_t`
  handle.
