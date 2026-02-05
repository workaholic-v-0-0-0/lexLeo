@page lifecycle_visibility_rules Lifecycle & Visibility Rules (Composition Root / Runtime)

# Lifecycle & Visibility Rules (Composition Root vs Runtime)

LexLeo strictly separates:

- **Composition-Root-time (CR-time)** operations
- **Runtime** operations

CR-time includes:
- dependency injection
- backend selection
- adapter registration
- strategy binding
- factory wiring

Runtime code must never perform CR-time operations.

Any function consuming a `*_ctx_t` is considered a **CR-time entry point**.

================================================================
1) Knowledge hierarchy inside a module
   ================================================================

Each module exposes multiple visibility tiers.

Tier 1 — Borrowers (`include/<mod>/borrowers/`)
------------------------------------------------
- Runtime usage only
- No ownership responsibilities
- Must not create or destroy handles
- Must not perform CR-time operations

Tier 2 — Owners (`include/<mod>/owners/`)
------------------------------------------------
- Runtime lifecycle management
- May create and destroy handles
- Creation must occur via a factory injected at CR-time
- Creation/destruction must be ctx-free
- Must not perform CR-time registration or binding

Tier 3 — Adapters (`include/<mod>/adapters/`)
------------------------------------------------
- Used only by module adapter implementations.
- May use the port’s adapter-only handle constructor
  (function that initializes a port handle with backend + ops/vtbl).
- Must implement the standardized constructor signature
  used by the module factory.
- Must not expose CR-time configuration APIs.

Tier 4 — Composition Root (`include/<mod>/cr/`)
------------------------------------------------
- CR-only headers
- May consume `*_ctx_t`
- Responsible for assembling, configuring, and binding modules
- The only public location where CR-time entry points may appear

Tier 5 — Internal (`include/internal/` and `src/internal/`)
------------------------------------------------
- Private to the module implementation
- Not exported as public include directories
- May be used by whitebox tests explicitly

Dependency Rule:
Each tier may depend only on itself and lower tiers.
`cr/` headers are CR-only.
`internal/` headers are module-private.

================================================================
2) CR-time lifecycle rule (ctx-consuming entry points)
   ================================================================

Not all `*_ctx_t` are CR-only.

LexLeo distinguishes two kinds of contexts:

A) CR-time contexts (Composition Root inputs)
---------------------------------------------
A context is CR-time when it is used to assemble/configure the system:
- adapter registration / binding
- backend selection
- factory wiring
- strategy binding

Rules:
- CR-time entry points must live under `include/<mod>/cr/` OR be internal.
- They may only be called by the Composition Root (bootstrap/teardown)
  or by tests acting as alternative composition roots.
- Runtime modules must never call CR-time entry points.

B) Adapter-only port contexts (runtime dependencies for handle construction)
---------------------------------------------------------------------------
A context may be required by adapters to construct a valid port handle
(e.g. providing mem ops / time ops needed by the port implementation).

Rules:
- Such ctx-consuming entry points may live under `include/<mod>/adapters/`.
- They may only be called by module adapters (backend implementations).
- Owners/borrowers must never see or use these entry points directly.

Summary:
- `*_ctx_t` is CR-only only when it represents CR-time assembly inputs.
- `*_ctx_t` used solely for port handle construction is adapter-visible
  and restricted to `adapters/`.


================================================================
3) Runtime lifecycle rule (ctx-free creation/destruction)
   ================================================================

Runtime creation and destruction must be ctx-free.

Rules:

- `destroy` / `close` must never require a `*_ctx_t`.
- Runtime lifecycle must not depend on CR stack frames.
- Any object created at runtime must retain what it needs to destroy itself safely.

================================================================
4) Runtime dynamic creation via factory
   ================================================================

If multiple instances must be created/destroyed during execution:

- The Composition Root wires a factory once at bootstrap time.
- The factory is injected as a runtime service object.
- Runtime creation/destruction remains ctx-free.

Two valid runtime factory shapes:

A) Handle-style factory (stateful)
-----------------------------------
Injected as an opaque handle:
stream_factory_t *factory;

Runtime calls:
stream_factory_create_stream(factory, key, args, &out);

B) Ops + userdata service object (lightweight)
-----------------------------------------------
    typedef struct module module_t;

    typedef struct {
        module_status_t (*create)(void *ud, module_t **out);
        void (*destroy)(void *ud, module_t *obj);
    } module_factory_ops_t;

    typedef struct {
        void *ud;
        const module_factory_ops_t *ops;
    } module_factory_t;

Semantics:
- The factory is wired once at CR-time.
- Runtime code only uses ctx-free operations.
- `userdata` is opaque and owned by the concrete implementation.

================================================================
5) Ops table lifetime rule (lifetime-stable)
   ================================================================

All `*_ops_t` tables (ports, factories, OSAL ops, etc.) must be lifetime-stable.

Rule:

- `const *_ops_t *ops` must point to an object that outlives all runtime users.
- Acceptable storage durations:
  - static storage (`static const *_ops_t OPS = {...};`)
  - VM-owned storage allocated by the Composition Root

Forbidden:

- stack-allocated ops tables
- temporary lifetime ops structures

Rationale:
Runtime objects must never depend on CR stack frames.

================================================================
6) Allocator retention rule (destroy must not depend on ctx)
   ================================================================

Any runtime object performing allocations must retain the allocator used.

Rule:

- If an object allocates with `mem_ops`,
  it must store `const osal_mem_ops_t *mem_ops`
  (or equivalent allocator handle).

This applies to:

- adapter backend state
- factory userdata blocks
- heap-allocated runtime state

Therefore:

- destruction must remain ctx-free
- teardown must not require CR-time contexts
- ownership must be explicit and local

================================================================
Summary Principle
================================================================

CR-time configures.
Runtime executes.
Destruction is always local and ctx-free.
Visibility strictly follows the knowledge hierarchy.
