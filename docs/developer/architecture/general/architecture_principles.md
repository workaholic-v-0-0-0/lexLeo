@page architecture_principles Architecture principles

# Scope

These principles apply to `lexleo_app` and the modules composed beneath it,
including ports and concrete adapters.

They define:

- authority boundaries;
- explicit dependency injection;
- ownership and destruction responsibilities;
- construction in two phases;
- controlled white-box access for testing.

Not every module needs every authority layer. The layers exposed by a module
depend on the operations that its clients require.

`lexleo_app` follows the same construction and ownership principles internally.
Its public construction API is deliberately narrower because it is the
application Composition Root and its production client is `cli`.

# Authority stack

The production authority stack is:

```text
cr -> owner -> borrower
```

White-box test access sits above these production authorities:

```text
white_box_tests_access -> cr -> owner -> borrower
```

The arrows indicate access from a higher authority to lower authorities.

Adapter-facing contracts are outside this stack. They do not introduce an
additional authority between `owner` and `cr`.

| Authority | Role |
| --- | --- |
| White-box test access | Exposes selected internal operations and handle fields through test-only wrappers, accessors, and injectors. |
| Composition Root | Builds configurations and environments, constructs handles, completes initialization, and assembles dependencies. |
| Owner | Creates handles through configured creators and destroys resources it owns. |
| Borrower | Uses existing handles without acquiring authority to construct or destroy those handles. |

## Borrower authority

Borrower APIs are used by ordinary runtime consumers.

Typical operations include:

```c
mod_status_t mod_run(mod_t *mod);
mod_status_t mod_read(mod_t *mod, void *buf, size_t len);
mod_status_t mod_write(mod_t *mod, const void *buf, size_t len);
```

A caller receiving `mod_t *` through a borrower API does not acquire ownership
of that handle.

This restriction concerns the caller's authority over `mod_t`. It does not
prevent the implementation of a borrower operation from creating and destroying
other resources needed to perform its work.

Such implementation details may depend privately on another module's owner API.

## Owner authority

Owner APIs are used by code responsible for resource lifetimes.

They provide destruction operations and, when needed, creation through
previously configured creator objects.

For example:

```c
mod_status_t mod_feature_creator_create(
    const mod_feature_creator_t *creator,
    const mod_feature_args_t *args,
    mod_t **out);
```

An owner-facing creator uses dependencies assembled by Composition Root code.
Its caller does not need access to concrete adapter construction.

## Composition Root authority

The CR API provides the highest production authority exposed by a module.

Depending on the module, it provides:

- configuration and environment types;
- default configuration and environment helpers;
- handle creation and completion of initialization;
- factory and creator construction;
- adapter registration;
- destruction operations.

Lower-level modules expose the construction services required by higher-level
Composition Root code.

# Ownership

Ownership defines who is responsible for destroying a resource.

A borrowed dependency is used without ownership transfer. Its borrower must
not destroy it, and its owner must keep it valid for the required lifetime.

An owned resource must be released by its owner when no longer needed,
including during teardown or failure cleanup.

A private handle may contain:

- borrowed dependencies supplied through its environment;
- copied configuration values;
- owned resources created during initialization or runtime;
- borrowed aliases of resources owned elsewhere.

Each distinct resource must have a clear destruction responsibility.
Multiple pointers to the same object do not imply multiple ownerships.

Destruction must respect dependency lifetimes: an object must be destroyed
before the resources that its destruction may still use.

# Common contracts and adapters

## Common layer

A port exposes dependency-neutral common contracts shared by its authorities
and concrete adapters.

Depending on the port, these include:

- opaque handle types;
- status codes;
- operation and construction argument types;
- backend callback types;
- dispatch-table types.

The common layer does not depend on concrete adapter implementations or
higher authority APIs.

## Port-adapter boundary

A port handle owns an opaque backend through a `void *`.

Its operations dispatch through callbacks that receive that backend explicitly.
For example:

