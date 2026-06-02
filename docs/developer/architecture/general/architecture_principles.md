@page architecture_principles Architecture principles

# Authority stack

A module `mod` is decomposed into authority layers.

Each layer `lay` has:
- its own public headers under `mod/include/lay/mod/lay/`;
- its own implementation `mod/src/lay/`;
- its own build target `mod_lay`.

Not every module exposes every authority layer.

| Authority layer | Build target                 | Public headers | Role                                                                                                                                                                                                                                                                                                            |
|---|------------------------------|---|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| White-box test access | `mod_white_box_tests_access` | `mod/tests/support/white_box_tests_access/include/` | White-box tests only. Exposes internal functions and handle's fields through controlled accessors/injectors.                                                                                                                                                                                                    |
| Composition Root | `mod_cr` | `mod/include/cr/mod/cr/` | Exposes the highest production authority for explicit wiring.<br>Provides `*_cfg_t`, `*_env_t`, default helpers, factories, creators, and adapter registration when required by the module.<br>Creates top-level module handles when applicable; later runtime creations must go through owner-facing creators. |
| Adapter | `mod_adapter`                | `mod/include/adapters/mod/adapters/` | Port modules only. Binds concrete backends to a port contract.                                                                                                                                                                                                                                                  |
| Owner | `mod_owner`                  | `mod/include/owners/mod/owners/` | Owns resources and creates module handles through owner-facing creators.                                                                                                                                                                                                                                        |
| Borrower | `mod_borrower`               | `mod/include/borrowers/mod/borrowers/` | Uses existing module handles without owning or constructing them.                                                                                                                                                                                                                                               |

## Ownership

Ownership defines who is responsible for destroying a resource.

A borrowed dependency is used without ownership transfer. The borrower must not
destroy it.

An owned resource is part of the owning handle lifetime. It must be released by
the owner destroy path.

For handle-based modules, the private handle may therefore contain both:

* borrowed dependencies injected at creation time;
* owned resources created later by default initialization.

## Explicit Composition Root

A Composition Root is the place where dependencies are assembled.

It creates configuration and environment values, injects borrowed dependencies,
creates handles, initializes owned resources, registers adapters in factory, and 
wires creators into factory.

The top-level application Composition Root lives in the high-level module
`lexleo_app`. So it is the only module which does not expose its composition root 
authority layer. Lower-level modules must expose their own CR API so that a
higher-level CR can assemble them explicitly.

Note: Handle constructions are not atomic in order to afford owned dependency 
injection.

## Borrower authority

Borrower APIs are used by ordinary runtime consumers.

A borrower can use an existing handle but cannot create it, destroy it, or
complete its wiring.

Typical borrower operations are:

```c
mod_status_t mod_run(mod_t *mod);
mod_status_t mod_read(mod_t *mod, void *buf, size_t len);
mod_status_t mod_write(mod_t *mod, const void *buf, size_t len);
```

For handle-based modules, a borrower receives a `mod_t *` whose ownership stays
elsewhere.

If another module borrows `mod_t`, then either:

* it receives `mod_t *` through its environment; or
* its own private handle stores a borrowed `mod_t *` initialized by CR code.

## Owner authority

Owner APIs are used by code that owns resources.

An owner may destroy and create handles through owner-facing creators.

Typical owner-facing creation goes through a creator object:

```c
mod_status_t mod_feature_creator_create(
    const mod_feature_creator_t *creator,
    arg_1,
    arg_2,
    ...,
    mod_t **out);
```

## Adapter authority

Adapter authority only exists for port modules.

A port defines an abstraction. An adapter binds a concrete backend to that
abstraction.

For example, stream port may expose adapter-facing types such as:

```c
/* Functions that a stream adapter must implement. */
typedef struct stream_vtbl_t {

	/** Backend read operation. Must not be `NULL`. */
	stream_read_fn_t read;

	/** Backend write operation. Must not be `NULL`. */
	stream_write_fn_t write;

	/** Backend flush operation. Must not be `NULL`. */
	stream_flush_fn_t flush;

	/** Backend close operation. Must not be `NULL`. */
	stream_close_fn_t close;

} stream_vtbl_t;

/* Structure that an adapter must provide to cr so that it can 
 * register it into a factory. */
typedef struct stream_adapter_desc_t {

	/** Adapter registration key. */
	stream_key_t key;

	/** Adapter constructor. */
	stream_ctor_fn_t ctor;

	/** Optional constructor user data. */
	const void *ud;

	/** Optional destructor for `ud`. */
	ud_dtor_fn_t ud_dtor;

} stream_adapter_desc_t;

typedef stream_status_t (*stream_ctor_fn_t)(
	const void *ud,
	const void *args,
	stream_t **out);

typedef struct stream_file_creator_args_t {

	/**
	 * @brief UTF-8 path of the target file.
	 */
	const char *path;

	/**
	 * @brief Portable OSAL file open mode.
	 *
	 * @details
	 * This string is forwarded to `osal_file_ops_t::open()`.
	 * Supported values are defined by the `osal_file` contract,
	 * currently `"rb"`, `"wb"`, and `"ab"`.
	 */
	const char *mode;

} stream_file_creator_args_t;

/* Some other stream_*_creator_args_t types defined in stream/owners/creators/ */
```

