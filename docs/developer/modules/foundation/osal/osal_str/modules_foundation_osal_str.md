@page modules_foundation_osal_str osal_str

# Purpose

`osal_str` is the low-level OSAL string module.

It provides portable string helpers for string length, comparison, character
search, whitespace classification, bounded formatting, and injectable string
operations that require allocator consistency.

# Public API

See @ref osal_str_api "osal_str API".

# Specifications

See @subpage specifications_osal_str "osal_str specifications".

# Tests

See @subpage testing_foundation_osal_str_unit "osal_str unit tests".

# Responsibilities

- Provide default string utility wrappers.
- Provide bounded formatting through `osal_snprintf()`.
- Provide character search helpers for first and last occurrence lookup.
- Provide whitespace classification through `osal_isspace()`.
- Provide an injectable `osal_str_ops_t` table for string operations that need
  dependency injection.
- Ensure duplicated strings are allocated through the memory provider supplied
  by the caller.
