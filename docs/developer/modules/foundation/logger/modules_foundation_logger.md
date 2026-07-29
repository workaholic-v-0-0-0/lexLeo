@page modules_foundation_logger logger

# Purpose

The `logger` port defines the common logging abstraction used by the
foundation layer.

It provides:
- a borrower-facing runtime API for emitting log messages,
- Composition Root services for constructing, completing, and destroying
  `logger_t` handles,
- adapter-facing contracts used to bind concrete logging backends to the
  generic `logger` port.

The module is designed to separate:
- the generic logging contract,
- the concrete adapter implementations,
- the Composition Root wiring used to connect both.

# Public API

- @ref logger_api "logger API"

Sub-APIs:
- @ref logger_borrowers_api "borrowers API"
- @ref logger_cr_api "CR API"
- @ref logger_adapters_api "adapters API"

# Architectural role

The `logger` module is a foundation port.
It defines the public logging boundary used by runtime code, while concrete
adapter modules provide backend-specific implementations.

Typical responsibilities:
- expose the common `logger_t` handle and status model,
- route borrower-facing logging operations through adapter-bound dispatch
  tables,
- provide Composition Root services for constructing and destroying logger
  handles,
- provide the environment contract used to inject runtime dependencies.

# Main concepts

## Borrower-facing usage

Runtime code emits messages through `logger_t` handles using the public
borrower API:
- `logger_log()`

## Adapter binding

Concrete adapters bind backend logic to the generic logger port through:
- `logger_vtbl_t`

## Logger construction

The Composition Root constructs and completes `logger_t` handles through:
- `logger_env_t`
- `logger_default_env()`
- `logger_create()`
- `logger_complete_default_init()`

## Logger destruction

The Composition Root destroys `logger_t` handles through:
- `logger_destroy()`

# Related modules

Concrete adapters for this port are documented in the surrounding
`logger` family.

See:
- @ref logger_family_group "logger family"

# See also

- @ref specifications_logger "logger specifications"
- @ref testing_foundation_logger_family_page "logger family tests page"
- @ref logger_tests_group "logger tests group"
