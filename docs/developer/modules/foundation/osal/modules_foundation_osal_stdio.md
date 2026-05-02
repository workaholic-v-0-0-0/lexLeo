@page modules_foundation_osal_stdio osal_stdio

# Purpose

The `osal_stdio` module provides the default low-level standard I/O service
for the project.

It exposes portable standard stream operations through `osal_stdio_ops_t`,
while keeping the concrete backend implementation private to the module.

The module is designed to:
- provide injectable access to standard streams,
- hide backend-specific stream handles behind opaque `OSAL_STDIO` handles,
- expose a default operations table for higher-level modules,
- make standard I/O dependencies replaceable in tests through fake providers.

# Public API

- @ref osal_stdio_api "osal_stdio API"

# Architectural role

The `osal_stdio` module is an OSAL service module.

It is not a logging system, terminal abstraction, formatting layer, or
business-oriented console framework.

Instead, it provides a minimal injectable standard I/O service by:
- exposing an operations table,
- wrapping standard streams inside opaque `OSAL_STDIO` handles,
- isolating standard I/O access behind a stable contract.

# Main concepts

## Default operations table

The entry point of the module is:

- `osal_stdio_default_ops()`

The returned table provides:
- `get_stdin()`
- `get_stdout()`
- `get_stderr()`
- `read()`
- `write()`
- `flush()`
- `error()`
- `eof()`

## Opaque standard I/O handle

Standard streams are represented through:

- `OSAL_STDIO`

Callers manipulate streams only through `osal_stdio_ops_t`.

# Related modules

This module belongs to the OSAL layer and is commonly consumed by higher-level
modules needing injectable standard I/O.

See:
- @ref osal_family_group "OSAL family"

# See also

- @ref specifications_osal_stdio "osal_stdio specifications"
- @ref osal_stdio_unit_tests "osal_stdio tests group"
