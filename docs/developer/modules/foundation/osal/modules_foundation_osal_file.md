@page modules_foundation_osal_file osal_file

# Purpose

The `osal_file` module provides the default low-level file operations service
for the project.

It exposes a portable file API through `osal_file_ops_t`, while delegating the
actual system calls to the active platform implementation.

The module is designed to:
- provide a small cross-platform file abstraction,
- hide platform-specific file handles and APIs,
- normalize file-related status codes through `osal_file_status_t`,
- allocate and destroy opaque `OSAL_FILE` handles through injected memory
  services,
- expose the default operations table used by higher-level modules.

# Public API

- @ref osal_file_api "osal_file API"

# Architectural role

The `osal_file` module is an OSAL service module.

It is not a high-level stream abstraction and does not define buffering,
formatting, logging, or business-oriented I/O behavior.

Instead, it provides a minimal portable file service by:
- exposing an operations table (`open`, `read`, `write`, `flush`, `close`),
- wrapping native file resources inside opaque `OSAL_FILE` handles,
- mapping backend failures to portable status values,
- isolating platform-specific implementations behind a stable contract.

Typical responsibilities:
- open a filesystem resource from a pathname and portable mode,
- read raw bytes or elements from an open file,
- write raw bytes or elements to an open file,
- flush pending writes,
- close and release file resources.

# Main concepts

## Default operations table

The entry point of the module is:

- `osal_file_default_ops()`

It returns the default callable operations table for the active platform.

The returned table provides:
- `open()`
- `read()`
- `write()`
- `flush()`
- `close()`

## Opaque file handle

Opened files are represented through:

- `OSAL_FILE`

This handle hides backend-specific runtime state such as native descriptors,
native streams, or adapter-private metadata.

Callers manipulate the resource only through `osal_file_ops_t`.

## Injected memory services

Creation and destruction of wrapper handles rely on injected allocation
services:

- `const osal_mem_ops_t *`

This keeps memory policy external to the module and allows testing through
fake providers.

## Portable statuses

Public operations report outcomes through:

- `osal_file_status_t`

Typical results include:
- success,
- invalid arguments,
- out-of-memory,
- I/O failure.

This avoids leaking backend-native error models directly into callers.

## Portable modes

The portable modes supported by the public contract are:

- `"rb"`
- `"wb"`
- `"ab"`

Their backend translation is implementation-specific and remains private to the
module.

# Related modules

This module belongs to the OSAL layer and is commonly consumed by higher-level
adapters needing filesystem access.

See:
- @ref osal_family_group "OSAL family"

# See also

- @ref specifications_osal_file "osal_file specifications"
- @ref osal_file_unit_tests "osal_file tests group"
