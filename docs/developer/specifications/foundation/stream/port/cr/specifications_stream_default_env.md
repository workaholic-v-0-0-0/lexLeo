@page specifications_stream_default_env stream_default_env() specifications

# Signature

~~~c
stream_env_t stream_default_env(
    const stream_vtbl_t *vtbl,
    const osal_mem_ops_t *mem_ops);
~~~

# Purpose

Build a default `stream_env_t` from the external borrowed dependencies
required by a `stream`.

# Preconditions

- `vtbl` must point to a valid `stream` virtual table.
- `mem_ops` must point to a valid memory operations table.

# Success

- Returns a well-formed `stream_env_t`.
- Initializes `ret.vtbl` with `vtbl`.
- Initializes `ret.mem` with `mem_ops`.

# Failure

- None.

# Ownership

- The returned environment stores borrowed references to the supplied external
  dependencies.
- Ownership of `vtbl` is not transferred.
- Ownership of `mem_ops` is not transferred.

# Notes

- The dependencies stored in `stream_env_t` are the external borrowed
  dependencies used during `stream` creation.
- This helper does not allocate.
- This helper performs no validation.
