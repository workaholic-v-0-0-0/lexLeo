@page arch_standards_naming Naming conventions

This page defines the standard naming scheme used across the project to
express **roles, responsibilities, and mutability guarantees** through
type and module suffixes.

---

## Standards

### `*_cfg_t`

- immutable;
- contains **values only**;
- no behavior (no function pointers);

Used to describe how a module is configured.

---

### `*_ctx_t`

- logically immutable after initialization;
- contains **injected dependencies, hooks, and callbacks**;
- enables behavior substitution, especially for testing and integration;
- passed as a `const` pointer throughout the module.

Used to describe the execution environment of a module.
More details in [Naming conventions](@ref arch_standards_naming)

---

### `*_state_t`

- mutable;
- represents data that **evolves during execution**;
- private to a single execution or instance;
- never exposed as part of the public API.

Used to track execution progress and transient data.

---

### `*_support`

- provides **integration-specific services**;
- intended to be consumed by **one specific module only**;
- not part of the public API;
- typically bridges a module with an external tool or framework
  (e.g. Bison, CLI, REPL).

Used to isolate tool- or integration-specific code from core logic.

---
