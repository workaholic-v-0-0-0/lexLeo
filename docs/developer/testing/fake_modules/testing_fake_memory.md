@page testing_fake_memory fake_memory

# Purpose

`fake_memory` is a fake backend for memory-dependent unit tests.

It allows tests to allocate memory from a deterministic arena, inject allocation
failures, reset fake state, and observe memory errors such as leaks, invalid
frees, and double frees.

# Usage

`fake_memory` is intended to be used by test fake providers that need
deterministic memory operations, for example an `osal_mem_ops_t` provider used
by tests.

# Reference

See @ref test_support_fake_memory "fake_memory reference".
