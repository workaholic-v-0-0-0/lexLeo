@page testing_foundation_fs_stream_unit fs_stream unit tests

It covers:
- `fs_stream_default_cfg()`
- `fs_stream_default_env()`
- `fs_stream_create_stream()`
- `fs_stream_create_desc()`
- `fs_stream_write()`
- `fs_stream_read()`
- `fs_stream_flush()`
- `fs_stream_close()`

---

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

---

@anchor testing_foundation_fs_stream_unit_default_env
# fs_stream_default_env() unit tests

See @ref specifications_fs_stream_default_env "fs_stream_default_env() specifications"

## Functions under test

~~~c
fs_stream_env_t fs_stream_default_env(
    const osal_file_ops_t *file_ops,
    const osal_mem_ops_t *adapter_mem_ops,
    const stream_env_t *port_env);
~~~

## Success

- `env.file_ops == file_ops`.
- `env.adapter_mem_ops == adapter_mem_ops`.
- `env.port_env == *port_env`.

## Failure

- None.

## Test doubles

- dummy `osal_file_ops_t *`
- dummy `osal_mem_ops_t *`
- dummy `stream_env_t`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `fs_stream_default_env(file_ops, adapter_mem_ops, port_env)` is called with valid inputs | returns an `fs_stream_env_t` such that `env.file_ops == file_ops`, `env.adapter_mem_ops == adapter_mem_ops`, and `env.port_env == *port_env` |

## Notes

- This helper preserves the injected OSAL file, adapter memory, and stream-port dependencies provided by the caller.

---

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
- `args->mode` must not be `NULL`.
- `args->mode` must be one of `"rb"`, `"wb"`, or `"ab"`.

## Success

- Returns `STREAM_STATUS_OK`.
- Stores a valid stream in `*out`.
- The produced stream is ready for normal runtime use.
- The produced stream must be destroyed via `stream_destroy()`.

## Failure

