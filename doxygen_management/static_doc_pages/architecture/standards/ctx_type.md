@page module_context_model Module context model

# Module Context Model (deps / backends / hooks / ops / callbacks)

## Purpose

This document describes the **context-based architecture** adopted in the project
to structure dependencies, interchangeable implementations, and extension points
in a **modular, testable, and explicit C codebase**.

This model is designed to:

- apply SOLID principles in practice (especially DIP and ISP),
- avoid hidden dependencies and service locators,
- allow **controlled dependency injection** (tests, variants),
- keep the public API minimal and readable,
- support *selective* test doubling without over-engineering.

A key idea of this document is that **everything can be made doublable,
but nothing is required to be**.  
Each module should expose **only the seams that are architecturally meaningful**.

---

## Overview

Each module *may* define an optional **context structure** (`<module>_ctx_t`)
used **only at creation time** to configure the module instance.

The context aggregates *potential* injection points:

- deps      : dependencies on other modules or services,
- backends  : interchangeable implementations (ports / adapters),
- hooks     : inbound extension points required for the module to operate,
- ops       : optional internal strategies or policies (if substitution is desired),
- callbacks : outward notifications,
- user_data : opaque caller-owned state for injected functions.

Typical *maximal* shape (all fields are optional):

```c
typedef struct <module>_ctx_t {
    <module>_deps_t deps;
    const <module>_backends_t *backends;
    const <module>_hooks_t *hooks;
    const <module>_ops_t *ops;
    <module>_callbacks_t callbacks;
    const <port>_ctx_t *<port>_ctx;
    void *user_data;
} <module>_ctx_t;
```

IMPORTANT  
This structure represents the *upper bound* of what can be injected.  
A real module is expected to **keep only the fields it actually needs**.

The ctx is optional: if `NULL`, the module falls back to its default configuration.

---

## 1. Dependencies (deps)

### Role

`deps` represents **explicit dependencies on other internal project modules**
or services required by the module at runtime.

A dependency may be expressed as:

- a pointer to another module’s **handle** (`dep_t *`), or
- a pointer to an **interface** (ops + optional user data).

### OSAL rule

All interactions with the outside world **must go through OSAL**.

OSAL is treated as a regular internal module, but it is the **only layer allowed**
to access libc or OS-level facilities.

As a consequence, project modules:

- never call `malloc`, `free`, or other libc functions directly,
- never access OS services implicitly,
- rely exclusively on OSAL public APIs or OSAL ops tables.

This keeps external effects **visible, controlled, and auditable**.

### Example: injectable OSAL memory dependency

```c
#include "mem/osal_mem.h"

typedef struct module_deps_t {
    const osal_mem_ops_t *mem;
} module_deps_t;

void *module_create_object(module_t *m, size_t size)
{
    return m->deps.mem->malloc(size);
}
```

### Design rules

- `deps` expresses **runtime needs**, not mere include relationships.
- Dependencies are injected explicitly (no lookups, no globals).
- `deps` may be:
    - copied into the module handle, or
    - referenced via pointer (lifetime managed by the bootstrap).

---

## 2. Backends

### Role

Backends represent **interchangeable implementations** of a given concept,
typically following a *port / adapter* pattern.

Common examples:

- stdio / fs / dynamic_buffer streams,
- flex / fake_lexer,
- real_backend / trace_backend.

They are expressed via immutable vtables (`*_ops_t`).

### Adopted model

```c
typedef struct <module>_backends_t {
    const <module>_<backend_1>_ops_t *backend_1;
    const <module>_<backend_2>_ops_t *backend_2;
} <module>_backends_t;
```

And in the context:

```c
const <module>_backends_t *backends;
```

### Rationale

- Vtables are immutable → const pointers.
- No copying of vtables.
- Explicit selection of implementations.
- Clean substitution in tests.

Guideline  
Backends are primarily intended for **ports/adapters**.  
Internal core modules usually depend on *handles*, not backends.

---

## 3. Hooks (inbound extension points)

### Role

Hooks represent **inbound extension points**: functions provided by the host
application and **called by the module** to obtain data or services it requires
to operate.

Hooks are **not notifications**.  
They are **functional dependencies**, expressed as function pointers.

Typical example:

- `read_ast` (interpreter requests the next AST),

### Example

```c
typedef struct <module>_hooks_t {
    struct ast *(*read_ast)(void *user_data);
} <module>_hooks_t;
```

### Design rules

- Hooks are **required or optional depending on the module semantics**.
- Hooks usually rely on `user_data` to access host-owned resources.

