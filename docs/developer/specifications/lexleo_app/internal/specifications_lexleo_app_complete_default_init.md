@page specifications_lexleo_app_complete_default_init lexleo_app_complete_default_init() specifications

# Signature

```c
lexleo_app_cr_status_t lexleo_app_complete_default_init(
    lexleo_app_t *app,
    const lexleo_app_cfg_t *cfg
);
```

# Purpose

Complete application initialization using `cfg` and the borrowed runtime
dependencies stored in the handle.

The currently supported action is `LEXLEO_APP_ACTION_CONTROL_VM`.

# Preconditions

- `app` must point to a valid handle created by `lexleo_app_create()`.
- `app->stream_owner_opaque` must designate a valid container allocated
  during handle creation.
- The stream factory and its three stream creators must either all be
  present or all be `NULL`.
- Previously initialized or injected resources must be valid and compatible
  with the requested configuration and their ownership contracts.
- The only permitted aliases between stream fields are `vm_in == in`
  and `vm_out == out`.
- Each distinct stream stored in the handle must be owned by the application.
- An existing VM must borrow the streams, logger, and runtime operations
  associated with this application.
- The borrowed dependencies stored in `app` must be supported and well-formed.
- `cfg != NULL`.
- `cfg->action` must be `LEXLEO_APP_ACTION_CONTROL_VM`.
- Each non-NULL input path, dynamic-library path, and output path must
  designate a valid null-terminated string.
- For each of `cfg->in`, `cfg->out`, and `cfg->err`:
    - `kind` must be `LEXLEO_APP_IO_STDIO`, `LEXLEO_APP_IO_FILE`,
      or `LEXLEO_APP_IO_BUFFER`.
    - For `LEXLEO_APP_IO_STDIO` and `LEXLEO_APP_IO_BUFFER`,
      `path` and `mode` must be `NULL`.
    - For `LEXLEO_APP_IO_FILE`, `path` must point to a nonempty,
      null-terminated file path and `mode` must be `"rb"`, `"wb"`,
      or `"ab"`.

# Success

- Returns `LEXLEO_APP_CR_STATUS_OK`.
- Leaves `app` initialized for the requested action.
- Copies `cfg->action` into `app->action`.
- Copies every pointer in `cfg->input_paths` and
  `cfg->input_dynamic_libraries` into the corresponding handle array.
- Copies `cfg->output_path` into `app->output_path`.
- Does not duplicate the strings designated by these pointers.
- Preserves previously initialized or injected resources.
- Preserves the existing `stream_owner_opaque` container.
- If the stream factory and its three stream creators are absent,
  creates them and stores them in that container.
- Creates missing input, output, and error streams according to `cfg`.
- For standard streams, binds input to stdin, output to stdout,
  and error to stderr.
- Resolves the log path if it is empty.
- Creates a missing logger stream using the resolved or injected log path
  and the `"ab"` opening mode.
- Creates a missing logger borrowing the logger stream.
- If `vm_in` is NULL, sets it to `in` as a borrowed alias.
- If `vm_out` is NULL, sets it to `out` as a borrowed alias.
- Preserves previously initialized or injected VM streams.
- Creates a missing VM borrowing `vm_in`, `vm_out`, the logger,
  and the runtime operations.

# Failure

- Returns the status corresponding to the first initialization failure:
    - `LEXLEO_APP_CR_STATUS_INPUT_INIT_ERROR` if input stream initialization fails.
    - `LEXLEO_APP_CR_STATUS_OUTPUT_INIT_ERROR` if output stream initialization fails.
    - `LEXLEO_APP_CR_STATUS_ERR_INIT_ERROR` if error stream initialization fails.
    - `LEXLEO_APP_CR_STATUS_LOG_PATH_RESOLUTION_ERROR` if log-path resolution fails.
    - `LEXLEO_APP_CR_STATUS_LOGGER_STREAM_INIT_ERROR` if logger stream initialization fails.
    - `LEXLEO_APP_CR_STATUS_LOGGER_INIT_ERROR` if logger initialization fails.
    - `LEXLEO_APP_CR_STATUS_STREAM_OWNER_OPAQUE_INIT_ERROR` if stream factory
      or stream creator initialization fails.
    - `LEXLEO_APP_CR_STATUS_VM_INIT_ERROR` if VM initialization fails.
- Stops initialization at the first failure.
- Does not destroy the application handle.
- Resources retained by previously completed initialization steps remain
  attached.
- If stream factory or stream creator initialization fails, destroys
  the factory and creators created by that step and resets their fields
  to NULL.
- Releases other resources acquired by the failing step that were not
  retained or transferred to a retained resource.
- Previously injected resources are preserved.
- Does not guarantee restoration of the previous configuration fields.
- Leaves the handle valid for destruction by `lexleo_app_destroy()`.

# Ownership

- Ownership of the application handle is not transferred.
- Runtime objects created and retained by this function are owned
  by the application, directly or through its owned resources.
- Previously injected resources retain their established ownership.
- When `vm_in == in` or `vm_out == out`, the corresponding VM stream
  field is a borrowed alias and introduces no additional ownership.
- Otherwise, a non-NULL `vm_in` or `vm_out` designates a distinct stream
  owned by the application.
- The VM borrows its streams, logger, and runtime operations.
- The runtime operations stored in the handle remain borrowed.
- The strings designated by `input_paths`, `input_dynamic_libraries`,
  and `output_path` are borrowed and must remain valid throughout
  their use by the application.
- The `cfg` structure itself is not retained.

# Notes

- This function does not replace existing resources to apply a different
  configuration.
- Input paths, dynamic-library paths, and the output path are stored
  but are not used by the current VM control action.
- This function prepares the requested action but does not execute it.
- It does not start the VM.
- It is safe to destroy the application after a failed call.
