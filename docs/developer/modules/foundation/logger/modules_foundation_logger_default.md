@page modules_foundation_logger_default logger_default

# Purpose

The `logger_default` module provides the default concrete adapter for the
`logger` port.

It implements logging on top of:
- a borrowed target `stream_t`,
- a borrowed time service used to obtain the current epoch time,
- borrowed memory operations used to manage the adapter-private backend,
- adapter-private formatting logic used to prefix log messages with a
  UTC+0 timestamp.

The module is designed to:
- implement the generic `logger` contract through a concrete backend,
- keep timestamp-formatting behavior private to the adapter,
- expose Composition Root services used to configure, create, and complete
  initialization of a `logger_default_t` backend,
- expose the `logger_vtbl_t` implemented by the adapter.

# Public API

- @ref logger_default_api "logger_default API"

Sub-APIs:
- @ref logger_default_cr_api "logger_default CR API"

# Architectural role

The `logger_default` module is a concrete adapter in the `logger` family.

It does not define a new public logging abstraction.
Instead, it binds backend-specific behavior to the generic `logger` port by:
- storing adapter-private runtime state in a private `logger_default_t` backend
  handle,
- providing adapter-private `log` and `destroy` callbacks,
- exposing these callbacks through `logger_default_vtbl()`,
- allowing a Composition Root to create and initialize the backend before
  binding it to the generic `logger` port.

Typical responsibilities:
- write emitted messages to an injected target stream,
- prefix emitted messages with a UTC+0 timestamp derived from the injected
  time service,
- optionally append a trailing newline,
- flush the target stream after successful message emission,
- allocate and destroy the adapter-private backend object.

# Main concepts

## Concrete backend creation

The Composition Root prepares and initializes a `logger_default_t` backend
through:
- `logger_default_default_cfg()`
- `logger_default_default_env()`
- `logger_default_create()`
- `logger_default_complete_default_init()`

The adapter operations used by the generic `logger` port are obtained through:
- `logger_default_vtbl()`

## Injected runtime dependencies

The adapter depends on borrowed runtime services provided by the Composition
Root through:
- `stream_t *`
- `const osal_time_ops_t *`
- `const osal_mem_ops_t *`

These dependencies are aggregated in:
- `logger_default_env_t`

The adapter configuration is represented separately by:
- `logger_default_cfg_t`

## Timestamped message emission

At runtime, the adapter:
- obtains the current epoch time from the injected time service,
- converts it to a decomposed UTC+0 timestamp,
- formats the timestamp as text,
- writes the timestamp prefix and message to the injected stream,
- optionally writes a trailing newline,
- flushes the target stream.

If time acquisition or timestamp conversion fails, the adapter writes the
fallback prefix:
- `"[timestamp error] "`

# Related modules

This module is a concrete adapter for the surrounding `logger` port.

See:
- @ref modules_foundation_logger "logger"
- @ref logger_family_group "logger family"

# See also

- @ref specifications_logger_default "logger_default specifications"
- @ref testing_foundation_logger_default_cr_unit "logger_default_cr.c unit tests"
- @ref logger_default_tests_group "logger_default tests group"
