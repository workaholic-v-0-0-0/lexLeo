@page module_taxonomy LexLeo module taxonomy

# Module taxonomy

## Maximal envelope layout

The maximal layout is shown first to establish context for the detailed sections that follow:

```
(mod/ | mod/port | <relative port>/adapters/mod)/
├── CMakeLists.txt
├── include/
│   └── mod/
│       ├── adapters/
│       │   ├── mod_adapters_api.h
│       │   ├── mod_env.h
│       │   ├── mod_install.h
│       │   └── mod_key_type.h
│       ├── borrowers/
│       │   ├── mod.h
│       │   └── mod_types.h
│       ├── cr/
│       │   ├── mod_cr_api.h
│       │   └── mod_factory_cr_api.h
│       ├── lifecycle/
│       │   └── mod_lifecycle.h
│       └── owners/
│           ├── mod_<functionality_1>_creator.h
│           ├── mod_<functionality_2>_creator.h
│           └── ...
├── src/
│   ├── internal/
│   │   ├── mod_factory_handle.h
│   │   └── mod_handle.h
│   ├── mod.c
│   └── mod_factory.c
└── tests/
    ├── CMakeLists.txt
    ├── support/
    │   ├── CMakeLists.txt
    │   └── fake_provider/
    │       ├── CMakeLists.txt
    │       ├── include/
    │       │   └── mod/
    │       │       └── test/
    │       │           └── mod_fake_provider.h
    │       └── src/
    │           └── mod_fake_provider.c
    ├── unit/
    │   ├── CMakeLists.txt
    │   └── unit_test_mod.c
    └── integration/
        ├── CMakeLists.txt
        └── integration_test_mod.c
```

IMPORTANT  
This layout represents a maximal envelope.
A real module is not expected to provide all of these directories or headers.
Modules must include only the parts that make sense for their nature.

## Module taxonomy in five axes

### Handle-based module

need allocation, has a runtime state structured in a handle

#### Rules

the handle is opaque, i.e. consumers only see it from include/borrowers/mod_types.h:

```c
typedef struct mod_t mod_t;
```

#### Headers specific to handle-based module

##### src/internal/mod_handle.h

to define the opaque type for the mod handle:

```c
typedef struct mod_t {
    mod_state_t state;
    const osal_mem_ops_t *mem_ops;
    // ...
} fs_stream_t;
```

##### src/internal/mod_state.h

to define the internal state of the module:

```c
typedef struct mod_state_t {
  // ...
} mod_state_t;
```

### Creator-provider module

- `mod_t` can be constructed by a runtime module via an injected creator.
- an owner consumer consum can do so if CR injects a `mod_<functionality>_creator_t`:
  - into `consum_env_t` if consum is not handle-based
  - or into `consum_t` if consum is handle-based

#### Rules

`m̀od_t` MUST BE constructed only in these cases:
- during CR
- during runtime by an owner consumer and through `mod_<functionality>_creator_t`

#### two kinds of mechanism for providing creators

##### Mechanism of a non-port module (see section "Port module") for providing creators

todo

##### Mechanism of a port module for providing creators

- through a hidden factory
- this mechanism is detailed in section "Port module" for a port module MUST BE a creator-provider module

#### Rules

Consumers never receive a factory directly; only creator objects are injectable.

### Port module

#### layout:

```
mod/port/
├── include/
│   └── mod/
│       ├── adapters/
│       │   ├── mod_adapters_api.h
│       │   ├── mod_env.h
│       │   ├── mod_install.h
│       │   └── mod_key_type.h
│       ├── borrowers/
│       │   ├── mod.h
│       │   └── mod_types.h
│       ├── cr/
│       │   ├── mod_cr_api.h
│       │   └── mod_factory_cr_api.h
│       ├── lifecycle/
│       │   └── mod_lifecycle.h
│       └── owners/
│           ├── mod_<functionality_1>_creator.h
│           ├── mod_<functionality_2>_creator.h
│           └── ...
├── src/
│   ├── internal/
│   │   ├── mod_factory_handle.h
│   │   └── mod_handle.h
│   ├── mod.c
│   └── mod_factory.c
└── tests/
```

#### Rules

- `mod_create` not in include/cr/ but in include/adapters/ so that an adapter adapt can implement:

