@page testing_foundation_logger_integration_logger_default logger / logger_default integration tests

It covers:
- `logger_log()` with a `logger_default`-backed logger

The `logger_default` adapter is created through the Composition Root API, and the
resulting logger is validated through the public borrower API.

@anchor testing_foundation_logger_integration_logger_default_log
# logger_log() / logger_default integration

## Function under test

~~~c
logger_status_t logger_log(logger_t *logger, const char *msg);
~~~

## Preconditions

- Unless stated otherwise by the scenario, `logger` designates a valid logger
  instance previously created by `logger_default_create_logger()`.
- Unless stated otherwise by the scenario, the injected target stream is valid
  and writable.
- Unless stated otherwise by the scenario, the injected time service is valid
  and returns epoch time `0`.
- Unless stated otherwise by the scenario, `msg` designates a valid
  null-terminated string.

## Invalid arguments

- `msg` must not be `NULL`.

## Success

- Returns `LOGGER_STATUS_OK`.
- Writes the provided message to the injected target stream.
- Prefixes the emitted message with a UTC+0 timestamp derived from the injected
  time service.
- If `append_newline == true`, appends a trailing newline after the message.

## Failure

- Returns:
    - `LOGGER_STATUS_INVALID` if `msg == NULL`
    - `LOGGER_STATUS_IO_ERROR` when an underlying stream write fails
- If the injected time service fails while obtaining the current time, the call
  still returns `LOGGER_STATUS_OK` provided the fallback timestamp-error prefix
  is written successfully.
- When time acquisition fails, the target stream receives the fallback prefix
  `"[timestamp error] "` instead of a formatted UTC+0 timestamp.

## Test doubles

- `fake_stream`
- `fake_time`
- `fake_memory`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| a `logger_default`-backed logger is created with `append_newline == false`, the injected time service returns epoch time `0`, and `logger_log(logger, "abc")` is called | returns `LOGGER_STATUS_OK`;<br>writes `"[1970-01-01 00:00:00 UTC+0] abc"` to the target stream |
| a `logger_default`-backed logger is created with `append_newline == true`, the injected time service returns epoch time `0`, and `logger_log(logger, "abc")` is called | returns `LOGGER_STATUS_OK`;<br>writes `"[1970-01-01 00:00:00 UTC+0] abc\n"` to the target stream |
| a `logger_default`-backed logger is created, the injected time service returns epoch time `0`, and `logger_log(logger, NULL)` is called | returns `LOGGER_STATUS_INVALID` |
| the underlying stream write operation returns `STREAM_STATUS_IO_ERROR` during `logger_log(logger, "abc")` | returns `LOGGER_STATUS_IO_ERROR` |
| the injected time service returns `OSAL_TIME_STATUS_ERROR` during `logger_log(logger, "abc")` | returns `LOGGER_STATUS_OK`;<br>writes `"[timestamp error] abc"` to the target stream |

## Notes

- The logger instance is created through `logger_default_create_logger()` before
  the call under test.
- Runtime stream behavior is controlled through `fake_stream`.
- Runtime time behavior is controlled through `fake_time`.
- In the nominal scenarios, the timestamp prefix is formatted in UTC+0.
- In the nominal scenarios above, epoch time `0` corresponds to
  `1970-01-01 00:00:00 UTC+0`.
- Logger destruction is exercised through `logger_destroy()` during fixture
  teardown.
