@page testing_fake_module_architecture Fake modules architecture

# Goals

Fake modules provide test doubles for unit tests.

Only injectable dependencies should be replaced by fake modules.

A fake module must therefore correspond to a dependency passed through
dependency injection via a `mod_env_t` field, such as `dep_t *` or
`dep_x_creator_t`.

For handle-based modules, injected dependencies are stored in the created
`mod_t` instance.

For non-handle-based modules, the `mod_env_t` is passed directly to the API
functions that need injected dependencies.

# API

A fake module exposes three API sections:

- **FAKE API**: functions injected in place of the real backend.
- **CFG API**: functions used by tests to configure fake behavior.
- **SPY API**: functions used by tests to observe calls, arguments, and effects.

# Implementation

## `fake_*_t`

`fake_*_t` represents the state of one fake instance.

It must be used only when the faked dependency has per-instance state.

The `fake_*_t` fields must be classified with the following sections:
- state,
- configuration,
- spy.

## `fake_*_ctrl_t`

`fake_*_ctrl_t` represents non-instance-relative fake control state.

It is stored in a private static object in the fake implementation file.

Examples:
- next instance creation result,
- non-instance-relative operation result,
- non-instance-relative call count,
- last arguments passed to non-instance-relative fake functions.

The `fake_*_ctrl_t` fields must be classified with the following sections:
- state,
- configuration,
- spy.

# Doxygen conventions

Each fake module must provide:
- a `@defgroup` page for the test-support reference,
- a `@file` header in the public fake header,
- a `@file` header in the implementation file,
- a developer testing page under `docs/developer/testing/fake_modules/`.

# Recommended layout

A fake module must follow this structure:

```text
tests/test_support/fake_x/
├── CMakeLists.txt
├── include/lexleo/test/fake_x.h
├── src/fake_x.c
└── test_support_fake_x.dox
```

The public header must be organized as:

```c
// FAKE API

// CFG

// SPY
```

The private source file may start with:

```c
typedef struct fake_x_t
{
    /* state */

    // [...]

    /* cfg */

    // [...]

    /* spy */

    // [...]

} fake_x_t;

typedef struct fake_x_ctrl_t
{
	/* state */

    // [...]

	/* cfg */

	// [...]

	/* spy */

	// [...]

} fake_x_ctrl_t;

static fake_x_ctrl_t g_fake_x_ctrl;

```
