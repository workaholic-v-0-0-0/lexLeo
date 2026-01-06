@page arch_standards_types Type standards

# Type Standards

This document defines the standard structure and responsibilities of
configuration, context, state, and integration-related types used across
the project.

---

## Module configuration

Suffix: \c \*_cfg_t

Configuration types define **static, value-only parameters** that describe
how a module is configured.

A configuration:

- contains **no behavior** (no function pointers);
- contains **values only** (flags, limits, modes, constants, options);
- is **immutable by design**;

Configuration objects are usually created once and passed as `const`
to the module.

Typical use cases:

- feature flags
- operating modes
- size limits
- thresholds
- compile-time or initialization-time parameters

---

## Module state

Suffix: \c \*_state_t

State types define the **mutable execution state** of a module.

A state:

- is **mutable**;
- represents data that **evolves during execution**;
- is **private to a single execution or instance**;
- must not be shared across concurrent executions.

State objects typically hold:

- counters
- cursors / positions
- temporary buffers
- execution progress
- error counters

State must never be exposed as part of the public API.

---

## Module context

Suffix: \c \*_ctx_t

---

### Dependency interfaces

Suffix: \c \*_ops_t

Each dependency of a module is represented by a dedicated **operations table**
(a set of function pointers describing the interface of that dependency).

typedef struct <module>_<dependence_1>_ops_t {
<dependence_1>_<function_1>_fn_t <function_1>;
<dependence_1>_<function_2>_fn_t <function_2>;
/* ... */
} <module>_<dependence_1>_ops_t;

---

### Aggregated dependencies

Suffix: \c \*_deps_t

All dependencies required by a module are grouped into a single
**dependency aggregate**.

typedef struct <module>_deps_t {
<module>_<dependence_1>_ops_t <dependence_1>;
<module>_<dependence_2>_ops_t <dependence_2>;
/* ... */
} <module>_deps_t;

This structure represents *what the module depends on*.

---

### Hooks

Suffix: \c \*_hooks_t

Hooks define **internal override points** within a module.
They are primarily intended for testing, instrumentation, or controlled
behavior substitution.

typedef struct <module>_hooks_t {
<module>_<function_1>_fn_t <function_1>;
<module>_<function_2>_fn_t <function_2>;
/* ... */
} <module>_hooks_t;

---

### Callbacks

Suffix: \c \*_callbacks_t

Callbacks are **client-provided functions** invoked by the module.

typedef struct <module>_callbacks_t {
<module>_<callback_1>_fn_t <callback_1>;
<module>_<callback_2>_fn_t <callback_2>;
/* ... */
} <module>_callbacks_t;

---

### Context

Suffix: \c \*_ctx_t

The context aggregates all injected dependencies, hooks, callbacks,
and an opaque pointer owned by the embedding code.

The context is **logically immutable after initialization** and is passed
as a `const` pointer throughout the module.

typedef struct <module>_ctx_t {
<module>_deps_t deps;
<module>_hooks_t hooks;
<module>_callbacks_t callbacks;
void                 *user_data;
} <module>_ctx_t;

`user_data` is an opaque pointer typically used to reference a host-side
context.

---

### Default providers

Modules should expose default providers for dependencies and hooks in order
to reduce boilerplate and ease testing.

<module>_deps_t module_default_deps(void);
<module>_hooks_t module_default_hooks(void);

Optional default callbacks may also be provided when appropriate.

---

## Summary

 Suffix        | Role                                         | Mutability 
---------------|----------------------------------------------|------------
 *_cfg_t       | Module configuration (value-only parameters) | immutable  
 *_state_t     | Execution state                              | mutable    
 *_ops_t       | Dependency interface (function table)        | immutable  
 *_deps_t      | Aggregated module dependencies               | immutable  
 *_hooks_t     | Internal override points                     | immutable  
 *_callbacks_t | Client-provided callbacks                    | immutable  
 *_ctx_t       | Execution context (deps + hooks + callbacks) | immutable  

---
