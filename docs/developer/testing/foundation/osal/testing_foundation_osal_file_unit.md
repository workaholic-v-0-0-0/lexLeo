@page testing_foundation_osal_file_unit osal_file unit tests

It covers:
- `osal_file_default_ops()`
- `osal_file_ops_t::open()`

---

@anchor testing_foundation_osal_file_unit_default_ops
# osal_file_default_ops() unit tests

See @ref specifications_osal_file_default_ops "osal_file_default_ops() specifications"

## Functions under test

~~~c
const osal_file_ops_t *osal_file_default_ops(void);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_file_default_ops()` is called | returns a non-`NULL` pointer to an `osal_file_ops_t`;<br>`ret->open != NULL`;<br>`ret->read != NULL`;<br>`ret->write != NULL`;<br>`ret->flush != NULL`;<br>`ret->close != NULL` |

## Notes

- This helper exposes the default low-level OSAL file operations table for the
  active platform.
- The returned table is expected to provide a well-formed set of callable
  low-level file operations.

---

@anchor testing_foundation_osal_file_unit_open
# osal_file_ops_t::open() unit tests

See @ref specifications_osal_file_open "osal_file_ops_t::open specifications"

## Functions under test

~~~c
osal_file_status_t (*open)(
    OSAL_FILE **out,
    const char *pathname,
    const char *mode,
    const osal_mem_ops_t *mem_ops);
~~~

## Test doubles

- fake memory provider via `osal_mem_test_fake_ops()`
- `fake_memory`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_file_ops_t::open()` is called with `out == NULL` | returns `OSAL_FILE_STATUS_INVALID`;<br>no `OSAL_FILE` handle is produced |
| `osal_file_ops_t::open()` is called with `pathname == NULL` | returns `OSAL_FILE_STATUS_INVALID`;<br>leaves `*out` unchanged if `out != NULL`;<br>no `OSAL_FILE` handle is produced |
| `osal_file_ops_t::open()` is called with `pathname[0] == '\0'` | returns `OSAL_FILE_STATUS_INVALID`;<br>leaves `*out` unchanged if `out != NULL`;<br>no `OSAL_FILE` handle is produced |
| `osal_file_ops_t::open()` is called with `mode == NULL` | returns `OSAL_FILE_STATUS_INVALID`;<br>leaves `*out` unchanged if `out != NULL`;<br>no `OSAL_FILE` handle is produced |
| `osal_file_ops_t::open()` is called with an unsupported mode | returns `OSAL_FILE_STATUS_INVALID`;<br>leaves `*out` unchanged if `out != NULL`;<br>no `OSAL_FILE` handle is produced |
| `osal_file_ops_t::open()` is called with `mem_ops == NULL` | returns `OSAL_FILE_STATUS_INVALID`;<br>leaves `*out` unchanged if `out != NULL`;<br>no `OSAL_FILE` handle is produced |
| allocation of the OSAL wrapper fails during `osal_file_ops_t::open()` | returns `OSAL_FILE_STATUS_OOM`;<br>leaves `*out` unchanged if `out != NULL`;<br>no `OSAL_FILE` handle is produced |
| `osal_file_ops_t::open()` is called with valid arguments | returns `OSAL_FILE_STATUS_OK`;<br>stores a non-`NULL` `OSAL_FILE` handle in `*out` |

## Notes

- These tests validate the public acquisition contract of
  `osal_file_ops_t::open()`.
- The OOM scenario is exercised through the injected memory operations table
  and `fake_memory` fail injection.
- Success requires a valid pathname, a supported portable mode, and a valid
  injected memory operations table.
- Supported portable modes covered by the contract are `"rb"`, `"wb"`, and
  `"ab"`.

---
