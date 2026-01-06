@page arch_standards_types Type standards

# Type standards

This document defines the standard structure and responsibilities of
**configuration, context, state, and wiring-related types**
used across the project.

The purpose is to clearly distinguish:

- value-level configuration,
- wiring / dependency injection,
- runtime execution and ownership,
- mutable execution state,
- extension and interaction points,

and to prevent accidental mixing of these concerns.

---

## Module handle (runtime object)

Suffix: <module>_t

The <module>_t type is the primary runtime handle of a module.

It represents a constructed, executable instance of the module,
after all wiring decisions have been resolved.

A module handle:

- is created via a factory API (directly or indirectly);
- represents the living module during execution;
- owns the module’s runtime state;
- stores the injected dependencies it needs at runtime;
- is responsible for releasing what it owns.

A module handle:

- is instantiated under the control of the bootstrap (directly or indirectly);
- may be created via a factory API, but lifecycle orchestration remains a
  bootstrap/teardown responsibility;
- represents the living module during execution;
- owns and manipulates its runtime state;
- holds references to injected dependencies required at runtime;
- releases only the resources it explicitly owns, typically when instructed
  by the teardown phase.

Typical shapes (illustrative, not normative):

A) Core module handle

- Contains runtime state and references to injected services.
- May embed <module>_state_t directly or reference it.

```c
typedef struct <core>_t {
  <core>_state_t state; /* mutable runtime state */
  const <core>_ops_t      *ops;
  const <dep1>_ops_t      *dep1_ops;
  const <dep2>_ops_t      *dep2_ops;
  const osal_mem_ops_t    *mem; /* OSAL services */
} <core>_t;
```

B) Port handle (port/adapter boundary)

- Stores a port-owned contract (vtbl) and an opaque backend instance.
- destroy is part of the port contract.

```c
typedef struct <port>_t {
  const <port>_vtbl_t     *vtbl; /* port contract */
  void                    *backend; /* adapter-owned backend */
  const osal_mem_ops_t    *mem; /* OSAL services (teardown-safe) */
} <port>_t;
```

C) Adapter handle (backend implementation)

- Concrete type, private to the adapter module.
- Typically owns adapter state and resources required to implement the port.

```c
typedef struct <adapter>_t {
  <adapter>_state_t state;
  const osal_mem_ops_t    *mem; /* OSAL services */
} <adapter>_t;
```

Mental model:

_ctx_t builds the module; <module>_t is the module.


---

## Module configuration

Suffix: \c *_cfg_t

Configuration types define **static, value-only parameters** that describe
how a module behaves from a functional or policy point of view.

A configuration:

- contains **values only** (flags, limits, modes, options);
- contains **no pointers to functions**;
- contains **no references to other modules**;
- is **immutable by design**;
- is typically **copied by value** or passed as `const`.

Configuration expresses *what the module is allowed or configured to do*,
not *what it depends on*.

Typical use cases:

- feature flags;
- operating modes;
- size or depth limits;
- thresholds;
- validation or policy parameters.

A module may have **no configuration at all** if it does not need it.

---

## Module state

Suffix: \c *_state_t

State types define the **mutable execution state** of a module instance.

A state:

- is **mutable**;
- represents data that **evolves during execution**;
- is **private to a single module instance**;
- must never be shared across concurrent executions.

State objects typically hold:

- counters;
- cursors / iterators;
- temporary buffers;
- execution progress;
- cached intermediate results.

State types:

- must live in `src/internal/`;
- must never be exposed as part of the public API;
- are owned by the module handle (`module_t`).

---

## Dependency interfaces (services / capabilities)

Suffix: `*_ops_t`

Dependency interfaces define **abstract capability contracts**
that a module relies on to interact with other modules or services.

They are expressed as **operation tables** (function pointer vtables).

Important distinction:

> Dependency interfaces (`*_ops_t`) describe **capabilities**  
> Port virtual tables (`*_vtbl_t`) describe **executable backends**

Use `*_ops_t` when the dependency:

- does not require a backend instance;
- does not manage a lifetime;
- represents a service or policy.

Use `*_vtbl_t` instead when the dependency is a
**runtime execution backend** selected via a port.

Rules:

- ops tables are **immutable**;
- ops tables must not carry state;
- ops tables describe *what is possible*, not *how it is implemented*;
- implementations are selected by the bootstrap.

---

## Aggregated dependencies

Suffix: \c *_deps_t

Dependency aggregates group **all external dependencies required by a module**
into a single structure.

