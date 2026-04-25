@page specifications_osal_file_gets osal_file_gets specifications

# Signature

```c
char *osal_file_gets(
    char *out,
    size_t out_size,
    OSAL_FILE *stream,
    osal_file_status_t *st);
```

# Purpose

Read at most `out_size - 1` characters from `stream` and store them into the buffer pointed to by `out`. 
Reading stops after an EOF or a newline. If a newline is read, it is stored into the buffer.
A terminating null byte ('\0') is stored after the last character in the buffer.

# Preconditions

- If `out != NULL`, then `out` designates writable storage large enough for
  the attempted read.
- If `stream != NULL`, then `stream` designates a valid `OSAL_FILE`.
- If `stream != NULL`, then `stream` has been opened with a mode compatible
  with reading.
- If `st != NULL`, then `st` designates writable storage for one
  `osal_file_status_t`.

# Invalid arguments

- `out` must not be `NULL`.
- `stream` must not be `NULL`.
- `st` must not be `NULL`.

# Success

- If end of file occurs while no characters have been read, returns `NULL`,
  otherwise returns `out`.
- Stores the read data into `out`.
- Sets `*st` to `OSAL_FILE_STATUS_OK`.

# Failure

- Returns `NULL`
- Sets `*st` to `OSAL_FILE_STATUS_INVALID` for invalid arguments.
- Sets `*st` to a non-OK `osal_file_status_t` representing the backend 
  read error when a backend read failure occurs.
- Backend read failures are reported through the portable
  `osal_file_status_t` status domain.

# Ownership

- This operation does not transfer ownership of `stream`.
- The caller retains ownership of `stream` on both success and failure.
