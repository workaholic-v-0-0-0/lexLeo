@page tests_management Test management

# Test management

This page defines the project conventions for organizing test code, reusable
test-support components, and CMake targets used to build tests.

It complements:

- @ref arch_standards_naming
- @ref arch_standards_module_layout
- @ref arch_standards_lifecycle_visibility_rules

---

## Goals

- Keep production code independent from test-only infrastructure.
- Avoid test targets manually managing include paths.
- Make test wiring and dependency injection explicit and reproducible.
- Enable reuse of fakes across multiple modules without coupling to prod code.

---

## Standard test target naming

For a production module `<module>`, the following target names are used.

### `<module>`

Production runtime target.

---

### `<module>_wiring_headers`

Wiring-only target meant for the composition root (CR) and for test-time wiring.

It typically exports internal wiring headers and transitive requirements needed
to build wiring code.

---

### `<module>_test_headers`

Test-only target exporting `<module>/tests/include/`.

Used to keep test-only headers out of production targets, while providing a
clean, decoupled include path for unit/integration tests.

Example contents:

- `<module>_test_api.h` (header included by the module's test sources)

Note:

- `<module>_test_api.h` is a header, not a CMake target.
- `<module>_test_headers` is the CMake target exporting the include directory.

This target exists mainly as a clean CMake decoupling directive: tests depend
on a target rather than hardcoded include paths.

---

### `<module>_test_usage_requirements`

Test-only "module requirements bundle" used by the module's own tests.

This target aggregates module-specific test requirements such as:

- `<module>_wiring_headers`
- `<module>_test_headers`
- module-level test-support dependencies (e.g., OSAL fake providers)

This target does not include the global test framework dependencies, which
remain linked explicitly by the test executable (e.g., `cmocka`).

Example:

```cmake
target_link_libraries(unit_test_<module> PRIVATE
    <module>_test_usage_requirements
    lexleo_cmocka_support
    cmocka
)
```

## Reusable fakes vs module-local providers

The project distinguishes between:

- reusable fake implementations (cross-module)
- module-local providers (facades for wiring/injection)

### `fake_<module>` (reusable fake)

Reusable fake implementation (STATIC library) located under:

- `lexleo/tests/test_support/`

Files:

- `fake_<module>.h`
- `fake_<module>.c`

Naming note:

- `fake_memory` is a deliberate naming exception
  (instead of `fake_osal_mem`).

---

### `<module>_fake_provider` (module-local facade)

Lightweight module-local provider located under:

- `<module>/tests/support/`

Files:

- `<module>_fake_provider.h`
- `<module>_fake_provider.c`

This target is a test-only library (typically STATIC) that:

- links the reusable `fake_<module>` target
- optionally links `<module>_wiring_headers` when injection requires internal
  wiring headers or internal context types

Its purpose is to provide a module-scoped facade for retrieving/configuring fake
components needed by other modules' tests, without duplicating wiring logic.
