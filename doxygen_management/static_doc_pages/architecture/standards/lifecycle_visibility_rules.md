@page lifecycle_visibility_rules Lifecycle Visibility Rules (create / destroy)

# Lifecycle visibility rule (create/destroy)

In LexLeo, any API that performs wiring (dependency injection, strategy selection,
adapter binding) is owned by the composition root (bootstrap/teardown).

Therefore, lifecycle entry points that consume wiring inputs (typically a `*_ctx_t`)
are internal (non-public API; typically declared under `include/internal/`) and
only exposed to:

- bootstrap / teardown targets
- tests acting as alternative composition roots

Runtime code must not perform wiring (i.e., it must not call entry points that
consume a `*_ctx_t`).

---

## Exception — runtime dynamic creation (factory)

If multiple instances must be created/destroyed during execution, runtime code
must use a factory that was wired once at bootstrap time.

Rules:

- the factory instance is VM-owned (created/destroyed by the composition root);
- runtime modules receive it via DI as an opaque service object;
- runtime instance creation/destruction uses ctx-free runtime operations
  (e.g. create/destroy) that do **not** require a `*_ctx_t`.

Public headers may expose **runtime** creation interfaces (ctx-free) when needed.
Public headers must not expose **wiring-time** lifecycle entry points (ctx-consuming).

---

## Factory interface shape (ops + userdata)

Factories are typically injected as an `ops + userdata` pair.

Example:

    // include/module_factory.h
    typedef struct module module_t;

    typedef struct {
        module_status_t (*create)(void *userdata, module_t **out);
        void (*destroy)(void *userdata, module_t *obj);
    } module_factory_ops_t;

    typedef struct module_factory {
        void *userdata;
        const module_factory_ops_t *ops;
    } module_factory_t;

Semantics:

- `module_factory_t` is the runtime service object injected into consumers.
- `userdata` is an opaque pointer owned by the concrete factory implementation.
  Consumers must not interpret it; they only pass it back to the ops.
- `ops` is the virtual table of ctx-free runtime operations.
- `create` / `destroy` manage *instances* at runtime; they are distinct from
  wiring-time lifecycle entry points (which consume a `*_ctx_t` and remain internal).

---

## Ops table lifetime rule (vm-owned / static)

All `*_ops_t` tables (including factories, ports, and OSAL operation tables such as
`osal_mem_ops_t`) follow the same lifetime rule:

- `const *_ops_t *ops` must always point to a lifetime-stable object.
- The pointed ops table must outlive every runtime object that references it.
- Acceptable storage durations are:
  - static storage (e.g. `static const *_ops_t OPS = {...};`)
  - VM-owned storage (allocated/initialized by the composition root and destroyed
    at VM teardown)

**Forbidden**: passing an ops table allocated on the stack (temporary local variable).

Rationale:
- runtime objects must never depend on wiring stack frames;
- ops pointers must remain valid for the entire execution lifetime of the VM.

---

## Allocator retention rule (destroy must not depend on ctx)

Any runtime object that performs allocations must retain the allocator used to
allocate its owned resources.

Rule:

- If a runtime object allocates memory using `mem_ops`, then the runtime object
  must store `const osal_mem_ops_t *mem_ops` (or an equivalent allocator handle)
  so that destruction can be performed later without requiring a `*_ctx_t`.

This applies to:
- stream backends (adapter-owned state)
- factory userdata blocks (if heap-allocated)
- any heap-allocated runtime state owned by an instance

Therefore:

- `destroy` / `close` operations must be **ctx-free**.
- teardown must never require access to wiring-time contexts (`*_ctx_t`).

Rationale:
- creation consumes wiring inputs; runtime destruction must remain safe and local;
- ownership is explicit: the object that allocates owns and frees;
- tests can inject custom allocators and still validate memory correctness.

---
