@page modules_foundation_osal_env osal_env

# Purpose

The `osal_env` module provides the default environment-variable query service
for the project.

It exposes a small portable API for reading values from the current process
environment, while delegating the actual lookup mechanism to the active
platform implementation.

The module is designed to:
- provide a minimal cross-platform environment access abstraction,
- hide platform-specific environment facilities,
- expose a stable API used by higher-level modules,
- centralize environment queries behind the OSAL boundary,
- isolate platform-specific behavior behind a portable contract.

# Public API

- @ref osal_env_api "osal_env API"

# Architectural role

The `osal_env` module is an OSAL service module.

It is not a configuration system, parser, or persistence layer.

Instead, it provides a minimal portable environment access service by:
- exposing a read-only query function,
- delegating native environment access to the backend,
- returning direct pointers to process-owned value strings,
- isolating platform-specific lookup semantics behind a stable contract.

Typical responsibilities:
- query the value associated with an environment variable name,
- detect whether a variable is present or absent,
- provide runtime configuration inputs to higher-level modules,
- hide backend-specific environment APIs.

# Main concepts

## Environment query function

The main entry point of the module is:

- `osal_getenv()`

It searches the current process environment for a variable name and returns
its associated value when available.

## Read-only access

The current public contract provides read-only access to the environment.

It does not currently expose services for:
- creating variables,
- updating variables,
- removing variables,
- enumerating the full environment.

## Borrowed result pointer

Returned values remain owned by the process environment.

Callers receive a borrowed pointer and must not free or modify it through the
module API.

The lifetime and stability of the returned pointer may depend on subsequent
environment modifications and platform behavior.

## Platform-dependent rules

Some details may vary by platform, including:
- valid variable naming rules,
- case-sensitivity of names,
- internal storage behavior,
- exact backend implementation strategy.

These differences are intentionally hidden behind the OSAL contract.

# Related modules

This module belongs to the OSAL layer and is commonly consumed by higher-level
modules needing runtime configuration from the process environment.

See:
- @ref osal_family_group "OSAL family"

# See also

- @ref specifications_osal_getenv "osal_getenv specifications"
- @ref osal_env_unit_tests "osal_env tests group"