Example:

```c
typedef struct <module>_deps_t {
    const <module>_<dep1>_ops_t *dep1;
    const <module>_<dep2>_ops_t *dep2;
} <module>_deps_t;
```

This structure represents **what the module depends on**, explicitly.

Rules:

- dependencies are injected, never discovered;
- no global lookups or service locators;
- ownership and lifetime are managed by the bootstrap.

---

## Callbacks (outbound notifications)

Suffix: \c *_callbacks_t

Callbacks define **outbound notifications** emitted by a module toward
its embedding environment.

Example:

    typedef struct <module>_callbacks_t {
        void (*on_error)(void *user_data, const char *msg);
    } <module>_callbacks_t;

Rules:

- callbacks are always optional;
- callbacks must never be required for correctness;
- callbacks are paired with `user_data`;
- callbacks must not drive core control flow.

Callbacks represent *observation*, not *control*.

---

## Module context (wiring object)

Suffix: `*_ctx_t`

The context defines the **wiring contract** of a module.

It aggregates everything that is injected **at construction time only**,
and expresses **how the module is assembled**, not how it executes.

It may aggregate:

- dependencies (`*_deps_t`);
- optional internal strategies (`*_ops_t`);
- outbound notifications (`*_callbacks_t`);
- **port virtual tables (`*_vtbl_t`) selecting adapter implementations (including teardown semantics)**;
- an optional `user_data` pointer.

The context represents the **upper bound of what can be injected**.
A real module is expected to include **only the fields it actually needs**.

Example (maximal form):

```c
typedef struct <module>_ctx_t {
    <module>_deps_t        deps;        // runtime dependencies
    const <module>_ops_t  *ops;         // optional internal strategies
    const <module>_vtbl_t *vtbl;        // port / adapter selection
    <module>_callbacks_t  callbacks;   // outbound notifications
    void                 *user_data;   // caller-owned opaque state
} <module>_ctx_t;
```

Key properties:

- the ctx is **logically immutable**;
- the ctx is **consumed only at construction time**;
- the ctx is **not runtime state**;
- the module must **not store the ctx as-is**;
- only selected fields are copied or referenced into the module handle.

Architectural rule:

> The context exists **only for wiring**.
> It expresses *how the module is connected*, not *how it behaves internally*.

In the project architecture:

- the **bootstrap is the primary (often sole) creator and user of ctx objects**;
- it is responsible for **selecting implementations and wiring dependencies**;
- the **teardown phase is centralized** (e.g. `teardown.c`) and is responsible for
  releasing all resources created or wired during bootstrap
  (module instances, backends, adapters, OSAL resources);
- individual modules **do not perform global teardown** and only release
  the resources they explicitly own via their injected `destroy` operations;
- regular module consumers must never depend on ctx types.

For this reason, ctx types typically live in:

- `include/internal/<module>_ctx.h`

and are exposed via CMake **only** to the bootstrap and test targets.

---

## Default providers

Modules may expose default providers to reduce boilerplate.

Examples:

```c
const <module>_ops_t *module_default_ops(void);

/* wiring helper: builds a ctx and applies fallbacks if inputs are NULL */
<module>_ctx_t module_default_ctx(
    const <dep>_ops_t *dep_ops,
    const osal_mem_ops_t *mem_ops
    /* ... */
);
```

Defaults:

- must be static or logically immutable;
- must not rely on mutable global state;
- are applied during construction, never via late lookup.

---

## Virtual tables (ports & adapters)

Suffix: `*_vtbl_t`

Virtual tables define **runtime callable interfaces** used to connect
a **port abstraction** to a **concrete backend implementation**.

They are the primary mechanism used to implement **ports / adapters**
in a low-level, explicit, and testable way.

A virtual table:

- is a table of function pointers;
- represents a **call contract** between a core module (port)
  and an adapter (backend);
- is **immutable**;
- carries **no state**;
- is typically injected via a context (`*_ctx_t`).

### Typical example: lexer port

```c
typedef lexer_next_rc_t (*lexer_next_fn_t)(
    void *backend,
    struct lexleo_token_t *out );

typedef void (*lexer_destroy_fn_t)(void *backend);

typedef struct lexer_vtbl_t {
    lexer_next_fn_t    next;
    lexer_destroy_fn_t destroy;
} lexer_vtbl_t;
```

In this model:

