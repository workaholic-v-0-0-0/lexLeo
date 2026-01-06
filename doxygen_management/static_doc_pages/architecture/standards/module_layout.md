@page arch_standards_module_layout Module layout standards

# Module layout standards

This document defines the standard directory layout options for a module,
along with the responsibilities and visibility rules associated with each part.

The goal is to:

- clearly separate public API, internal implementation, wiring/assembly, and tests;
- enforce architectural boundaries via the filesystem and CMake;
- make module intent immediately readable from its layout;
- reflect ownership and lifecycle responsibilities (bootstrap vs runtime);
- avoid premature abstractions or over-engineering.

IMPORTANT  
The layout described in this document represents a maximal, feature-complete envelope.
A real module is not expected to provide all of these directories or headers.

Modules must include only the parts that make sense for their nature
(stateless vs stateful, core vs adapter, simple vs configurable).

---

## Reference layout (maximal envelope)

    module/
    ├── CMakeLists.txt
    ├── include/
    │   ├── module.h
    │   ├── module_cfg.h        (optional, public only if contractual)
    │   ├── module_factory.h
    │   ├── module_ops.h
    │   ├── module_types.h
    │   └── internal/
    │       ├── module_ctx.h    (optional, bootstrap-only configuration)
    │       └── module_factory_ctx.h
    ├── src/
    │   ├── module.c
    │   └── internal/
    │       ├── module_backend.h (if adapter)
    │       ├── module_internal.h
    │       └── module_state.h
    └── tests/
        ├── CMakeLists.txt
        ├── include/
        │   └── module_test_api.h
        ├── support/
        │   ├── CMakeLists.txt
        │   └── fake_provider/
        │       ├── CMakeLists.txt
        │       ├── include/
        │       │   └── module_fake_provider.h
        │       └── src/
        │       │   └── module_fake_provider.c
        ├── unit/
        │   └── (white_box / black_box)
        └── integration/

This layout describes everything a module may contain, not what it must contain.

---

## include/ — Public API (runtime-facing)

The include/ directory contains the public, stable API of the module.

It exposes the types and functions required to *use* a module at runtime,
not to assemble or wire it.

Typical contents:

- module.h — main public API (always present, exposes `<module>_t`);
- module_cfg.h — optional public configuration (`*_cfg_t`), only if contractual;
- module_ops.h — public capability interfaces (`*_ops_t`);
- module_types.h — public forward declarations and shared types.

Public headers must:

- be self-contained;
- compile when included alone;
- avoid including headers from src/internal/ or include/internal/;
- never expose wiring or bootstrap-only concepts.

---

## include/internal/ — Wiring / bootstrap API (optional)

The include/internal/ directory contains non-public headers reserved for
**module assembly and lifecycle orchestration**.

These headers are intended for:

- the application bootstrap;
- the centralized teardown logic;
- black-box tests requiring wiring/injection;
- selected white-box or integration tests.

Typical contents:

- module_ctx.h — context types (`*_ctx_t`) defining how a module is wired;

Key principles:

- ctx are wiring-time objects, not runtime objects;
- they express how modules are assembled and connected;
- they do not represent execution state or behavior;
- they are consumed only during construction and teardown coordination.

Visibility rules:

- headers in include/internal/ are not installed;
- they are exposed via CMake only to bootstrap and test targets;
- regular module consumers must never include them.

Rationale:

- wiring and lifecycle orchestration belong to the bootstrap/teardown layer;
- keeping these headers internal prevents accidental architectural coupling;
- it preserves the rule: runtime code sees `<module>_t`, not wiring types.

---

## src/ — Runtime implementation

The src/ directory contains the runtime implementation of the module.

It defines:

- the concrete behavior of `<module>_t`;
- the logic executed during the program’s lifetime;
- the code that owns and manipulates runtime state.

### src/module.c

- implements the public API declared in include/;
- defines creation/destruction functions for `<module>_t`;
- consumes module_ctx.h only at construction time;
- for adapter modules: defines the concrete virtual table adapter `*_vtbl` used to implement the port;
- never exposes wiring details to callers.

---

## src/internal/ — Internal runtime details (optional)

The src/internal/ directory contains private headers that are part of the
module’s runtime implementation but are not part of any API.

Typical contents:

- execution state types (`*_state_t`);
- internal invariants and helpers;
- concrete adapter/backend types.

Rules:

- headers in src/internal/ must never be included by other modules;
- exposed via CMake as PRIVATE include directories only;
- concrete adapter/backend types must remain confined here.

Note  
State headers are expected only for stateful modules.  
Stateless modules may not require src/internal/ at all.

---

## tests/ — Tests and test-only APIs

The tests/ directory contains all tests for the module, as well as test-only
utilities.

### tests/include/ — Test API

The tests/include/ directory may contain test-only APIs, such as:

- module_test_api.h

This header:

- provides helpers, builders, or observation utilities for tests;
- may expose controlled access to behavior (never raw state);
- is exposed via CMake only to test targets.

It must never be installed or exposed as part of the public API.

This directory is exported via:

- `<module>_test_headers` (INTERFACE)

---

### tests/support/ — Test-only support code

The tests/support/ directory contains test-only support libraries owned by this
module but intended to be consumed by other modules’ tests. 

This code is not part of production builds.

Typical use cases:

- `<module>_fake_provider` — a module-local facade that exposes/configures
  reusable fakes for *this module* (reusable fake implementations live in
  `lexleo/tests/test_support/` as `fake_<module>`).

---

## CMake visibility rules (summary)

- include/ → PUBLIC
- include/internal/ → PRIVATE (bootstrap/tests only)
- src/ → PRIVATE
- src/internal/ → PRIVATE
- tests/ → test-only targets

Standard targets:

- `<module>` — runtime library target
- `<module>_wiring_headers` — INTERFACE target exporting wiring/injection headers
- `<module>_test_headers` — INTERFACE target exporting `tests/include/`
- `<module>_test_usage_requirements` — INTERFACE bundle of module-specific test
  requirements for the module's own tests (does NOT include global test frameworks)

Important  
Global unit test frameworks (e.g., cmocka) must be linked explicitly by each
test executable. They must not be hidden inside `<module>_test_usage_requirements`.

---

## Summary

 Directory         | Responsibility                     
-------------------|------------------------------------
 include/          | Runtime-facing public API          
 include/internal/ | Wiring / injection API (bootstrap + tests) 
 src/              | Runtime implementation             
 src/internal/     | Private runtime details            
 tests/            | Unit and integration tests         
 tests/include/    | Module test API (`<module>_test_headers`)
 tests/support/    | Module-local test-only support (fake provider facade)

---

This layout is explicitly aligned with the Type standards and Test management
documents.

Together, they ensure a strict separation between runtime objects,
wiring/lifecycle orchestration, and internal implementation details,
while remaining flexible enough to support simple modules and complex
runtime/adapter architectures.
