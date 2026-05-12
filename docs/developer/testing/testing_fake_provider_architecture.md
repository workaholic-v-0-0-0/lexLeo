@page testing_fake_provider_architecture Fake provider architecture

# Goals

Fake providers expose ready-to-inject fake dependencies for unit tests.

A fake provider does not implement fake behavior itself.

The fake behavior must remain implemented by the underlying fake modules
under tests/test_support/.

# Injectable dependency shapes

A fake provider must only expose injectable dependencies such as:
- `_ops_t *`,
- `dep_t *`,
- `dep_x_creator_t`.

Examples:
- `osal_stdio_test_fake_ops()`
- `stream_test_create_fake_stream()`
- `stream_test_fake_file_creator()`

# Architecture role

A fake provider acts as a lightweight test Composition Root.

# Doxygen conventions

A fake provider must provide:
- a public header under the tested module support tree,
- a `@file` header in the public fake provider header,
- a `@file` header in the implementation file.

# Recommended layout

A fake provider must follow the support layout of the module that owns the
injectable dependency.

For example:

```text
src/foundation/osal/osal_mem/tests/support/fake_provider/
├── CMakeLists.txt
├── include
│   └── osal
│       └── mem
│           └── test
│               └── osal_mem_fake_provider.h
└── src
    └── osal_mem_fake_provider.c
```

The fake provider target should link to:
- the fake module that implements the fake behavior,
- the module API that defines the injectable dependency shape.

# Notes

- An adapter module must not expose a fake provider ;
  its relative port module must.
