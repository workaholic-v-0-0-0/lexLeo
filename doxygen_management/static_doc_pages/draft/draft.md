@page module_context_model Module context model

# Module Context Model (deps / backends / callbacks)

## Purpose

This document describes the context-based architecture (ctx) adopted in the project
to structure dependencies, interchangeable implementations (backends), and callbacks
in a modular, testable, and decoupled C codebase.

This model is designed to:

- apply SOLID principles in practice (especially DIP and ISP),
- avoid hidden dependencies,
- allow controlled dependency injection (tests, variants),
- keep the public API minimal and stable.

---

## Overview

Each module may define an optional context (<module>_ctx_t) that aggregates:

- deps: what the module depends on (incoming ports),
- backends: interchangeable implementations the module uses,
- callbacks: optional outward notifications,
- user_data: opaque caller-owned state.

Typical shape of a context:

```c
typedef struct <module>_ctx_t {
    <module>_deps_t deps;
    const <module>_backends_t *backends;
    const <module>_ops_t ops;
    <module>_callbacks_t callbacks;
    void *user_data;
} <module>_ctx_t;
```

The ctx is optional: if NULL, the module falls back to its default configuration.

---

## 1. Dependencies (deps)

### Role

`deps` represents **explicit dependencies on other internal project modules**.

In particular, **all interactions with the outside world go through OSAL**.
OSAL is treated as a regular internal module, but it is the **only layer allowed**
to access libc or OS-level facilities.

As a consequence, project modules:

- never call `malloc`, `free`, or other libc functions directly,
- never access OS services implicitly,
- rely exclusively on OSAL public APIs (such as `osal_mem`).

These dependencies are made explicit at the architectural level, even when they
are consumed through direct function calls rather than injected interfaces.

This keeps cross-module coupling visible, controlled, and auditable.

### Conceptual example (OSAL memory dependency)

```c
// Dependency toward the OSAL memory module
#include "osal_mem.h"

typedef struct module_deps_t {
    /* This module depends on OSAL for all memory allocation */
    /* No direct use of malloc/free is allowed */
    int unused; /* placeholder: dependency is expressed by module usage */
} module_deps_t;

/* Example usage inside the module */
void *module_create_object(size_t size)
{
    return osal_malloc(size);
}
```

### Design choice

- deps are stored by value in the context.
- Rationale:
    - structures are usually small,
    - no lifetime management issues,
    - easy partial initialization.

---

## 2. Backends

### Role

Backends are interchangeable implementations of a given concept, for example:

- stdio / fs / dynamic_buffer,
- flex / fake_lexer,
- real_backend / trace_backend.

They are expressed via immutable vtables (*_ops_t).

### Adopted model

typedef struct <module>_backends_t {
const <module>_<backend_1>_ops_t *<backend_1>;
const <module>_<backend_2>_ops_t *<backend_2>;
} <module>_backends_t;

And in the context:

const <module>_backends_t *backends;

### Rationale

- Vtables are immutable, hence const pointers.
- No copying of vtables.
- Explicit selection of implementations.
- Clean injection of alternative backends in tests.

---

## 3. Callbacks

### Role

Callbacks allow the module to notify the outside world:
errors, logs, events, without creating tight coupling.

Conceptual example:

typedef struct <module>_callbacks_t {
void (*on_error)(void *user_data, const char *msg);
} <module>_callbacks_t;

### Rules

- All callbacks are optional (NULL allowed).
- Always paired with user_data.
- No callback must be required for correct module behavior.

---

## 4. Context (ctx)

### Role

The ctx is a configuration and injection aggregate,
never internal mutable state.

### Key properties

- May be NULL.
- May be partially filled.
- Must not be modified by the module.
- May point to static const data.

---

## 5. Defaults

Each module provides default factories:

<module>_deps_t module_default_deps(void);
const <module>_backends_t *module_default_backends(void);
const <module>_ctx_t *module_default_ctx(void);

### Rules

- module_default_ctx() returns a pointer to a static const.
- No mutable global state.
- Thread-safe by construction.

### Usage conventions

- ctx == NULL → use module_default_ctx().
- ctx->backends == NULL → use module_default_backends().
- callback == NULL → no callback invoked.

---

## 6. Why this model?

### What we avoid

- Hidden dependencies.
- #ifdef TEST.
- Mutable global variables.
- Direct coupling to implementations.
- Rigid APIs that are hard to test.

### What we gain

- Explicit dependency injection.
- Testability without linking real implementations.
- Clean backend substitution.
- Readable and evolvable architecture.
- Strong decoupling between modules.

---

## 7. When to use this model

Recommended if the module:

- has multiple possible implementations,
- depends on external services,
- requires fine-grained testing.

Unnecessary if the module:

- is trivial,
- has a single fixed implementation,
- depends on no external services.

---

## Conclusion

The ctx / deps / backends / callbacks model is an explicit architectural choice
balancing rigor and pragmatism in C.

It is not meant to be used everywhere,
but serves as a reference standard for the project’s core modules.