An adapter can provide to cr:
- a function to build port handle with its backend;
- a function to build a `mod_adapter_desc_t` so that cr can register it
  into factory.

For example, the adapter fs_stream of the stream port exposes:

```c
stream_status_t fs_stream_create_stream(
	stream_t **out,
	const stream_file_creator_args_t *args,
	const fs_stream_cfg_t *cfg,
	const fs_stream_env_t *env);

stream_status_t fs_stream_create_desc(
	stream_adapter_desc_t *out,
	stream_key_t key,
	const fs_stream_cfg_t *cfg,
	const fs_stream_env_t *env,
	const osal_mem_ops_t *mem);
```

## Composition Root authority

The CR API is the highest module-level authority (in production).

It can create configuration values, build environments, create handles, register
adapters into port factory, construct factories and creators, complete default initialization,
and destroy owned resources.

Typical CR APIs include:

```c
mod_cfg_t mod_default_cfg(void);

mod_env_t mod_default_env(...);

mod_status_t mod_create(
    mod_t **out,
    const mod_cfg_t *cfg,
    const mod_env_t *env);

mod_status_t mod_complete_default_init(mod_t *mod);

void mod_destroy(mod_t **mod);
```

# Module decomposition

A module should be physically organized so that authority is visible in the
filesystem, build targets, and include paths.

## Public include layout

A typical module may expose:

```text
mod/include/
├── borrowers/mod/borrowers/
│   ├── mod.h
│   └── mod_types.h
├── owners/mod/owners/
│   ├── mod_owners_api.h
│   └── ...
├── adapters/mod/adapters/
│   ├── mod_adapters_api.h
│   ├── mod_adapters_types.h
│   └── mod_adapters_vtbl.h
└── cr/mod/cr/
    ├── mod_cr_api.h
    └── ...
```

Not every module needs every layer. For example, a non-port module does not 
have an adapter API and a high-level top-level CR module may also choose not
to split itself into all authority submodules when its only consumer is the 
presentation module cli.

## Source layout

Source files should also follow the authority split when the module needs it:

```text
mod/src/
├── mod_borrower.c
├── mod_owner.c
├── mod_adapter.c
├── mod_cr.c
└── internal/
    └── mod_handle.h
```

Private handles and private helpers (that we want to white-box test) 
stay under `src/internal/`.

Public clients must not include private headers.

## CMake targets

The CMake decomposition should match authority layers.

Typical targets:

```text
mod_borrower
mod_owner
mod_adapter
mod_cr
```

The expected dependency direction is upward:

```text
mod_owner   PUBLIC mod_borrower
mod_adapter PUBLIC mod_owner
mod_cr      PUBLIC mod_adapter
```

A target should expose only the include directory corresponding to its public
authority layer. Internal headers should be added only as private include
directories.

For exemple, to build the relative "stack of libraries" of the stream module:

```cmake
add_library(
    stream_borrower
    STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/borrowers/stream_borrower.c"
)
target_include_directories(
    stream_borrower
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/borrowers"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/adapters"
)
target_link_libraries(
    stream_borrower
    PUBLIC
        lexleo_policy_headers
    PRIVATE
        osal_mem
        lexleo_policy_headers
        lexleo_assert
)

file(
    GLOB STREAM_CREATOR_OWNERS_SOURCES
    CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/owners/creators/*.c"
)
add_library(
    stream_owner
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/owners/stream_owner.c"
        ${STREAM_CREATOR_OWNERS_SOURCES}
)
target_include_directories(
    stream_owner
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/owners"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/adapters"
)
target_link_libraries(
    stream_owner
    PUBLIC
        stream_borrower
    PRIVATE
        osal_mem
        osal_str
        lexleo_policy_headers
        lexleo_assert
)

add_library(
    stream_adapter
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/adapters/stream_adapter.c"
)
target_include_directories(
    stream_adapter
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/adapters"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries(
    stream_adapter
    PUBLIC
        stream_owner
    PRIVATE
        osal_mem
        lexleo_policy_headers
)

file(
    GLOB STREAM_CREATOR_CR_SOURCES
    CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/cr/creators/*.c"
)
add_library(
    stream_cr
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/cr/stream_cr.c"
        ${STREAM_CREATOR_CR_SOURCES}
)
target_include_directories(
    stream_cr
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/cr"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries(
    stream_cr
    PUBLIC
        stream_adapter
    PRIVATE
        osal_mem
        osal_str
        lexleo_policy_headers
        lexleo_assert
)
```

