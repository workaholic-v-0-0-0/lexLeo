@page specifications_osal_file_mkdir osal_file_mkdir specifications

# Signature

```c
osal_file_status_t osal_file_mkdir(const char *pathname);
```

# Purpose

Attempt to create the directory designated by `pathname`.

# Invalid arguments

- `pathname` must not be `NULL`.
- `pathname` must not be an empty string.

# Success

- Returns `OSAL_FILE_STATUS_OK`.
- Creates the directory designated by `pathname`.
- The newly created directory is owned according to the rules of the underlying platform.
- Directory permissions and access rights are determined by the underlying platform.

# Failure

- Returns `OSAL_FILE_STATUS_INVALID` for invalid arguments.
- Returns `OSAL_FILE_STATUS_OOM` if an internal allocation needed by the platform backend fails.
- Returns an `osal_file_status_t` representing the backend mkdir error when a backend mkdir failure occurs.
- Backend mkdir failures are reported through the portable `osal_file_status_t` status domain.

# Ownership

- The caller retains ownership of `pathname` on both success and failure.
