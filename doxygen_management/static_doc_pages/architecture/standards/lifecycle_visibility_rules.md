@page lifecycle_visibility_rules Lifecycle Visibility Rules (create / destroy)

# Lifecycle Visibility Rules (create / destroy)

This document defines **when `*_create` / `*_destroy` functions are public**
and **when they are internal**, in the context of the LexLeo architecture
(ctx / dependency injection / VM-owned infrastructure).

The goal is to make **ownership, lifecycle authority, and wiring responsibilities explicit**
and to avoid accidental coupling or misuse of infrastructure modules.

This document does not define module structure or type roles.
Those are defined in:

- @ref arch_standards_types
- @ref arch_standards_module_layout

---

## Core principles

### Dependency injection does not imply ownership

Receiving a dependency via DI does **not** mean owning its lifecycle.

A module may depend on an object without being responsible for:

- creating it;
- destroying it;
- coordinating its lifetime.

Ownership and lifecycle authority are **architectural decisions**,
not side effects of dependency injection.

---

### Lifecycle authority is explicit and centralized

In the LexLeo architecture:

- **Lifecycle orchestration is owned by the composition root** (VM bootstrap / teardown);
- runtime modules manage **only the resources they explicitly own**;
- cross-module creation and destruction order is never implicit.

Rule:

> **Everything created under VM authority is destroyed under VM authority.**

---

## Visibility rule for `*_create` / `*_destroy`

### Rule 1 — Modules requiring a `_ctx_t`

If a module requires a `_ctx_t` to be constructed:

- its creation involves wiring decisions (dependencies, adapters, strategies);
- its lifecycle is **bootstrap-owned**;
- its `*_create` / `*_destroy` functions are **internal**.

Consequences:

- `*_create` / `*_destroy` are declared in `include/internal/`;
- they are visible only to:
    - VM bootstrap,
    - VM teardown,
    - test composition roots (via CMake);
- public headers expose **runtime operations only**, never lifecycle APIs.

This applies regardless of whether the module is stateful or not.

---

### Rule 2 — Modules not requiring a `_ctx_t`

If a module can be constructed **without a `_ctx_t`**:

- it has no wiring-time dependencies;
- it does not rely on VM-controlled assembly;
- its lifecycle is **local and self-contained**.

In that case:

- `*_create` / `*_destroy` may be **public**;
- the module may be instantiated independently of the VM;
- it follows the classic reusable C library model.

This includes data structures, utilities, adapters, or services that are
designed for reuse outside the VM.

---

### Rule 3 — Stateless modules

If a module has:

- no runtime state,
- no ownership,
- no lifecycle,

then:

- it exposes **no `create` / `destroy` APIs**;
- all functions operate on caller-owned data or are pure.

---

## Factories and dynamic creation

If a module must create or destroy multiple instances of another module at runtime:

- it must **not** receive a `_ctx_t`;
- it must receive an **abstract factory or provider interface** via DI.

In this case:

- the factory interface may be public;
- concrete implementations remain VM-owned;
- factories do **not** imply public lifecycle APIs for concrete modules.

Factories are a controlled escape hatch, not a general relaxation of lifecycle rules.

---

## Testing and internal access

Tests are allowed to:

- include `include/internal/` headers;
- act as alternative composition roots;
- call internal `*_create` / `*_destroy`;
- inject fakes, mocks, or alternative adapters.

This preserves:

- strict public API boundaries;
- full testability;
- no leakage of lifecycle authority to runtime consumers.

---

## Canonical rules (to remember)

> If a module requires a `_ctx_t`, its lifecycle APIs are internal and
> controlled by the bootstrap/teardown.

> Public headers expose runtime behavior, never lifecycle orchestration.

These rules enforce:

- explicit ownership;
- clear layering;
- safe dependency injection;
- long-term architectural stability.
