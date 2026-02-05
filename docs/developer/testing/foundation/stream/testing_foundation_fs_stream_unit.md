@page testing_foundation_fs_stream_unit fs_stream unit tests

It covers:
- `fs_stream_default_cfg()`
- `fs_stream_default_env()`
- `fs_stream_create_stream()`
- `fs_stream_create_desc()`
- descriptor constructor usage through `stream_adapter_desc_t::ctor`

@anchor testing_foundation_fs_stream_unit_default_cfg
# fs_stream_default_cfg() unit tests

See @ref specifications_fs_stream_default_cfg "fs_stream_default_cfg() specifications"

## Functions under test

~~~c
fs_stream_cfg_t fs_stream_default_cfg(void);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `fs_stream_default_cfg()` is called | returns a value-initialized `fs_stream_cfg_t`;<br>`ret.reserved == 0` |

## Notes

- This helper establishes the default configuration invariant for the `fs_stream` adapter.

@anchor testing_foundation_fs_stream_unit_default_env
# fs_stream_default_env() unit tests

See @ref specifications_fs_stream_default_env "fs_stream_default_env() specifications"

## Functions under test

~~~c
fs_stream_env_t fs_stream_default_env(
    const osal_file_env_t *file_env,
    const osal_file_ops_t *file_ops,
    const stream_env_t *port_env);
~~~

## Success

- `env.file_env == *file_env`.
- `env.file_ops == file_ops`.
- `env.port_env == *port_env`.

## Failure

- None.

## Test doubles

- dummy `osal_file_env_t`
- dummy `osal_file_ops_t *`
- dummy `stream_env_t`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `fs_stream_default_env(file_env, file_ops, port_env)` is called with valid inputs | returns an `fs_stream_env_t` such that `env.file_env == *file_env`, `env.file_ops == file_ops`, and `env.port_env == *port_env` |

## Notes

- This helper preserves the injected file-layer and stream-port dependencies provided by the caller.

@anchor testing_foundation_fs_stream_unit_create_stream
# fs_stream_create_stream() unit tests

See @ref specifications_fs_stream_create_stream "fs_stream_create_stream() specifications"

## Functions under test

~~~c
stream_status_t fs_stream_create_stream(
    stream_t **out,
    const fs_stream_args_t *args,
    const fs_stream_cfg_t *cfg,
    const fs_stream_env_t *env);
~~~

## Invalid arguments

- `out`, `args`, `cfg`, `env` must not be `NULL`.
- `args->path` must not be `NULL` and must not be an empty string.
- `args->flags` must not be zero.

## Success

- Returns `STREAM_STATUS_OK`.
- Stores a valid stream in `*out`.
- The produced stream is ready for normal runtime use.
- The produced stream must be destroyed via `stream_destroy()`.

## Failure

- Returns:
    - `STREAM_STATUS_INVALID` for invalid arguments
    - `STREAM_STATUS_OOM` on allocation failure
    - `STREAM_STATUS_IO_ERROR` when OSAL file operations fail
- Leaves `*out` unchanged if `out` is not `NULL`.

## Test doubles

- `fake_memory`
- `fake_file`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `fs_stream_create_stream(out, args, cfg, env)` is called with valid arguments and OSAL file opening succeeds | returns `STREAM_STATUS_OK`;<br>stores a non-`NULL` stream handle in `*out`;<br>the produced stream is ready for normal runtime use |
| `out == NULL` | returns `STREAM_STATUS_INVALID`;<br>no stream handle is produced |
| `args == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `cfg == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `env == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `args != NULL` but `args->path == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `args != NULL` but `args->path` is an empty string and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `args != NULL` but `args->flags == 0` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| allocation required by `fs_stream_create_stream()` fails | returns `STREAM_STATUS_OOM`;<br>leaves `*out` unchanged |
| `fs_stream_create_stream(out, args, cfg, env)` is called with valid arguments but OSAL file opening fails | returns `STREAM_STATUS_IO_ERROR`;<br>leaves `*out` unchanged |

## Notes

- The nominal scenario also verifies file-backed runtime behavior through the public `stream` API:
    - `stream_write()` writes the expected payload,
    - the fake file backing contains the written bytes,
    - `stream_flush()` succeeds.
- OOM is injected through `fake_memory`.
- Open failure is injected through `fake_file`.

@anchor testing_foundation_fs_stream_unit_create_desc
# fs_stream_create_desc() unit tests

See @ref specifications_fs_stream_create_desc "fs_stream_create_desc() specifications"

