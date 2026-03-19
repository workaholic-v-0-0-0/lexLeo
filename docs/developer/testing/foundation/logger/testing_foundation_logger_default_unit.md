@page testing_foundation_logger_default_unit logger_default unit tests

It covers:
- `logger_default_default_cfg()`
- `logger_default_default_env()`

---

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

~~~c
logger_default_env_t logger_default_default_env(
    stream_t *stream,
    const osal_time_ops_t *time_ops,
    const osal_mem_ops_t *adapter_mem,
    const logger_env_t *port_env);
~~~

## Success

- `env.stream == stream`.
- `env.time_ops == time_ops`.
- `env.adapter_mem == adapter_mem`.
- `env.port_env == *port_env`.

## Failure

- None.

## Test doubles

- dummy `stream_t *`
- dummy `osal_time_ops_t *`
- dummy `osal_mem_ops_t *`
- dummy `logger_env_t`

## Tested scenarios

| WHEN                                                                                              | EXPECT |
|---------------------------------------------------------------------------------------------------|---|
| `logger_default_default_env(stream, time_ops, adapter_mem, port_env)` is called with valid inputs | returns a `logger_default_env_t` such that `env.stream == stream`, `env.adapter_mem == adapter_mem`, and `env.port_env == *port_env` |

## Notes

- This helper preserves the injected stream, time, and logger-port dependencies
  provided by the caller.
- The returned environment does not take ownership of the borrowed `stream`
  handle, borrowed `time_ops` table, or borrowed `adapter_mem` table.
