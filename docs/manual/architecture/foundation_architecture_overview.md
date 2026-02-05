@page foundation_architecture_overview LexLeo foundation architecture — Overview

This overview of the foundation architecture provides the context necessary to understand the detailed pages.

# LexLeo foundation architecture — Overview

## CR, runtime, teardown

CR modules: vm, test modules
runtime modules: the others

---

## Via DI, all runtime module MUST BE UNIT TESTABLE

**DI locations**
- `mod_env_t`
- `mod_t (if mod handle-based)`

**DI time**
- CR time

**DI way**
- if `mod_1` depends on `mod_2`:
  - if `mod_1` handle-based (ie has `mod_1_t`):
    - `mod_1_t` MUST contain a field of type:
        - either `mod_2_t`
        - either `mod_2_factory_t`
        - either `mod_2_<funtionality>_creator_t`
  - if `mod_1` not handle-based:
      - `mod_1_env_1` MUST contain a field of type:
          - either `mod_2_t`
          - either `mod_2_factory_t`
          - either `mod_2_<funtionality>_creator_t`

---

## Module taxonomy

A module is characterized by five properties:

- `port`?
- `adapter`?
- `handle-based`?
- `creator-provider`?
- `stateful`?

---

## Module visibility hierarchy

Module visibility levels define:
- consumer kinds
- allowed header scopes

```
mod/include/mod/
├── adapters/
│   ├── mod_adapters_api.h
│   ├── mod_env.h
│   ├── mod_install.h
│   └── mod_key_type.h
├── borrowers/
│   ├── mod.h
│   └── mod_types.h
├── cr/
│   ├── mod_cr_api.h
│   └── mod_factory_cr_api.h
├── lifecycle/
│   └── mod_lifecycle.h
└── owners/
    ├── mod_<funtionality>_creator.h
    └── ...
```

A stack of four visibility levels:
- borrower consumer
  - CAN only include mod/include/mod/borrower/*
  - CANNOT create or destroy a `mod_t`

- lifecycle consumer
  - CAN include the borrower's headers and mod/include/mod/lifecycle/*
  - CANNOT create `mod_t`
  - CAN destroy `mod_t` it owns

- owner consumer
  - CAN include the lifecycle's headers and mod/include/mod/owners/*
  - HAS access to a `mod_factory_t` or a `mod_<funtionality>_creator_t`
  - CAN create and destroy a `mod_t`

- cr consumer
  - CAN include the whole mod/include/mod/*
  - CAN create/destroy `mod_t` and factories of `mod_t`

If mod is port:
- adapter "consumer"
  - CAN include mod/include/mod/adapters/* and mod/include/mod/borrowers/*
  - MUST implement functions of `mod_vtbl_t`
  - MUST implement

```c
mod_status_t consumer_create_mod(
    mod_t **out,
    const consumer_args_t *args,
    const consumer_cfg_t *cfg,
    const consumer_env_t *env );
```

  - if mod creator-provider, this consumer
    - MUST implement

```c
mod_status_t consumer_ctor(
    void *ud,
    const void *args,
    mod_t **out );

mod_status_t consumer_create_desc(
    mod_adapter_desc_t *out,
    mod_key_t key,
    const consumer_cfg_t *cfg,
    const consumer_env_t *env,
    const osal_mem_ops_t *mem );
```

---

## Allocation rules

## CR allocations

The Composition Root (CR) is allowed to allocate:

- `mod_cfg_t`
- `mod_env_t`
- `mod_t`
- `mod_factory_t`
- `mod_<functionality>_creator_t`

---

## Runtime allocation rules

At runtime:

- Allocations MUST be performed only by an **owner consumer**.
- `mod_t` MUST be created exclusively through:
    - `mod_factory_t`, or
    - `mod_<functionality>_creator_t`.