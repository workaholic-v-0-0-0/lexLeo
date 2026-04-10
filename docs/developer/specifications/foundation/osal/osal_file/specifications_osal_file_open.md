@page specifications_osal_file_open osal_file_ops_t::open specifications

# Signature

```c
osal_file_status_t (*open)(
    OSAL_FILE **out,
    const char *pathname,
    const char *mode,
    const osal_mem_ops_t *mem_ops);
```

# Purpose

Acquire a public `OSAL_FILE` handle for `pathname` through the active OSAL
file backend, using the provided portable mode string and injected memory
operations.

# Preconditions

- If `mem_ops != NULL`, then `mem_ops` designates a valid `osal_mem_ops_t`.

# Invalid arguments

- `out` must not be `NULL`.
- `pathname` must not be `NULL`.
- `pathname` must not designate an empty string.
- `mode` must not be `NULL`.
- `mode` must be one of the supported portable `osal_file` modes.
- `mem_ops` must not be `NULL`.

# Success

- Returns `OSAL_FILE_STATUS_OK`.
- Stores a valid newly acquired `OSAL_FILE` handle in `*out`.

# Failure

- Returns `OSAL_FILE_STATUS_INVALID` for invalid arguments.
- Returns `OSAL_FILE_STATUS_OOM` on allocation failure.
- Returns a backend-mapped file status when the requested file resource
  cannot be opened.
- Such statuses may include, depending on the active platform and failure
  condition:
  - `OSAL_FILE_STATUS_NOENT`
  - `OSAL_FILE_STATUS_PERM`
  - `OSAL_FILE_STATUS_EXISTS`
  - `OSAL_FILE_STATUS_NOSPC`
  - `OSAL_FILE_STATUS_NAMETOOLONG`
  - `OSAL_FILE_STATUS_NOTDIR`
  - `OSAL_FILE_STATUS_ISDIR`
  - `OSAL_FILE_STATUS_MFILE`
  - `OSAL_FILE_STATUS_NFILE`
  - `OSAL_FILE_STATUS_LOOP`
  - `OSAL_FILE_STATUS_ROFS`
  - `OSAL_FILE_STATUS_NODEV`
  - `OSAL_FILE_STATUS_NXIO`
  - `OSAL_FILE_STATUS_STALE`
  - `OSAL_FILE_STATUS_IO`
- Leaves `*out` unchanged if `out` is not `NULL`.

# Ownership

- On success, ownership of the acquired `OSAL_FILE` handle is transferred
  to the caller.
- The acquired handle must later be released via `osal_file_ops_t::close`.
- No ownership is transferred on failure.

# Notes

- Supported portable modes are `"rb"`, `"wb"`, and `"ab"`.
- `pathname` is expressed as a project-level UTF-8 path string.
- Any platform-specific pathname conversion is performed by the active
  backend implementation.
