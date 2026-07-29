@page testing_foundation_logger_borrower_unit logger_borrower.c unit tests

---

@anchor testing_foundation_logger_unit_logger_log

# `logger_log()` unit tests

See:

- @ref specifications_logger_log "logger_log() specifications"
- @ref specifications_logger_create "logger_create() specifications"
- @ref specifications_logger_complete_default_init "logger_complete_default_init() specifications"
- @ref specifications_logger_destroy "logger_destroy() specifications"

## Function under test

```c
logger_status_t logger_log(
    logger_t *logger,
    const char *message);
```

## Test doubles

- fake logger adapter backend

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `logger == NULL` | `logger_log()` returns `LOGGER_STATUS_INVALID` and does not invoke the backend `log` operation |
| `message == NULL` | `logger_log()` returns `LOGGER_STATUS_INVALID` and does not invoke the backend `log` operation |
| the backend `log` operation returns `LOGGER_STATUS_IO_ERROR` | `logger_log()` invokes the backend `log` operation exactly once, passes the expected backend and message pointers unchanged, and returns `LOGGER_STATUS_IO_ERROR` |
| the backend `log` operation returns `LOGGER_STATUS_OK` | `logger_log()` invokes the backend `log` operation exactly once, passes the expected backend and message pointers unchanged, and returns `LOGGER_STATUS_OK` |
