@page testing_foundation_logger_default_unit logger_default unit tests

@anchor testing_foundation_logger_default_unit_default_cfg
# logger_default_default_cfg() unit tests

See @ref specifications_logger_default_default_cfg "logger_default_default_cfg() specifications"

## Functions under test

~~~c
logger_default_cfg_t logger_default_default_cfg(void);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `logger_default_default_cfg()` is called | returns a well-formed default `logger_default_cfg_t`;<br>`ret.append_newline == true` |

## Notes

- This helper establishes the default configuration invariant for the
  `logger_default` adapter.

---

@anchor testing_foundation_logger_default_unit_default_env

# logger_default_default_env() unit tests

See @ref specifications_logger_default_default_env "logger_default_default_env() specifications"

## Functions under test

```c
logger_default_env_t logger_default_default_env(
    stream_t *stream,
    const osal_time_ops_t *time_ops,
    const osal_mem_ops_t *adapter_mem_ops,
    const osal_mem_ops_t *port_mem_ops);
```

## Test doubles

- dummy `stream_t *`
- dummy `osal_time_ops_t *`
- dummy `osal_mem_ops_t *` for the adapter memory operations
- dummy `osal_mem_ops_t *` for the port memory operations

## Tested scenarios

| WHEN                                                                                                                     | EXPECT                                                                                                                                                                                   |
| ------------------------------------------------------------------------------------------------------------------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `logger_default_default_env(stream, time_ops, adapter_mem_ops, port_mem_ops)` is called with the four dummy dependencies | returns a `logger_default_env_t` such that:<br>`ret.stream == stream`;<br>`ret.time_ops == time_ops`;<br>`ret.adapter_mem_ops == adapter_mem_ops`;<br>`ret.port_mem_ops == port_mem_ops` |

## Notes

- This test verifies that the helper stores each injected dependency unchanged
  in the corresponding environment field.
- No ownership or lifetime behavior is exercised by this test.

---

@anchor testing_foundation_logger_default_unit_create_logger

# logger_default_create_logger() unit tests

See @ref specifications_logger_default_create_logger "logger_default_create_logger() specifications"

## Functions under test

```c
logger_status_t logger_default_create_logger(
    logger_t **out,
    const logger_default_cfg_t *cfg,
    const logger_default_env_t *env);
```

## Invalid arguments

- `out` must not be `NULL`.
- `cfg` must not be `NULL`.
- `env` must not be `NULL`.

## Success

- Returns `LOGGER_STATUS_OK`.
- Stores a non-`NULL` logger handle in `*out`.

## Failure

- Returns `LOGGER_STATUS_INVALID` if `out`, `cfg`, or `env` is `NULL`.
- Returns `LOGGER_STATUS_OOM` if either allocation performed during logger
  creation fails.
- If `out != NULL`, leaves `*out` unchanged on failure.

## Test doubles

- `fake_stream`
- `fake_time`
- `fake_memory`

## Tested scenarios

| WHEN                                                                         | EXPECT                                                                     |
| ---------------------------------------------------------------------------- | -------------------------------------------------------------------------- |
| `logger_default_create_logger(out, cfg, env)` is called with valid arguments | returns `LOGGER_STATUS_OK`;<br>stores a non-`NULL` logger handle in `*out` |
| `out == NULL`                                                                | returns `LOGGER_STATUS_INVALID`                                            |
| `cfg == NULL` and `out != NULL`                                              | returns `LOGGER_STATUS_INVALID`;<br>leaves `*out` unchanged                |
| `env == NULL` and `out != NULL`                                              | returns `LOGGER_STATUS_INVALID`;<br>leaves `*out` unchanged                |
| the first allocation performed during logger creation fails                  | returns `LOGGER_STATUS_OOM`;<br>leaves `*out` unchanged                    |
| the second allocation performed during logger creation fails                 | returns `LOGGER_STATUS_OOM`;<br>leaves `*out` unchanged                    |

## Notes

- Logger creation is exercised directly through the adapter CR API.
- The target stream is provided by `fake_stream`.
- The time operations table is provided by `fake_time`.
- Failure of each allocation performed during logger creation is injected
  separately through `fake_memory`.
- The output-preservation scenarios use a non-`NULL` sentinel value to verify
  that `*out` remains unchanged on failure.

---

@anchor testing_foundation_logger_default_unit_log
# logger_default_log() unit tests