```c
mod_status_t adapt_create_mod(
    mod_t **out,
    const adapt_args_t *args,
    const adapt_cfg_t *cfg,
    const adapt_env_t *env );
```

- port module MUST BE a creator-provider module 

#### Headers specific to port module

##### include/adapters/mod_adapters_api.h

to know what to implement:

```c
typedef struct mod_vtbl_t {
	//...
} stream_vtbl_t;
```

to know how to create a `mod_t`:

```c
// used by adapter to define its mod_ctor_fn_t
stream_status_t mod_create(
	mod_t **out,
	const mod_vtbl_t *vtbl,
	void *backend,
	const mod_env_t *env );
```

##### include/adapters/mod_env.h

to be able to call `mod_create`:

```c
typedef struct mod_env_t {
    // ...
} mod_env_t;
```

##### include/stream/adapters/stream_install.h

to know what CR need to provide to it how to register the relative adapter:

```c
typedef void (*ud_dtor_fn_t)(void *ud, const osal_mem_ops_t *mem);

// constructor that an adapter must implement
typedef mod_status_t (*mod_ctor_fn_t)(
    void *ud,
    const void *args,
    mod_t **out );

// adapter must provide to CR a constructor of
// this type, where mod_key_t param is to be
// provide by CR
typedef struct mod_adapter_desc_t {
    mod_key_t key;
    mod_ctor_fn_t ctor;
    void *ud;
    ud_dtor_fn_t ud_dtor;
} mod_adapter_desc_t;
```

#### include/adapters/stream_key_type.h

to know the type of the key related to the relative adapter in registration by CR:

```c
typedef const char *stream_key_t;
```

##### include/cr/mod_factory_cr_api.h

to make CR know how to create a factory and register adapters

```c
typedef struct stream_factory_cfg_t {
	size_t fact_cap;
} stream_factory_cfg_t;

stream_status_t stream_create_factory(
	stream_factory_t **out,
	const stream_factory_cfg_t *cfg,
	const stream_env_t *env );

void stream_destroy_factory(stream_factory_t **fact);

stream_status_t stream_factory_add_adapter(
	stream_factory_t *fact,
	const stream_adapter_desc_t *desc );

stream_status_t stream_factory_create_stream(
	const stream_factory_t *f,
	stream_key_t key,
	const void *args,
	stream_t **out);

```

### Adapter module

#### layout:

A port module and its adapters are structured in the following layout:

```
<relative port>/
├── adapters/
│   ├── <adapter 1>
│   ├── <adapter 2>
│   └── ...
└── port/
```

An adapter layout such as `<adapter 1>`:

```
<adapter 1>/
├── include/
│   └── mod/
│       └── cr/
│           ├── mod_cr_api.h
│           └── mod_types.h
├── src/
│   ├── internal/
│   │   ├── mod_ctor_ud.h
│   │   ├── mod_handle.h
│   │   └── mod_state.h
│   └── mod.c
└── tests/
```

#### Rules 

An adapter module is visible only by CR

#### Headers specific to adapter module

##### src/internal/mod_ctor_ud.h

to define the opaque user data type to be passed for creation of a `mod_t`:

```c
typedef struct mod_ctor_ud_t {
	const mod_cfg_t *cfg;
	mod_env_t env;
} mod_ctor_ud_t;
```

##### include/mod/cr/mod_cr_api.h

to make CR know how to register/install this adapter into the port factory (i.e., build the adapter descriptor).

```c
stream_status_t mod_create_stream(
  stream_t **out,
  const mod_args_t *args,
  const mod_cfg_t *cfg,
  const mod_env_t *env );

// Factory callback delegating to create_mod
stream_status_t fs_stream_ctor(
  void *ud,
  const void *args,
  mod_t **out );

stream_status_t mod_create_desc(
  mod_adapter_desc_t *out,
  <relative port>_key_t key,
  const mod_cfg_t *cfg,
  const mod_env_t *env,
  const osal_mem_ops_t *mem ); // MUST BE THE FACTORY'S
```

### Stateful module

- A module is stateful if it owns internal mutable static or global state.
- A module IS NOT stateful if it only contains:
  - type declarations,
  - static const data,
  - pure functions,
  - logic operating exclusively on explicit handles or parameters.

#### Rules

vm and test modules in tests/test_support/ MUST BE the only stateful modules.
