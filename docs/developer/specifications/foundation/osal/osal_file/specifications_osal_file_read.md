@page specifications_osal_file_read osal_file_ops_t::read specifications

# Signature

```c
size_t (*read)(
    void *ptr,
    size_t size,
    size_t nmemb,
    OSAL_FILE *stream,
    osal_file_status_t *st);
```

# Purpose

Read up to `nmemb` elements of size `size` from an open `OSAL_FILE` into
`ptr` through the active OSAL file backend.

# Preconditions

- If `ptr != NULL`, then `ptr` designates writable storage large enough for
  the attempted read.
- If `stream != NULL`, then `stream` designates a valid `OSAL_FILE`.
- If `stream != NULL`, then `stream` has been opened with a mode compatible
  with reading.
- If `st != NULL`, then `st` designates writable storage for one
  `osal_file_status_t`.

# Invalid arguments

- `ptr` must not be `NULL`.
- `stream` must not be `NULL`.
- `st` must not be `NULL`.

# Success

- Returns the number of elements successfully read.
- Stores the read data into `ptr`.
- Sets `*st` to `OSAL_FILE_STATUS_OK`.

# Failure

- Returns `0` and sets `*st` to `OSAL_FILE_STATUS_INVALID` for invalid
  arguments.
- Returns a value smaller than `nmemb` and sets `*st` to a non-OK
  `osal_file_status_t` representing the backend read error when a backend
  read failure occurs.
- Backend read failures are reported through the portable
  `osal_file_status_t` status domain.

# Ownership

- This operation does not transfer ownership of `stream`.
- The caller retains ownership of `stream` on both success and failure.

# Notes

- A return value smaller than `nmemb` does not by itself distinguish
  end-of-file from failure; callers must inspect `*st`.
- If `nmemb == 0`, the operation returns `0` and sets `*st` to
  `OSAL_FILE_STATUS_OK`.
- This operation is element-oriented: the returned value is expressed in
  elements, not bytes.
