@page specifications_lexleo_vm_default_env lexleo_vm_default_env() specifications

# Signature

```c
lexleo_vm_env_t lexleo_vm_default_env(
    const osal_mem_ops_t *mem_ops,
    const osal_stdio_ops_t *stdio_ops,
    const osal_file_ops_t *file_ops,
    const osal_str_ops_t *str_ops,
    const osal_time_ops_t *time_ops,
    stream_t *in,
    stream_t *out,
    stream_t *err,
    logger_t *logger
);
```

# Purpose

Return the default runtime environment used to create a LexLeo VM handle.

## Preconditions

- `mem_ops != NULL`.
- `stdio_ops != NULL`.
- `file_ops != NULL`.
- `str_ops != NULL`.
- `time_ops != NULL`.
- `in != NULL`.
- `out != NULL`.
- `err != NULL`.
- `logger != NULL`.
- Each OSAL operations table must be well-formed.

# Success

- Returns a `lexleo_vm_env_t` initialized with the provided borrowed
  dependencies.

# Failure

- This function cannot fail.

# Ownership

- The returned environment value is returned by value.
- The caller owns the returned value.
- All stored dependencies are borrowed.
- No destructor is required for `lexleo_vm_env_t`.

# Notes

- The returned value may be adjusted by Composition Root code before being
  passed to `lexleo_vm_create()`.
- The borrowed dependencies may come from default providers or from
  test/custom providers.
