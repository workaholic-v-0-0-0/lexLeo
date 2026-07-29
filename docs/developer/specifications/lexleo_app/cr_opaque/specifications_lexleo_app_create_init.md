@page specifications_lexleo_app_create_init lexleo_app_create_init() specifications

# Signature

```c
bool lexleo_app_create_init(
    lexleo_app_t **out,
    const lexleo_app_cfg_t *cfg
);
```

# Purpose

Act as the LexLeo application Composition Root by creating and fully
initializing the application using `cfg` and the default runtime
environment.

The currently supported action is `LEXLEO_APP_ACTION_CONTROL_VM`.

# Preconditions

- `out != NULL`.
- `*out` does not need to be initialized before the call.
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

- Returns `true`.
- Stores a valid, fully initialized `lexleo_app_t` handle in `*out`.
- Uses the default memory, standard I/O, file, string, and time operations.
- Copies the requested action and path pointers into the handle without
  duplicating the strings.
- Creates the stream-owner container, stream factory, and stream creators.
- Creates the input, output, and error streams according to `cfg`.
- For standard streams, binds input to stdin, output to stdout,
  and error to stderr.
- Resolves the default log path.
- Creates the logger stream using that path and the `"ab"` opening mode.
- Creates the logger borrowing the logger stream.
- Sets `vm_in` to `in` and `vm_out` to `out` as borrowed aliases.
- Creates the VM borrowing those streams, the logger, and the runtime
  operations.
- Satisfies the success guarantees of
  `lexleo_app_complete_default_init()`.

# Failure

- Returns `false` if creation or initialization fails.
- Releases all resources acquired for the application during this call.
- Leaves `*out` unchanged.
- Does not transfer a partially initialized handle to the caller.

# Ownership

- The created application handle is owned by the caller.
- The handle must later be destroyed by `lexleo_app_destroy()`.
- Runtime objects created for the application are owned by the application,
  directly or through its owned resources.
- The `vm_in` and `vm_out` aliases introduce no additional ownership.
- The default runtime operations are borrowed.
- The strings designated by `input_paths`, `input_dynamic_libraries`,
  and `output_path` are borrowed and must remain valid throughout
  their use by the application.
- Ownership of `cfg` is not transferred.
- The `cfg` structure itself is not retained.

# Notes

- Combines application creation and default initialization in one call.
- Detailed internal status codes are not exposed through this function.
- Input paths, dynamic-library paths, and the output path are stored
  but are not used by the current VM control action.
- This function does not execute the requested action or start the VM.
- Failure cleanup releases resources but does not undo external effects
  already produced, such as file creation or truncation.