```c
typedef struct stream_vtbl_t {
    stream_read_fn_t read;
    stream_write_fn_t write;
    stream_flush_fn_t flush;
    stream_close_fn_t close;
} stream_vtbl_t;
```

The port therefore does not need to know the backend's concrete type.

Concrete adapters implement these contracts. Their private representation,
configuration, construction, and runtime details remain outside the port scope.

Adapter-facing contracts are kept outside the authority stack and expose only
what is necessary to implement and connect an adapter.

## Adapter construction

Concrete adapters follow the same two-phase construction principle as other
handle-based modules.

Their CR APIs provide the services needed to construct their backends and,
where applicable, adapter providers for registration in a port factory.

Composition Root code connects the backend to the port or registers the
provider and constructs the appropriate creators.

Runtime consumers use the resulting port handles or configured creators
without depending on concrete adapter types.

# Construction in two phases

Two-phase construction applies to handle-based modules beneath `lexleo_app`
and to `lexleo_app` itself.

The purpose is to allow independent injection of:

- borrowed dependencies before handle creation;
- owned resources after handle creation and before default initialization.

Exact function signatures depend on each module's contract.

## Configuration

A configuration contains the values needed to select and configure behavior.

```c
mod_cfg_t mod_default_cfg(void);
```

Pointer fields in configurations must have explicit lifetime and ownership
contracts. Passing a configuration does not implicitly transfer ownership
of the objects it references.

## Environment

An environment contains borrowed dependencies.

For example:

```c
typedef struct mod_env_t {
    const osal_mem_ops_t *mem_ops;
    stream_t *in;
    stream_t *out;
    logger_t *logger;
} mod_env_t;
```

An environment helper constructs the value without acquiring ownership
of those dependencies.

For lower-level modules, higher-level CR code and tests can construct or adjust
the environment before creating the handle.

## Handle creation

The first phase allocates the handle and establishes its initial state.

It:

- stores borrowed dependencies;
- initializes resource fields to their documented empty state;
- stores configuration values when required by the module's API;
- allocates structural containers needed to represent or inject later resources.

It does not perform default construction of the owned runtime resources
intended to be injectable between the two phases.

Allocating the handle or an empty ownership container is therefore compatible
with this rule. For example, `lexleo_app_create()` allocates
`stream_owner_opaque` while leaving its factory and creator fields NULL.

## Completion of initialization

The second phase creates and initializes missing owned runtime resources.

It preserves resources already initialized or injected according to the
module's contract.

Some modules may also accept externally constructed resources at this stage.
Any ownership transfer must be explicit in the API contract.

This separation allows a test to:

1. create the handle;
2. inject selected owned resources;
3. complete the remaining initialization.

For example, the owned runtime resource of `fs_stream` is its `OSAL_FILE`
handle. White-box test access can inject that file between creation and
completion of initialization.

## Failure cleanup

Each construction phase defines:

- its failure statuses;
- whether output arguments remain unchanged;
- which resources are retained after failure;
- which resources are released by the failing step.

A partially initialized handle must remain valid for destruction whenever
the module's contract permits partial initialization.

A public convenience function may combine both phases and release the
temporary handle if either phase fails.

## Destruction

Destruction releases the handle and its attached owned resources.

It does not destroy borrowed dependencies.

Where specified, destruction accepts NULL and resets the caller's handle
pointer to NULL.

Calling destruction repeatedly through that same pointer variable is safe.
Calling it through another pointer that still designates an already destroyed
object is not covered by this guarantee.

# Application Composition Root

The application Composition Root resides in `lexleo_app`.

Its production client, `cli`, supplies an application configuration and uses
the public construction service:

```c
bool lexleo_app_create_init(
    lexleo_app_t **out,
    const lexleo_app_cfg_t *cfg);
```

This service creates and initializes the application in one call.

The public `cr_opaque` API exposes the configuration, default configuration
helper, combined construction service, and destruction service. It does not
expose the application's environment or its separate construction phases.

The CLI runs the resulting handle through the borrower API:

```c
lexleo_app_status_t lexleo_app_run(lexleo_app_t *app);
```