## Functions under test

~~~c
stream_status_t fs_stream_create_desc(
    stream_adapter_desc_t *out,
    stream_key_t key,
    const fs_stream_cfg_t *cfg,
    const fs_stream_env_t *env,
    const osal_mem_ops_t *mem);
~~~

## Invalid arguments

- `out`, `key`, `cfg`, `env`, `mem` must not be `NULL`.
- `key` must not be an empty string.
- `env->file_env.mem` and `env->file_ops` must not be `NULL`.

## Success

- Returns `STREAM_STATUS_OK`.
- Stores a valid adapter descriptor in `*out`.
- The produced descriptor must later be released via `out->ud_dtor()`.

## Failure

- Returns:
    - `STREAM_STATUS_INVALID` for invalid arguments
    - `STREAM_STATUS_OOM` on allocation failure
- If `out` is not `NULL`, resets `*out` to an empty descriptor.

## Test doubles

- `fake_memory`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `fs_stream_create_desc(out, key, cfg, env, mem)` is called with valid arguments | returns `STREAM_STATUS_OK`;<br>stores a valid adapter descriptor in `*out`;<br>the produced descriptor is eligible for later destruction via `out->ud_dtor()` |
| `out == NULL` | returns `STREAM_STATUS_INVALID`;<br>no descriptor is produced |
| `key == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>resets `*out` to an empty descriptor |
| `key` is an empty string and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>resets `*out` to an empty descriptor |
| `cfg == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>resets `*out` to an empty descriptor |
| `env == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>resets `*out` to an empty descriptor |
| `mem == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>resets `*out` to an empty descriptor |
| `env != NULL` but `env->file_env.mem == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>resets `*out` to an empty descriptor |
| `env != NULL` but `env->file_ops == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>resets `*out` to an empty descriptor |
| allocation required by `fs_stream_create_desc()` fails | returns `STREAM_STATUS_OOM`;<br>resets `*out` to an empty descriptor |

## Notes

- The invalid-argument and OOM scenarios verify the “empty descriptor on failure” postcondition.
- A valid descriptor is expected to expose non-`NULL` `key`, `ctor`, `ud`, and `ud_dtor` fields.

@anchor testing_foundation_fs_stream_unit_desc_ctor
# descriptor constructor usage through fs_stream_create_desc() unit tests

See @ref specifications_fs_stream_create_desc "fs_stream_create_desc() specifications"

## Functions under test

~~~c
stream_status_t desc.ctor(
    const void *ud,
    const fs_stream_args_t *args,
    stream_t **out);
~~~

## Invalid arguments

- `args` and `out` must not be `NULL`.
- `args->path` must not be `NULL` and must not be an empty string.
- `args->flags` must not be zero.

## Success

- Returns `STREAM_STATUS_OK`.
- Stores a valid stream in `*out`.
- The produced stream is ready for normal runtime use.
- The produced stream must be destroyed via `stream_destroy()`.

## Failure

- Returns:
    - `STREAM_STATUS_INVALID` for invalid arguments
    - `STREAM_STATUS_OOM` on allocation failure
    - `STREAM_STATUS_IO_ERROR` when OSAL file operations fail
- Leaves `*out` unchanged if `out` is not `NULL`.

## Test doubles

- `fake_memory`
- `fake_file`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `desc.ctor(ud, args, out)` is called with valid arguments and OSAL file opening succeeds | returns `STREAM_STATUS_OK`;<br>stores a non-`NULL` stream handle in `*out`;<br>the produced stream is ready for normal runtime use |
| `args == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `out == NULL` | returns `STREAM_STATUS_INVALID`;<br>no stream handle is produced |
| `args != NULL` but `args->path == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `args != NULL` but `args->path` is an empty string and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `args != NULL` but `args->flags == 0` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| allocation required by `desc.ctor()` fails | returns `STREAM_STATUS_OOM`;<br>leaves `*out` unchanged |
| `desc.ctor(ud, args, out)` is called with valid arguments but OSAL file opening fails | returns `STREAM_STATUS_IO_ERROR`;<br>leaves `*out` unchanged |

## Notes

- These tests validate constructor usage through the descriptor produced by `fs_stream_create_desc()`.
- The nominal scenario also verifies file-backed runtime behavior through the public `stream` API:
    - `stream_write()` writes the expected payload,
    - the fake file backing contains the written bytes,
    - `stream_flush()` succeeds.
- OOM is injected through `fake_memory`.
- Open failure is injected through `fake_file`.

