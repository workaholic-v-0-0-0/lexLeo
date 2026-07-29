@page testing_foundation_logger_integration_logger_logger_default_stream_osal_time_doubled logger / logger_default integration tests with doubled stream and doubled osal_time dependencies

---

@anchor testing_foundation_logger_integration_logger_logger_default_stream_osal_time_doubled_smoke

# `logger` / `logger_default` smoke tests

See:

- @ref specifications_logger "logger specifications"
- @ref specifications_logger_default "logger_default specifications"

## Test doubles

- fake stream dependency
- fake OSAL time dependency

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| a public `logger_t` handle and a `logger_default` backend are created, initialized, bound together, used to log a message at the Unix epoch, and then destroyed | creation and initialization succeed, `logger_log()` returns `LOGGER_STATUS_OK`, the expected timestamped message is written and flushed to the fake stream sink, `logger_destroy()` returns `LOGGER_STATUS_OK`, and destroying the logger does not close the borrowed stream |

---

@anchor testing_foundation_logger_integration_logger_logger_default_stream_osal_time_doubled_log

# `logger_log()` / `logger_default` integration tests

See:

- @ref specifications_logger_log "logger_log() specifications"
- @ref specifications_logger_default_log "logger_default_log() specifications"

## Function under test

~~~c
logger_status_t logger_log(
    logger_t *logger,
    const char *message);
~~~

## Test doubles

- fake stream dependency
- fake OSAL time dependency

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `logger == NULL` | `logger_log()` returns `LOGGER_STATUS_INVALID` and leaves the fake stream sink content unchanged |
| `message == NULL` | `logger_log()` returns `LOGGER_STATUS_INVALID` and leaves the fake stream sink content unchanged |
| the target stream reports `STREAM_STATUS_IO_ERROR` during a write operation | `logger_log()` returns `LOGGER_STATUS_IO_ERROR` and leaves the fake stream sink content unchanged |
| the target stream reports `STREAM_STATUS_IO_ERROR` during the flush operation | `logger_log()` returns `LOGGER_STATUS_IO_ERROR` and leaves the fake stream sink content unchanged |
| a valid message is logged successfully at the Unix epoch | `logger_log()` returns `LOGGER_STATUS_OK` and appends `[1970-01-01 00:00:00 UTC+0] It is the Unix epoch now!\n` to the existing fake stream sink content |
| after a successful first log, a second valid message is logged one minute after the Unix epoch | the second `logger_log()` call returns `LOGGER_STATUS_OK` and appends `[1970-01-01 00:01:00 UTC+0] One minute after the Unix epoch!\n` after the content produced by the first log |