See @ref specifications_logger_default_log "logger_default_log() specifications"

## Functions under test

~~~c
static logger_status_t logger_default_log(void *backend, const char *message);
~~~

## Exercise path

This private callback is exercised through:

~~~c
logger_status_t logger_log(logger_t *logger, const char *message);
~~~

on a logger previously created by `logger_default_create_logger()`.

## Preconditions

- Unless stated otherwise by the scenario, `logger` designates a valid logger
  instance previously created by `logger_default_create_logger()`.
- Unless stated otherwise by the scenario, the injected target stream is valid
  and writable.
- Unless stated otherwise by the scenario, the injected time service is valid
  and returns epoch time `0`.
- Unless stated otherwise by the scenario, `message` designates a valid
  null-terminated string.

## Invalid arguments

- `message` must not be `NULL`.

## Success

- Returns `LOGGER_STATUS_OK`.
- Writes a UTC+0 timestamp prefix to the injected target stream.
- The formatted timestamp prefix has the form:
  `[YYYY-MM-DD HH:MM:SS UTC+0] `
- In the nominal scenarios below, epoch time `0` corresponds to:
  `[1970-01-01 00:00:00 UTC+0] `
- Writes the full contents of `message` immediately after the trailing space of
  the timestamp prefix.
- If `append_newline == true`, appends a trailing newline after the message.
- If the injected time service fails while obtaining the current time, writes
  the fallback prefix `[timestamp error] ` instead of a formatted UTC+0
  timestamp prefix.

## Failure

- Returns:
  - `LOGGER_STATUS_INVALID` if `message == NULL`
  - `LOGGER_STATUS_IO_ERROR` when a required stream write fails or is partial
- If time acquisition fails, the call still returns `LOGGER_STATUS_OK`
  provided the fallback prefix is written successfully.

## Test doubles

- `fake_stream`
- `fake_time`
- `fake_memory`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| a `logger_default`-backed logger is created with `append_newline == false`, the injected time service returns epoch time `0`, and `logger_log(logger, "abc")` is called | returns `LOGGER_STATUS_OK`;<br>writes `"[1970-01-01 00:00:00 UTC+0] abc"` to the target stream;<br>flushes the target stream |
| a `logger_default`-backed logger is created with `append_newline == true`, the injected time service returns epoch time `0`, and `logger_log(logger, "abc")` is called | returns `LOGGER_STATUS_OK`;<br>writes `"[1970-01-01 00:00:00 UTC+0] abc\n"` to the target stream;<br>flushes the target stream |
| a `logger_default`-backed logger is created, the injected time service returns epoch time `0`, and `logger_log(logger, NULL)` is called | returns `LOGGER_STATUS_INVALID` |
| the underlying stream write operation returns `STREAM_STATUS_IO_ERROR` during `logger_log(logger, "abc")` | returns `LOGGER_STATUS_IO_ERROR` |
| the injected time service returns `OSAL_TIME_STATUS_ERROR` during `logger_log(logger, "abc")` | returns `LOGGER_STATUS_OK`;<br>writes `"[timestamp error] abc"` to the target stream |

## Notes

- The private callback is not called directly.
- It is exercised through the public `logger` borrower API on a concrete
  logger produced by `logger_default_create_logger()`.
- Runtime stream behavior is controlled through `fake_stream`.
- Runtime time behavior is controlled through `fake_time`.

---

@anchor testing_foundation_logger_default_unit_destroy
# logger_default_destroy() unit tests

See @ref specifications_logger_default_destroy "logger_default_destroy() specifications"

## Functions under test

~~~c
static logger_status_t logger_default_destroy(void *backend);
~~~

## Exercise path

This private callback is exercised through:

~~~c
logger_status_t logger_destroy(logger_t **logger);
~~~

on a logger previously created by `logger_default_create_logger()`.

## Success

- Returns `LOGGER_STATUS_OK`.
- Releases the private `logger_default` backend object.
- Does not destroy the borrowed target stream.
- Does not destroy the borrowed time operations table.

## Test doubles

- `fake_stream`
- `fake_time`
- `fake_memory`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| a `logger_default`-backed logger is created and later destroyed through `logger_destroy(&logger)` | returns `LOGGER_STATUS_OK`;<br>releases the private backend object;<br>destroys the public logger handle;<br>sets `logger` to `NULL`;<br>does not destroy the borrowed target stream |

