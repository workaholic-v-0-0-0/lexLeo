@page specifications_osal_mem_default_ops osal_mem_default_ops() specifications

# Signature

```c
const osal_mem_ops_t *osal_mem_default_ops(void);
```

# Purpose

Returns the default low-level OSAL memory operations table.

# Success

- Returns a non-`NULL` pointer to an `osal_mem_ops_t`.
- The returned table uses the public default `osal_mem` primitives as its
  implementation:
  - `osal_malloc`
  - `osal_free`
  - `osal_calloc`
  - `osal_realloc`

# Failure

- This operation does not report failure.

# Ownership

- Ownership of the returned operations table is not transferred to the caller.
- The caller must not destroy or modify the returned operations table.

# Notes

- The returned table exposes the active platform implementation of the
  `osal_mem` module.
- The returned table is intended to be used through the
  `osal_mem_ops_t` contract.
- The lifetime of the returned table is managed by the module.