## Internal construction phases

Internally, `lexleo_app` retains separate services:

```c
lexleo_app_cr_status_t lexleo_app_create(
    lexleo_app_t **out,
    const lexleo_app_env_t *env);

lexleo_app_cr_status_t lexleo_app_complete_default_init(
    lexleo_app_t *app,
    const lexleo_app_cfg_t *cfg);
```

Thus, `lexleo_app` does not depart from the two-phase construction principle.
It hides that decomposition from its production client.

## Test access

The test-only library
`lexleo_app_white_box_tests_access_cr_wrapper`
provides access to the internal construction services.

Its wrappers allow tests to supply an environment with test doubles and
invoke creation separately from completion of initialization. They translate
internal statuses into test-facing statuses.

The white-box access helpers allow tests to inspect the handle and inject
selected owned resources between these phases.

Consequently, unit tests can replace borrowed dependencies and owned resources
without exposing internal composition details to the CLI.

# Module organization

Authority boundaries should be visible in public headers, build targets,
and implementation organization.

A typical module may contain:

```text
mod/include/
    common/mod/common/
    borrowers/mod/borrowers/
    owners/mod/owners/
    adapters/mod/adapters/
    cr/mod/cr/
```

Only the directories required by the module are present.

`lexleo_app` exposes `cr_opaque` instead of its internal CR services.

Private handle definitions and internal helper declarations remain under
`src/internal/`. Production clients must not include these private headers.

White-box access implementations may include private headers. Their public
test headers should expose only the types and operations needed by tests.

# Build dependencies

Build targets should express the authority boundaries and actual dependencies.

For a module exposing all three production authorities, the public dependency
chain may be:

```cmake
target_link_libraries(mod_owner PUBLIC mod_borrower)
target_link_libraries(mod_cr PUBLIC mod_owner)
```

There is no mandatory adapter target between `owner` and `cr`.

A dependency is PUBLIC when its usage requirements are part of the target's
public interface. A dependency used only by the implementation is PRIVATE.

In particular, a borrower implementation may privately use another module's
owner API:

```cmake
target_link_libraries(
    lexleo_app_borrower
    PRIVATE compiler_borrower
)

target_link_libraries(
    compiler_borrower
    PRIVATE ast_owner
)
```

This allows the compiler implementation to create and destroy AST objects
without exposing AST ownership operations through its borrower interface.

Under this project's authority policy, a borrower target must not publicly
expose another module's owner API:

```cmake
# Forbidden by the project's authority policy:
target_link_libraries(mod_borrower PUBLIC dep_owner)

# Permitted when required by the implementation:
target_link_libraries(mod_borrower PRIVATE dep_owner)
```

Private implementation directories must remain PRIVATE include directories.

Common headers may be exposed where required by public declarations without
exposing higher authority APIs.

# Testing

Tests use the construction phases to control resource creation.

A typical unit test can:

1. construct the required configuration and environment;
2. substitute borrowed dependencies with test doubles;
3. create the handle;
4. inspect its initial state;
5. inject selected owned resources through white-box helpers;
6. complete initialization;
7. verify that injected resources were preserved;
8. exercise the required behavior;
9. destroy the handle and verify ownership and cleanup.

For `lexleo_app`, internal construction is accessed through the CR wrapper
library rather than through its production `cr_opaque` API.

Integration tests may instead call the combined public construction service
with production dependencies.

# Validation policy

Validation distinguishes recoverable invalid arguments from programming
preconditions.

Conditions documented as invalid arguments must be checked and reported
through the API's error mechanism.

Conditions documented as preconditions may be enforced with
`LEXLEO_ASSERT()`.

Project-defined operation and dispatch tables must be constructed according
to their contracts. Their designated construction helpers validate the fields
for which they are responsible.

Consumers may then rely on the documented well-formedness preconditions.
Arguments merely forwarded to another API need not be deeply revalidated
at every layer.

A violated `LEXLEO_ASSERT()` indicates a programming error. It is not
a recoverable runtime status or a replacement for validating external input.
