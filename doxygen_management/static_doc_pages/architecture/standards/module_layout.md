@page arch_standards_module_layout Module layout standards

# Module layout standards

This document defines the standard directory layout for a module, along with
the responsibilities and visibility rules associated with each part.

The goal is to:

- clearly separate public API, internal implementation, and integrations;
- enforce architectural boundaries via the filesystem and CMake;
- make module intent immediately readable from its layout.

---

## Reference layout

    module/
    ├── CMakeLists.txt
    ├── include/
    │   ├── module.h
    │   ├── module_cfg.h
    │   └── module_ctx.h
    ├── src/
    │   ├── module.c
    │   └── internal/
    │       ├── module_internal.h
    │       └── module_state.h
    ├── support/
    │   ├── consumer_1/
    │   │   ├── module_consumer_1_support.h
    │   │   └── module_consumer_1_support.c
    │   └── consumer_2/
    │       ├── module_consumer_2_support.h
    │       └── module_consumer_2_support.c
    └── tests/
        ├── CMakeLists.txt
        ├── include/
        │   └── module_test_api.h
        ├── unit/
        │   ├── white_box
        │   └── black_box
        └── integration/

---

## include/ — Public API

The include/ directory contains the public, stable API of the module.

Typical contents:

- module.h — main public API;
- module_cfg.h — configuration types (*_cfg_t) (see [Type standards](@ref arch_standards_types));
- module_ctx.h — context types (*_ctx_t) (see [Type standards](@ref arch_standards_types)).

Public headers must:

- be self-contained;
- compile when included alone;
- avoid including headers from src/internal/.

---

## src/ — Core implementation

The src/ directory contains the core implementation of the module.

### src/module.c

- implements the public API declared in include/;
- may include both public headers and internal headers.

---

## src/internal/ — Internal implementation details

The src/internal/ directory contains private headers that are part of the
module implementation but are not part of the public API.

Typical contents:

- internal helper declarations;
- execution state types (*_state_t);
- internal invariants and utilities.

Rules:

- headers in src/internal/ must never be included by other modules;
- exposed via CMake as PRIVATE include directories only.

---

## support/ — Consumer-specific integration APIs

The support/ directory contains integration code intended for a single,
specific consumer of the module.

A consumer can be a higher-level module requiring a specialized interface.

Each consumer gets its own subdirectory:

    support/<consumer>/
        module_<consumer>_support.h
        module_<consumer>_support.c

### Role of support code

Support code:

- adapts the module to the needs or conventions of a specific consumer;
- may depend on both the module public API and the consumer’s API;
- is not intended to be reused by other consumers.

Support headers:

- are not part of the public API;
- are exposed only to the intended consumer via CMake targets;
- must not be globally visible.

---

## `tests/` — Tests and test-only APIs

The `tests/` directory contains all tests for the module, as well as
**test-only utilities**.

### `tests/include/` — Test API

The `tests/include/` directory may contain **test-only APIs**, such as:

- `module_test_api.h`

This header:

- provides helpers, builders, or observation utilities for tests;
- may expose controlled access to internal behavior **without exposing
  internal state types**;
- is exposed via CMake **only to test targets**.

It must never be installed or exposed as part of the public API.

### Test structure

- `unit/` — unit tests (isolated, fast);
- `integration/` — integration tests (with real dependencies).

Tests must:

- treat the module as a black box by default;
- access internal behavior only through explicit test-specific mechanisms
  (hooks, callbacks, or test APIs).

---

## CMake visibility rules (summary)

- include/ → PUBLIC
- src/ → PRIVATE
- src/internal/ → PRIVATE
- support/consumer/ → PRIVATE to a consumer-specific target
- tests/ → test-only targets

These rules ensure that architectural boundaries are enforced at build time.

---

## Summary

 Directory     | Responsibility                     
---------------|------------------------------------
 include/      | Public, stable API                 
 src/          | Core implementation                
 src/internal/ | Private implementation details     
 support/      | Consumer-specific integration APIs 
 tests/        | Unit and integration tests         

---

This layout, combined with strict CMake visibility rules, guarantees a clear
separation between API, implementation, and integration concerns, and scales
naturally as the project grows.
