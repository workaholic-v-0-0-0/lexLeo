@page specifications_osal_time_default_ops osal_time_default_ops() specifications

# Signature

```c
const osal_time_ops_t *osal_time_default_ops(void);
```

# Purpose

Returns the default injectable OSAL time operations table.

# Success

- Returns a non-NULL pointer to an `osal_time_ops_t`.
- The returned table provides callable function pointers for:
    - `now`
- The returned table is suitable for time-related operations that
  require dependency injection.

# Failure

- This operation does not report failure.

# Ownership

- Ownership of the returned operations table is not transferred to the caller.
- The caller must not destroy or modify the returned operations table.
