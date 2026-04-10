@page specifications_osal_file_close osal_file_ops_t::close specifications

# Signature

```c
osal_file_status_t (*close)(OSAL_FILE *stream);
```

# Purpose

Close an open `OSAL_FILE` handle and release the associated OSAL file
resource.

# Preconditions

- If `stream != NULL`, then `stream` designates a valid `OSAL_FILE`.

# Invalid arguments

- `stream` must not be `NULL`.

# Success

- Returns `OSAL_FILE_STATUS_OK`.
- Closes the underlying file resource associated with `stream`.
- Releases the OSAL wrapper associated with `stream`.

# Failure

- Returns `OSAL_FILE_STATUS_INVALID` for invalid arguments.
- Returns a backend-mapped file status when the underlying file resource
  cannot be closed.
- Such statuses may include, depending on the active platform and failure
  condition:
    - `OSAL_FILE_STATUS_BADF`
    - `OSAL_FILE_STATUS_INTR`
    - `OSAL_FILE_STATUS_IO`
- Does not release ownership to another party.

# Ownership

- On entry, `stream` is owned by the caller.
- On success, ownership of `stream` ends and the handle is no longer valid.
- No ownership is transferred to another party.
- On failure, the caller retains ownership of `stream`.

# Notes

- `osal_file_ops_t::close` is the matching release operation for handles
  acquired by `osal_file_ops_t::open`.
- After a successful close, the `OSAL_FILE` handle must not be reused.