- Returns:
    - `STREAM_STATUS_INVALID` for invalid arguments
    - `STREAM_STATUS_OOM` on allocation failure
    - `STREAM_STATUS_IO_ERROR` when OSAL file opening fails
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
| `args != NULL` but `args->mode == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `args != NULL` but `args->mode` is not supported and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| allocation required by `fs_stream_create_stream()` fails | returns `STREAM_STATUS_OOM`;<br>leaves `*out` unchanged |
| `fs_stream_create_stream(out, args, cfg, env)` is called with valid arguments but OSAL file opening fails | returns `STREAM_STATUS_IO_ERROR`;<br>leaves `*out` unchanged |

## Notes

- The nominal scenario also verifies file-backed runtime behavior through the public `stream` API:
    - `stream_write()` writes the expected payload,
    - the fake file buffered backing contains the written bytes,
    - `stream_flush()` succeeds,
    - the fake file sink backing contains the flushed bytes.
- OOM is injected through `fake_memory`.
- Open failure is injected through `fake_file`.

---

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
- dummy `osal_file_ops_t *`

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
| allocation required by `fs_stream_create_desc()` fails | returns `STREAM_STATUS_OOM`;<br>resets `*out` to an empty descriptor |

## Notes

- The invalid-argument and OOM scenarios verify the “empty descriptor on failure” postcondition.
- A valid descriptor is expected to expose non-`NULL` `key`, `ctor`, `ud`, and `ud_dtor` fields.

---

@anchor testing_foundation_fs_stream_unit_write
# fs_stream_write() unit tests

See @ref specifications_fs_stream_write "fs_stream_write() specifications"

## Functions under test

~~~c
size_t stream_write(
    stream_t *s,
    const void *buf,
    size_t n,
    stream_status_t *st);
~~~

## Callback exercised

~~~c
static size_t fs_stream_write(
    void *backend,
    const void *buf,
    size_t n,
    stream_status_t *st);
~~~

## Default setup

Unless stated otherwise:

- `s` is created by `fs_stream_create_stream()` in `"wb"` mode.
- The opened OSAL file is provided by `fake_file`.
- `buf != NULL`.
- `n == FS_STREAM_WRITE_TEST_MSG_LEN`.
- The fake file buffered backing is initially empty.

## Success

- Returns the number of bytes reported by the underlying OSAL file write operation.
- If `st != NULL`, stores the mapped `stream_status_t`.
- Delegates the write operation to the injected OSAL file write operation.

## Failure

- Returns the number of bytes reported by the underlying OSAL file write operation.
- If `st != NULL`, stores the mapped failure status.

## Test doubles

- `fake_memory`
- `fake_file`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `stream_write(s, buf, n, st)` is called and the injected OSAL file write operation reports `OSAL_FILE_STATUS_IO` | returns `0`;<br>sets `*st = STREAM_STATUS_IO_ERROR`;<br>calls the injected OSAL file write operation exactly once;<br>leaves the fake file buffered backing empty |
| `stream_write(s, buf, n, NULL)` is called and the injected OSAL file write operation reports `OSAL_FILE_STATUS_IO` | returns `0`;<br>calls the injected OSAL file write operation exactly once;<br>leaves the fake file buffered backing empty |
| `stream_write(s, buf, n, st)` is called and the injected OSAL file write operation reports `OSAL_FILE_STATUS_OK` | returns `FS_STREAM_WRITE_TEST_MSG_LEN`;<br>sets `*st = STREAM_STATUS_OK`;<br>calls the injected OSAL file write operation exactly once;<br>writes `FS_STREAM_WRITE_TEST_MSG` into the fake file buffered backing |
| `stream_write(s, buf, n, NULL)` is called and the injected OSAL file write operation reports `OSAL_FILE_STATUS_OK` | returns `FS_STREAM_WRITE_TEST_MSG_LEN`;<br>calls the injected OSAL file write operation exactly once;<br>writes `FS_STREAM_WRITE_TEST_MSG` into the fake file buffered backing |

## Notes

- These tests exercise `fs_stream_write()` through the public `stream_write()` API.
- The oracle combines public return/status observation with fake-file interaction checks.

---

@anchor testing_foundation_fs_stream_unit_read
# fs_stream_read() unit tests

See @ref specifications_fs_stream_read "fs_stream_read() specifications"

## Functions under test

~~~c
size_t stream_read(
    stream_t *s,
    void *buf,
    size_t n,
    stream_status_t *st);
~~~

## Callback exercised

~~~c
static size_t fs_stream_read(
    void *backend,
    void *buf,
    size_t n,
    stream_status_t *st);
~~~

## Default setup

Unless stated otherwise:

- `s` is created by `fs_stream_create_stream()` in `"rb"` mode.
- The opened OSAL file is provided by `fake_file`.
- `buf != NULL`.
- `n == FS_STREAM_READ_TEST_MSG_LEN`.
- The fake file sink backing initially contains `FS_STREAM_READ_TEST_MSG`.
- Opening the fake file in `"rb"` mode makes that payload available for reading.

## Success

- Returns the number of bytes reported by the underlying OSAL file read operation.
- If `st != NULL`, stores the mapped `stream_status_t`.
- Delegates the read operation to the injected OSAL file read operation.

## Failure

- Returns the number of bytes reported by the underlying OSAL file read operation.
- If `st != NULL`, stores the mapped failure status.

## Test doubles

- `fake_memory`
- `fake_file`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `stream_read(s, buf, n, st)` is called and the injected OSAL file read operation reports `OSAL_FILE_STATUS_IO` | returns `0`;<br>sets `*st = STREAM_STATUS_IO_ERROR`;<br>calls the injected OSAL file read operation exactly once;<br>leaves `buf` unchanged |
| `stream_read(s, buf, n, NULL)` is called and the injected OSAL file read operation reports `OSAL_FILE_STATUS_IO` | returns `0`;<br>calls the injected OSAL file read operation exactly once;<br>leaves `buf` unchanged |
| `stream_read(s, buf, FS_STREAM_READ_TEST_EOF_N, st)` is called and the injected OSAL file read operation reports `OSAL_FILE_STATUS_EOF` | returns `FS_STREAM_READ_TEST_MSG_LEN`;<br>sets `*st = STREAM_STATUS_EOF`;<br>calls the injected OSAL file read operation exactly once;<br>copies exactly `FS_STREAM_READ_TEST_MSG` into `buf` |
| `stream_read(s, buf, FS_STREAM_READ_TEST_EOF_N, NULL)` is called and the injected OSAL file read operation reports `OSAL_FILE_STATUS_EOF` | returns `FS_STREAM_READ_TEST_MSG_LEN`;<br>calls the injected OSAL file read operation exactly once;<br>copies exactly `FS_STREAM_READ_TEST_MSG` into `buf` |
| `stream_read(s, buf, FS_STREAM_READ_TEST_MSG_LEN, st)` is called and the injected OSAL file read operation reports `OSAL_FILE_STATUS_OK` | returns `FS_STREAM_READ_TEST_MSG_LEN`;<br>sets `*st = STREAM_STATUS_OK`;<br>calls the injected OSAL file read operation exactly once;<br>copies exactly `FS_STREAM_READ_TEST_MSG` into `buf` |
| `stream_read(s, buf, FS_STREAM_READ_TEST_MSG_LEN, NULL)` is called and the injected OSAL file read operation reports `OSAL_FILE_STATUS_OK` | returns `FS_STREAM_READ_TEST_MSG_LEN`;<br>calls the injected OSAL file read operation exactly once;<br>copies exactly `FS_STREAM_READ_TEST_MSG` into `buf` |

## Notes

- These tests exercise `fs_stream_read()` through the public `stream_read()` API.
- The oracle combines public return/status observation with fake-file interaction checks.
- EOF behavior is injected through `fake_file`; detailed `fread()` / EOF semantics belong to `osal_file` tests.

---

@anchor testing_foundation_fs_stream_unit_flush
# fs_stream_flush() unit tests

See @ref specifications_fs_stream_flush "fs_stream_flush() specifications"

## Functions under test

~~~c
stream_status_t stream_flush(stream_t *s);
~~~

## Callback exercised

~~~c
static stream_status_t fs_stream_flush(void *backend);
~~~

## Default setup

Unless stated otherwise:

- `s` is created by `fs_stream_create_stream()` in `"wb"` mode.
- The opened OSAL file is provided by `fake_file`.

## Success

- Returns the mapped `stream_status_t` corresponding to the status reported by the underlying OSAL file flush operation.
- Delegates the flush operation to the injected OSAL file flush operation.

## Failure

- Returns the mapped failure status reported by the underlying OSAL file flush operation.

## Test doubles

- `fake_memory`
- `fake_file`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `stream_flush(s)` is called and the injected OSAL file flush operation reports `OSAL_FILE_STATUS_IO` | returns `STREAM_STATUS_IO_ERROR`;<br>calls the injected OSAL file flush operation exactly once |
| `stream_flush(s)` is called and the injected OSAL file flush operation reports `OSAL_FILE_STATUS_OK` | returns `STREAM_STATUS_OK`;<br>calls the injected OSAL file flush operation exactly once |

## Notes

- These tests exercise `fs_stream_flush()` through the public `stream_flush()` API.
- The oracle combines public return/status observation with fake-file interaction checks.

---

@anchor testing_foundation_fs_stream_unit_close
# fs_stream_close() unit tests

See @ref specifications_fs_stream_close "fs_stream_close() specifications"

## Functions under test

~~~c
stream_status_t stream_destroy(stream_t **s);
~~~

## Callback exercised

~~~c
static stream_status_t fs_stream_close(void *backend);
~~~

## Default setup

Unless stated otherwise:

- `s` is created by `fs_stream_create_stream()` in `"wb"` mode.
- The opened OSAL file is provided by `fake_file`.

## Success

- Returns the mapped `stream_status_t` corresponding to the status reported by the underlying OSAL file close operation.
- Delegates the close operation to the injected OSAL file close operation.
- Releases the public stream handle only when close succeeds.

## Failure

- Returns the mapped failure status reported by the underlying OSAL file close operation.
- Leaves the public stream handle unchanged when close fails.

## Test doubles

- `fake_memory`
- `fake_file`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `stream_destroy(&s)` is called and the injected OSAL file close operation reports `OSAL_FILE_STATUS_IO` | returns `STREAM_STATUS_IO_ERROR`;<br>calls the injected OSAL file close operation exactly once;<br>leaves `s` unchanged |
| `stream_destroy(&s)` is called and the injected OSAL file close operation reports `OSAL_FILE_STATUS_OK` | returns `STREAM_STATUS_OK`;<br>calls the injected OSAL file close operation exactly once;<br>releases the public stream handle;<br>sets `s` to `NULL` |

## Notes

- These tests exercise `fs_stream_close()` through the public `stream_destroy()` API.
- The oracle combines public return/status observation with fake-file interaction checks.