# Composition Root

Every module that needs explicit construction should expose a CR API.

The CR API is responsible for making construction and dependency injection
explicit.

## Configuration values

A module configuration type stores pure configuration:

```c
typedef struct mod_cfg_t {
    int reserved;
} mod_cfg_t;
```

A default configuration helper returns a value:

```c
mod_cfg_t mod_default_cfg(void);
```

## Environment values

A module environment type stores borrowed dependencies:

For exemple:
```c
typedef struct lexleo_vm_env_t {
	/** Borrowed memory operations. */
	const osal_mem_ops_t *mem_ops;

	/** Borrowed standard I/O operations. */
	const osal_stdio_ops_t *stdio_ops;

	/** Borrowed file operations. */
	const osal_file_ops_t *file_ops;

	/** Borrowed string operations. */
	const osal_str_ops_t *str_ops;

	/** Borrowed time operations. */
	const osal_time_ops_t *time_ops;

	/** Borrowed input stream. */
	stream_t *in;

	/** Borrowed output stream. */
	stream_t *out;

	/** Borrowed error stream. */
	stream_t *err;

	/** Borrowed logger. */
	logger_t *logger;
} lexleo_vm_env_t;
```

A default environment helper builds an explicit environment value:

For exemple:
```c
lexleo_vm_env_t lexleo_vm_default_env(
	const osal_mem_ops_t *mem_ops,
	const osal_stdio_ops_t *stdio_ops,
	const osal_file_ops_t *file_ops,
	const osal_str_ops_t *str_ops,
	const osal_time_ops_t *time_ops,
	stream_t *in,
	stream_t *out,
	stream_t *err,
	logger_t *logger
);
```

The returned environment does not own the dependencies it stores.

Because `mod_env_t` is CR-public, tests and higher-level CR code can adjust
selected fields before calling `mod_create()`. In particular, this affords
unit test with borrowed dependency injection.

## Handle creation

The create function allocates and initializes the handle itself:

```c
mod_status_t mod_create(
    mod_t **out,
    const mod_cfg_t *cfg,
    const mod_env_t *env);
```

This step copies configuration values and stores borrowed dependencies into the
private handle.

It must not create owned runtime resource in order to afford unit test with 
owned dependency injection (see the following section).

## Default owned-resource initialization

Default initialization is a separate step:

```c
mod_status_t mod_complete_default_init(
    mod_t *mod, 
    [a resource that is externally built and whose ownership is passed now],
    [an other resource that is externally built and whose ownership is passed now],
    [...]);
```

This function creates internal owned runtime resources required by the default module
configuration and takes ownership of the resources that is externally built.

For example, it may take ownership of a `stream_factory_t`, `stream_file_creator_t`
and a handle (all created by cr) and initialize resources. 

This separation allows tests to create a handle, inject fakes and then 
complete only the remaining default initialization.

## Destruction

The destroy function releases the handle and every owned resource attached to
it:

```c
void mod_destroy(mod_t **mod);
```

It must not destroy borrowed dependencies.

It should tolerate `NULL` and already-`NULL` handles when the module contract
requires idempotent teardown.

# Testing

This architecture makes modules testable because construction is split into
observable phases.

A test can:

1. build a default configuration;
2. build a CR-public environment;
3. replace borrowed dependencies with fakes;
4. create the handle without completing owned-resource initialization;
5. inject selected private owned resources through white-box test
   access when needed;
6. call `mod_complete_default_init()`;
7. verify status, ownership, and teardown behavior.

# Validation policy

Validation must distinguish public invalid arguments from internal
preconditions.

Conditions documented as invalid arguments must be checked and must return an
error status when violated.

Conditions documented as preconditions may be enforced with `LEXLEO_ASSERT()`.

Structures defined bu this project such as `_ops_t *` or `_vtbl_t *` have to
be built with their relative create-helper and the latter is the only one 
which check the validity of their fileds. Hence, when such a structure is
a function argument, it has to be treated as well-formed invariants.

Nested arguments that are only forwarded to another API-level contract should
not be deeply validated at every forwarding layer.

A violated `LEXLEO_ASSERT()` indicates a programming error and must not be
treated as a recoverable runtime status.
