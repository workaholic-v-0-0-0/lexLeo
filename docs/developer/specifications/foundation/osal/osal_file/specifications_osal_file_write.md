@page specifications_osal_file_write osal_file_ops_t::write specifications

# Signature

```c
size_t (*write)(
    const void *ptr,
    size_t size,
    size_t nmemb,
    OSAL_FILE *stream,
    osal_file_status_t *st);
```

# Purpose

Write up to `nmemb` elements of size `size` from `ptr` to an open
`OSAL_FILE` through the active OSAL file backend.

# Preconditions

- If `ptr != NULL`, then `ptr` designates readable storage large enough for
  the attempted write.
- If `stream != NULL`, then `stream` designates a valid `OSAL_FILE`.
- If `stream != NULL`, then `stream` has been opened with a mode compatible
  with writing.
- If `st != NULL`, then `st` designates writable storage for one
  `osal_file_status_t`.

# Invalid arguments

- `ptr` must not be `NULL`.
- `stream` must not be `NULL`.
- `st` must not be `NULL`.

# Success

- Returns the number of elements successfully written.
- Writes data from `ptr`.
- Sets `*st` to `OSAL_FILE_STATUS_OK`.

# Failure

- Returns `0` and sets `*st` to `OSAL_FILE_STATUS_INVALID` for invalid
  arguments.
- Returns a value smaller than `nmemb` and sets `*st` to a non-OK
  `osal_file_status_t` representing the backend write error when a backend
  write failure occurs.
- Backend write failures are reported through the portable
  `osal_file_status_t` status domain.

# Ownership

- This operation does not transfer ownership of `stream`.
- The caller retains ownership of `stream` on both success and failure.

# Notes

- If `size == 0` or `nmemb == 0`, the operation returns `0` and sets `*st`
  to `OSAL_FILE_STATUS_OK`.
- This operation is element-oriented: the returned value is expressed in
  elements, not bytes.
