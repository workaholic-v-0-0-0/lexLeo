@page testing_foundation_fs_stream_cr_unit fs_stream_cr.c unit tests

---

@anchor testing_foundation_fs_stream_unit_fs_stream_vtbl

# `fs_stream_vtbl()` unit tests

See @ref specifications_fs_stream_vtbl "fs_stream_vtbl() specifications"

## Function under test

~~~c
const stream_vtbl_t *fs_stream_vtbl(void);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `fs_stream_vtbl()` is called | returns a non-`NULL` virtual table whose `read`, `write`, `flush`, and `close` operations are all non-`NULL` |

---

@anchor testing_foundation_fs_stream_unit_fs_stream_default_cfg

# `fs_stream_default_cfg()` unit tests

See @ref specifications_fs_stream_default_cfg "fs_stream_default_cfg() specifications"

## Function under test

~~~c
fs_stream_cfg_t fs_stream_default_cfg(void);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `fs_stream_default_cfg()` is called | returns a configuration whose `reserved` field is equal to `0` |

---

@anchor testing_foundation_fs_stream_unit_fs_stream_default_env

# `fs_stream_default_env()` unit tests

See @ref specifications_fs_stream_default_env "fs_stream_default_env() specifications"

## Function under test

~~~c
fs_stream_env_t fs_stream_default_env(
    const osal_file_ops_t *file_ops,
    const osal_mem_ops_t *mem_ops);
~~~

## Test doubles

- dummy OSAL file operations pointer
- dummy OSAL memory operations pointer

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| `fs_stream_default_env(file_ops, mem_ops)` is called | returns an environment whose `file_ops` and `mem_ops` members contain the supplied pointers unchanged |

---

@anchor testing_foundation_fs_stream_unit_fs_stream_t_lifecycle

# `fs_stream_t` lifecycle unit tests

See:

- @ref specifications_fs_stream_create "fs_stream_create() specifications"
- @ref specifications_fs_stream_complete_default_init "fs_stream_complete_default_init() specifications"
- @ref specifications_fs_stream_close "fs_stream_close() specifications"
- @ref specifications_fs_stream_vtbl "fs_stream_vtbl() specifications"
- @ref specifications_fs_stream_default_cfg "fs_stream_default_cfg() specifications"

## Functions under test

~~~c
fs_stream_status_t fs_stream_create(
    fs_stream_t **out,
    const fs_stream_env_t *env);

fs_stream_status_t fs_stream_complete_default_init(
    fs_stream_t *fs_stream,
    const fs_stream_cfg_t *cfg,
    const stream_regular_file_creator_args_t *args);
~~~

The backend is destroyed through the `close` operation returned by
`fs_stream_vtbl()`.

## Test doubles

- fake OSAL file operations
- fake OSAL memory operations

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| allocation of the `fs_stream_t` handle fails | `fs_stream_create()` returns `FS_STREAM_STATUS_OOM`, leaves `*out` unchanged, and causes no memory leak, invalid free, or double free |
| creation of the `fs_stream_t` handle succeeds | `fs_stream_create()` returns `FS_STREAM_STATUS_OK`, stores a valid non-`NULL` handle in `*out`, preserves the supplied file and memory operation tables, and initializes the owned OSAL file handle to `NULL` |
| the OSAL file open operation returns `OSAL_FILE_STATUS_OOM` during default initialization | `fs_stream_complete_default_init()` invokes the OSAL file open operation exactly once with the expected path, mode, and memory operations, returns `FS_STREAM_STATUS_OOM`, leaves the owned file handle `NULL`, and preserves the injected operation tables |
| the OSAL file open operation returns `OSAL_FILE_STATUS_PERM` during default initialization | `fs_stream_complete_default_init()` invokes the OSAL file open operation exactly once with the expected path, mode, and memory operations, maps the error to `FS_STREAM_STATUS_IO_ERROR`, leaves the owned file handle `NULL`, and preserves the injected operation tables |
| `args->path == NULL` | `fs_stream_complete_default_init()` returns `FS_STREAM_STATUS_INVALID`, leaves the owned file handle `NULL`, and preserves the injected operation tables |
| `args->path` is an empty string | `fs_stream_complete_default_init()` returns `FS_STREAM_STATUS_INVALID`, leaves the owned file handle `NULL`, and preserves the injected operation tables |
| the length of `args->path` exceeds the supported maximum | `fs_stream_complete_default_init()` returns `FS_STREAM_STATUS_INVALID`, leaves the owned file handle `NULL`, and preserves the injected operation tables |
| an OSAL file handle has already been injected before default initialization | `fs_stream_complete_default_init()` returns `FS_STREAM_STATUS_OK`, preserves the injected file handle, does not invoke the OSAL file open operation, and preserves the injected operation tables |
| no OSAL file handle has been injected and the OSAL file open operation succeeds | `fs_stream_complete_default_init()` returns `FS_STREAM_STATUS_OK`, invokes the OSAL file open operation exactly once with the expected path, mode, and memory operations, stores the created OSAL file handle, and preserves the injected operation tables |
| destruction is performed after successful initialization | the `close` operation closes the owned OSAL file exactly once when one is present, returns `STREAM_STATUS_OK`, and releases the `fs_stream_t` handle without memory leak, invalid free, or double free |
| the underlying OSAL file close operation returns `OSAL_FILE_STATUS_IO` | the `close` operation invokes the OSAL file close operation exactly once, maps the error to `STREAM_STATUS_IO_ERROR`, and still releases the `fs_stream_t` handle without memory leak, invalid free, or double free |

---

@anchor testing_foundation_fs_stream_unit_stream_adapter_provider_t_lifecycle

# `stream_adapter_provider_t` lifecycle unit tests

See:

- @ref specifications_fs_stream_create_adapter_provider "fs_stream_create_adapter_provider() specifications"
- @ref specifications_stream_destroy_adapter_provider "stream_destroy_adapter_provider() specifications"

## Functions under test

~~~c
fs_stream_status_t fs_stream_create_adapter_provider(
    stream_adapter_provider_t **out,
    const fs_stream_cfg_t *cfg,
    const fs_stream_env_t *env);

void stream_destroy_adapter_provider(
    stream_adapter_provider_t *provider);
~~~

## Test doubles

- fake OSAL file operations
- fake OSAL memory operations

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| allocation required by `fs_stream_create_adapter_provider()` fails | returns `FS_STREAM_STATUS_OOM`, leaves `*out` unchanged, and causes no memory leak, invalid free, or double free |
| adapter provider creation succeeds | returns `FS_STREAM_STATUS_OK`, stores a valid non-`NULL` provider in `*out`, initializes a non-`NULL` backend constructor, uses the `fs_stream` virtual table, stores non-`NULL` constructor user data and destructor, and uses the expected memory operations |
| a successfully created adapter provider is destroyed | `stream_destroy_adapter_provider()` releases all provider-owned resources without memory leak, invalid free, or double free |
