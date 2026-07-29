@page specifications_fs_stream_complete_default_init fs_stream_complete_default_init() specifications

# Signature

```c
fs_stream_status_t fs_stream_complete_default_init(
    fs_stream_t *fs_stream,
    const fs_stream_cfg_t *cfg,
    const stream_regular_file_creator_args_t *args);
```

# Purpose

Complete the initialization of a partially initialized `fs_stream_t` backend by
creating and attaching its owned file resource from adapter configuration and
runtime regular-file creation arguments.

# Preconditions

- `fs_stream` must point to an `fs_stream_t` created by `fs_stream_create()`
  whose external borrowed dependencies are correctly initialized.
- If `fs_stream->file != NULL`, it must point to a valid `OSAL_FILE`.
- `cfg` must point to a valid `fs_stream_cfg_t`.
- `args` must not be `NULL`.
- `args->mode` must be one of the supported portable `osal_file` modes (`"rb"`,
  `"wb"`, or `"ab"`).

# Invalid arguments

- `args->path` must not be `NULL`.
- `args->path` must not be an empty string.
- `args->path` length must be less than or equal to 255.

# Success

- Returns `FS_STREAM_STATUS_OK`.
- If `fs_stream->file != NULL`:
    - Leaves `fs_stream` unchanged.
- Otherwise:
    - Delegates file creation to the external borrowed file operations using the
      runtime arguments provided by `args` and the external borrowed memory
      operations.
    - Stores the resulting valid `OSAL_FILE` handle in `fs_stream->file` as an
      owned resource.
- Leaves all external borrowed dependencies of `fs_stream` unchanged.
- The resulting `fs_stream_t` is fully initialized and can be used by the
  `stream_vtbl_t` operations fetchable by `fs_stream_vtbl()`.

# Failure

- Returns `FS_STREAM_STATUS_INVALID` for invalid arguments.
- Otherwise, returns the `fs_stream_status_t` corresponding to the status
  returned by the borrowed file operations.
- Leaves `fs_stream->file` equal to `NULL`.
- Leaves all external borrowed dependencies of `fs_stream` unchanged.
- No file ownership is transferred to `fs_stream`.

# Ownership

- Ownership of `fs_stream`, `cfg`, and `args` is not transferred.
- The external borrowed dependencies stored in `fs_stream` remain borrowed.
- If a new `OSAL_FILE` handle is created successfully, its ownership is
  transferred to `fs_stream`.
- The owned file resource is later released via `fs_stream_t::close()`.

# Notes

- This function completes the initialization started by `fs_stream_create()`.
- `fs_stream_create()` initializes the backend's external borrowed
  dependencies.
- `fs_stream_complete_default_init()` initializes the remaining backend state,
  namely internal borrowed dependencies and owned resources when applicable.
- In the `fs_stream` adapter, this completion-time state consists of the owned
  file resource.
