@page modules_foundation_logger_default logger_default

# Purpose

The `logger_default` module provides the default concrete adapter for the
`logger` port.

It implements logging on top of:
- a borrowed target `stream_t`,
- a borrowed time service used to obtain the current epoch time,
- adapter-private formatting logic used to prefix log messages with a
  UTC+0 timestamp.

The module is designed to:
- implement the generic `logger` contract through a concrete backend,
- keep timestamp-formatting behavior private to the adapter,
- expose Composition Root services used to configure and create a
  `logger_default`-backed `logger_t`.

# Public API

- @ref logger_default_api "logger_default API"

Sub-APIs:
- @ref logger_default_cr_api "logger_default CR API"

# Architectural role

The `logger_default` module is a concrete adapter in the `logger` family.

It does not define a new public logging abstraction.
Instead, it binds backend-specific behavior to the generic `logger` port by:
- storing adapter-private runtime state in a private backend handle,
- providing adapter-private `log` and `destroy` callbacks,
- creating public `logger_t` handles through `logger_create()`.

Typical responsibilities:
- write emitted messages to an injected target stream,
- prefix emitted messages with a UTC+0 timestamp derived from the injected
  time service,
- optionally append a trailing newline,
- allocate and destroy the adapter-private backend object.

# Main concepts

## Concrete backend creation

The Composition Root creates a `logger_default`-backed logger through:
- `logger_default_default_cfg()`
- `logger_default_default_env()`
- `logger_default_create_logger()`

## Injected runtime dependencies

The adapter depends on borrowed runtime services provided by the Composition
Root through:
- `stream_t *`
- `const osal_time_ops_t *`
- `const osal_mem_ops_t *`
- `logger_env_t`

These dependencies are aggregated in:
- `logger_default_env_t`

## Timestamped message emission

At runtime, the adapter:
- obtains the current epoch time from the injected time service,
- converts it to a decomposed UTC+0 timestamp,
- formats the timestamp as text,
- writes the timestamp prefix and message to the injected stream.

If time acquisition fails, the adapter writes the fallback prefix:
- `"[timestamp error] "`

# Related modules

This module is a concrete adapter for the surrounding `logger` port.

See:
- @ref modules_foundation_logger "logger"
- @ref logger_family_group "logger family"

# See also

- @ref specifications_logger_default "logger_default specifications"
- @ref testing_foundation_logger_default_unit "logger_default unit tests"
- @ref logger_default_tests_group "logger_default tests group"