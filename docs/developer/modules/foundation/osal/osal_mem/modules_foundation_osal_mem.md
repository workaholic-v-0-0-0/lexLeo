@page modules_foundation_osal_mem osal_mem

# Purpose

`osal_mem` is the low-level OSAL memory module.

It provides portable memory primitives for allocation, deallocation, raw memory
copy, raw memory fill, raw memory move, alignment helpers, and injectable memory
operations.

# Public API

See @ref osal_mem_api "osal_mem API".

# Specifications

See @subpage specifications_osal_mem "osal_mem specifications".

# Tests

See @subpage testing_foundation_osal_mem_unit "osal_mem unit tests".

# Responsibilities

- Provide default memory allocation and deallocation primitives.
- Provide raw byte-oriented memory operations.
- Provide an injectable `osal_mem_ops_t` table for modules that need allocator
  dependency injection.
- Provide alignment helpers used by low-level components.