Terminology note  
In professional C architectures, such functions are commonly called **hooks**
or **ports**, not callbacks.

---

## 4. Ops (internal strategies – optional)

### Role

`ops` represents **internal policies or strategies** of a module
that may be substituted for testing or variation purposes.

Examples:

- alternative error handling strategies,
- internal algorithms,
- tracing or instrumentation hooks,
- deliberately replaceable internal behavior.

Important principle

Making a module “fully doublable” does **not** mean
making every function virtual.

Only operations that represent **meaningful architectural seams**
should be placed in `ops`.

Everything else should remain direct, concrete code.

### Example

```c
typedef struct module_ops_t {
    int (*process)(module_t *m, int value);
} module_ops_t;
```

### Rule of thumb

- Use `ops` **sparingly and intentionally**.
- Prefer doubling **dependencies and effects** over doubling
  the module’s own logic.
- If *all* public functions are in `ops`, the module is effectively a port.

---

## 5. Callbacks

### Role

Callbacks allow the module to **emit notifications** to the outside world
(errors, diagnostics, events) without tight coupling.

Callbacks are **outbound only**.

### Example

```c
typedef struct <module>_callbacks_t {
    void (*on_error)(void *user_data, const char *msg);
} <module>_callbacks_t;
```

### Rules

- All callbacks are optional (NULL allowed).
- Always paired with `user_data`.
- No callback must be required for correct module behavior.

---

## 6. Context (ctx)

### Role

The context is a **configuration and injection input**,
**not runtime state**.

It is consumed by the module constructor (`create`, `init`, etc.)
to initialize the module instance.

### Key properties

- May be NULL.
- May be partially filled.
- Must not be modified by the module.
- May reference static const data.
- Has no lifetime beyond module creation.

Crucial rule

The module **does not store the ctx as-is**.  
It stores only the fields it actually needs in its handle.

---

## 7. Defaults

Each module may provide default factories:

```c
<module>_deps_t module_default_deps(void);
const <module>_backends_t *module_default_backends(void);
const <module>_hooks_t *module_default_hooks(void);
const <module>_ops_t *module_default_ops(void);
```

Optionally:

```c
const <module>_ctx_t *module_default_ctx(void);
```

### Rules

- Default objects are static const.
- No mutable global state.
- Thread-safe by construction.
- Defaults are applied during creation, not looked up dynamically.

---

## 8. What this model avoids

- Hidden dependencies.
- Service locators / singletons.
- `#ifdef TEST`.
- Mutable global variables.
- Over-mocking or over-virtualization.

---

## 9. What this model enables

- Explicit dependency injection.
- Fine-grained and *selective* test doubling.
- Clear architectural seams.
- Black-box testing where appropriate.
- White-box testing where valuable.
- Readable, maintainable, evolvable C code.

---

## 10. When to use this model

Recommended if the module:

- depends on external services,
- has multiple implementations or strategies,
- requires controlled test isolation.

Not required if the module:

- is trivial or purely functional,
- has no external effects,
- has a single stable implementation.

### Note on stateless / purely functional modules

Some modules in the codebase are **pure** or **stateless** by nature.

For such modules:

- No handle is created.
- No constructor or destructor is required.
- No runtime state or lifetime management exists.

Their public API typically consists of plain functions:

```c
result_t module_do_something(arg1, arg2);
```

or, when a configuration or policy is required:

```c
result_t module_do_something(const module_cfg_t *cfg, arg1, arg2);
```

In this case:

- `module_cfg_t` represents a **configuration**, not a dependency container.
- It is usually small and often `static const`.
- It may be injected into consumers when appropriate.

Important distinction  
For stateless modules, a `cfg` is **not** a dependency injection mechanism in the same sense as a `ctx`:

- it does not manage lifetimes,
- it does not carry resources or services,
- it does not imply ownership.

Guideline  
Do **not** introduce a ctx or handle for a module unless it actually needs:

- runtime state,
- external dependencies,
- interchangeable implementations,
- or controlled test isolation.

In short:

- **Stateful modules** → handle + optional ctx at creation time.
- **Stateless modules** → plain functions, optional config passed explicitly.
- **Avoid uniformity for its own sake**: architectural clarity is preferred over mechanical consistency.

---

## Conclusion

The ctx / deps / backends / hooks / ops / callbacks model defines a
**maximal architectural envelope**.

It allows everything to be made doublable,
but encourages **intentional restraint**:

Make explicit only what is architecturally meaningful to substitute.

This balances rigor and pragmatism,
and serves as a reference standard for the project’s core modules.
