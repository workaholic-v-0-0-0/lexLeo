@page testing_page_strategy Testing stategy

LexLeo is designed to be **unit-testable by construction**. All runtime modules must support dependency injection (DI) so they can be tested in isolation with deterministic doubles.

# Core principles

- **Test-first (TDD):** behavior is specified by tests before or alongside implementation.
- **Unit tests by default:** the primary validation level is the unit test suite.
- **Explicit dependency injection:** modules must not hard-depend on OS, filesystem, heap, time, or global state.
- **Deterministic doubles:** external effects are replaced with fakes that are fully controlled by the test.
- **Memory safety as a contract:** tests validate absence of leaks and invalid frees (including OOM scenarios where relevant).

# What “DI everywhere” means in LexLeo

All runtime modules must receive their dependencies through one of these injection points:

- `mod_env_t` for non-handle-based modules.
- `mod_t` fields for handle-based modules (dependencies stored in the runtime handle).

Injected dependencies typically include:

- `osal_mem_ops_t` (allocation strategy, leak/invalid-free tracking, OOM simulation)
- `osal_file_ops_t` (file I/O abstraction)
- other ports’ creators/factories when a module depends on another module

# Test levels

- **Unit tests (default):** validate one module in isolation using doubles.
- **Integration tests:** validate a small set of modules wired together (still using controlled OSAL layers).
- **System-level checks:** reserved for higher-level scenarios once the interpreter pipeline stabilizes.

# Unit test pattern

LexLeo unit tests follow a consistent structure:

- **Contract comment** stating what is tested and which doubles are used.
- **Parametric test cases** describing scenarios and expected outcomes.
- **Fixture-based setup/teardown** that:
    - wires injected dependencies (`env`),
    - configures fakes (including failure injection),
    - enforces memory invariants at teardown.

# Example: testing an adapter via injected OSAL fakes

A typical adapter unit test (e.g., `fs_stream_create_stream`) injects:
- `osal_mem_ops_t` via a fake memory provider (leak checks + optional OOM),
- `osal_file_ops_t` via a fake file provider (backing buffer + controlled failures).

The test validates both:
- **API-level behavior** (returned status, output handle validity),
- **observable effects** through the fake backend (written bytes, flush behavior),
  while teardown asserts:
- no leaks,
- no invalid frees,
- no double frees.

# Validation testing policy

- black box: test must only check what is observable
- Tests do not check preconditions in the arrange phase.
- Fields of a `_ops_t` are only checked at creation time.
- Fields of a `_vtbl_t` are only checked at the creation time of the relative port handle.
