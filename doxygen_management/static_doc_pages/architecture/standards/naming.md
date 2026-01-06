@page arch_standards_naming Naming conventions

# Naming conventions

This page defines the standard suffix-based naming conventions used across
the project to make **architectural intent immediately readable from names**.

This document is a **reference index only**.

It does **not** define architecture, lifecycle rules, or ownership semantics.
Those are defined in:

- @ref arch_standards_types
- @ref arch_standards_module_layout

The purpose of this page is to help readers quickly identify the role of a
type from its name alone.

---

## Standard suffixes (overview)

### `*_cfg_t` — Configuration

Value-only configuration parameters.

Indicates:

- functional or policy-level options;
- immutable data;
- no behavior.

Details and rules: see @ref arch_standards_types.

---

### `*_ctx_t` — Context (wiring)

Wiring-time context used to assemble a module.

Indicates:

- dependency injection;
- adapter/strategy selection;
- bootstrap-controlled assembly.

Details and rules: see @ref arch_standards_types
and @ref arch_standards_module_layout.

---

### `*_state_t` — Runtime state

Mutable execution state of a module instance.

Indicates:

- data evolving during execution;
- internal-only visibility.

Details and rules: see @ref arch_standards_types.

---

### `<module>_t` — Module handle

Primary runtime handle of a module.

Indicates:

- a living executable module instance;
- runtime ownership of state and local resources.

Details and rules: see @ref arch_standards_types.

---

### `*_ops_t` — Operation tables (capabilities)

Abstract capability interfaces expressed as function pointer tables.

Indicates:

- replaceable services or strategies;
- no backend instance;
- no lifetime management.

Details and rules: see @ref arch_standards_types.

---

### `*_vtbl_t` — Port / adapter contracts

Callable contracts binding a port to an opaque backend implementation.

Indicates:

- port/adapter pattern;
- backend instance hidden behind `void *`;
- explicit teardown semantics.

Details and rules: see @ref arch_standards_types.

---

### `*_callbacks_t` — Outbound notifications

Client-provided notification callbacks.

Indicates:

- observability only;
- no control-flow authority.

Details and rules: see @ref arch_standards_types.

---

### `*_factory_*` — Factory modules (construction API)

Naming used for modules whose primary responsibility is the **construction**
(and typically destruction) of instances from another module.

Indicates:

- object construction boundary (creation API);
- optional policy enforcement (pooling, limits, instrumentation);
- may be either handle-based (`*_factory_t`) or stateless (functions only).

Notes:

- `*_factory_ops_t` denotes an injectable factory capability table.
- Ownership and lifetime rules are defined in @ref arch_standards_types
  and @ref arch_standards_module_layout (not here).

---

## Discouraged or non-standard suffixes

The project intentionally avoids vague or ambiguous suffixes such as:

- `*_support`
- `*_utils`
- `*_helpers` (unless strictly internal)

Integration and variation points must be expressed explicitly using
`*_ops_t`, `*_vtbl_t`, adapter modules, or callbacks.

---

## Summary table

 Suffix        | Intent (at a glance)          
---------------|-------------------------------
 *_cfg_t       | Value-only configuration      
 *_ctx_t       | Wiring / dependency injection 
 *_state_t     | Mutable runtime state         
 <module>_t    | Runtime module handle         
 *_ops_t       | Capability interfaces         
 *_vtbl_t      | Port / adapter contracts      
 *_callbacks_t | Outbound notifications        

---

This page exists to improve readability and onboarding.
Architectural rules and guarantees are defined elsewhere and must not
be inferred solely from naming.