- the port (e.g. lexer) owns the contract (lexer_vtbl_t);
- the adapter (e.g. lexleo_flex) provides:
    - a concrete backend object;
    - an implementation of the vtable functions.

The port:

- does **not** know the concrete backend type;
- stores only:
    - an opaque backend pointer (`void *backend`);
    - a port-owned vtable (`const lexer_vtbl_t *vtbl`);
    - injected OSAL services required for teardown or allocation (e.g. `const osal_mem_ops_t *mem`).

Example:

```c
typedef struct lexer_t {
    const lexer_vtbl_t *vtbl;     // port contract
    void *backend;                // adapter-owned instance
    const osal_mem_ops_t *mem;    // OSAL dependency (lifetime-safe teardown)
} lexer_t;
```

### Relationship with `_ops_t`

Although both `_ops_t` and `_vtbl_t` are tables of function pointers,
they serve **distinct architectural roles**.

| Type      | Purpose                             | Typical usage                       |
|-----------|-------------------------------------|-------------------------------------|
| `_ops_t`  | Dependency interface (capabilities) | Services, policies, dependencies    |
| `_vtbl_t` | Port / adapter call contract        | Ports with interchangeable backends |

## Guidelines

- use `*_ops_t` when:
    - expressing **replaceable operations** (capabilities, strategies, or services);
    - the operations may belong to:
        - a dependency (external service or module), **or**
        - the module itself (internal strategies or policies);
    - the caller knows the **handle type** (even if the handle is opaque);
    - calls are made on a **typed pointer** (`struct foo *`);
    - no opaque backend indirection is involved;
    - no lifetime management is attached to the operation table itself.

- use `*_vtbl_t` when:
    - implementing a **port / adapter pattern**;
    - the caller stores an **opaque backend instance** (`void *`);
    - the concrete backend type must remain **completely unknown** to the caller;
    - behavior is **bound to a specific backend implementation**;
    - explicit **lifetime management** (`destroy`) is part of the contract.

In short:

> `_ops_t` expresses **replaceable operations on a known (possibly opaque) handle**  
> `_vtbl_t` expresses **callable behavior bound to an unknown backend instance**

---

## Factory type

Some modules require dynamic runtime instance management (create/destroy multiple
instances during execution). In LexLeo, this must be done via a factory
object that is wired once at bootstrap time and then used at runtime.

### `module_factory_t`

`module_factory_t` is an opaque handle representing a VM-owned runtime service that
creates and destroys instances of a given family of objects.

Rules:

- `module_factory_t` is typically forward-declared in public headers as an opaque type:
  `typedef struct module_factory_t module_factory_t;`
- its concrete definition must remain private to the module implementation
  (e.g. in `src/internal/`)
- the factory instance is created/destroyed by the composition root
  (wiring-time lifecycle, internal API)
- runtime modules may hold a pointer/reference to the factory, but do not own it

### Factory runtime operations (acquire/release)

Factories only expose the two following runtime operations:

- `create`
- `destroy`

These operations are runtime-safe and must not require a `*_ctx_t`.

### Factory ops table (optional)

A factory may expose its runtime API through an ops table:

- `module_factory_ops_t` (function pointers)
- `module_factory_handle_t` (self + ops)

This follows the standard `ops/self` pattern used across LexLeo modules and
supports easy substitution in tests.

---

## Summary

 Suffix        | Role                                                     | Mutability | Scope                  
---------------|----------------------------------------------------------|------------|------------------------
 \<module\>_t  | Runtime module handle (executable instance)              | mutable    | public / internal      
 *_cfg_t       | Value-only configuration                                 | immutable  | public                 
 *_state_t     | Runtime execution state                                  | mutable    | internal only          
 *_ops_t       | Replaceable operations (services or internal strategies) | immutable  | public / internal      
 *_vtbl_t      | Port / adapter call contract (opaque backend)            | immutable  | internal / wiring      
 *_deps_t      | Aggregated injected dependencies                         | immutable  | wiring                 
 *_callbacks_t | Outbound notifications (observability only)              | immutable  | wiring only            
 *_ctx_t       | Wiring / dependency injection object                     | immutable  | bootstrap / tests only 

This type model enforces a strict separation between:

- **configuration** (`*_cfg_t`);
- **wiring and assembly** (`*_ctx_t`, `*_deps_t`, `*_vtbl_t`);
- **execution and behavior** (`*_state_t`, concrete implementations).

It enables explicit dependency injection, controlled substitution,
and clear architectural boundaries, while avoiding hidden dependencies
and over-virtualization.
