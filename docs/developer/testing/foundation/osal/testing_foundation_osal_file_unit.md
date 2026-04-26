@page testing_foundation_osal_file_unit osal_file unit tests

It covers:
- `osal_file_default_ops()`
- `osal_file_ops_t::open()`
- `osal_file_ops_t::write()`
- `osal_file_ops_t::read()`
- `osal_file_ops_t::flush()`
- `osal_file_ops_t::close()`
- `osal_file_gets()`

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

@anchor testing_foundation_osal_file_unit_read
# osal_file_ops_t::read() unit tests

See @ref specifications_osal_file_read "osal_file_ops_t::read specifications"

## Functions under test

~~~c
size_t (*read)(
    void *ptr,
    size_t size,
    size_t nmemb,
    OSAL_FILE *stream,
    osal_file_status_t *st);
~~~

## Test doubles

- fake memory provider via `osal_mem_test_fake_ops()`
- `fake_memory`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_file_ops_t::read()` is called with `ptr == NULL` | returns `0`;<br>sets `*st` to `OSAL_FILE_STATUS_INVALID` if `st != NULL` |
| `osal_file_ops_t::read()` is called with `st == NULL` | returns `0`;<br>no status is written |
| `osal_file_ops_t::read()` is called with `stream == NULL` | returns `0`;<br>sets `*st` to `OSAL_FILE_STATUS_INVALID` if `st != NULL` |
| `osal_file_ops_t::read()` is called with valid arguments and the requested element count is smaller than the available file content | returns `nmemb`;<br>copies the requested elements into `ptr`;<br>sets `*st` to `OSAL_FILE_STATUS_OK` |
| `osal_file_ops_t::read()` is called with valid arguments and the requested element count exactly matches the available file content | returns `nmemb`;<br>copies the requested elements into `ptr`;<br>sets `*st` to `OSAL_FILE_STATUS_OK` |
| `osal_file_ops_t::read()` is called with valid arguments and the requested element count is greater than the available file content | returns a value smaller than `nmemb`;<br>copies the available elements into `ptr`;<br>sets `*st` to `OSAL_FILE_STATUS_OK` |
| `osal_file_ops_t::read()` is called with valid arguments and the file is empty | returns `0`;<br>sets `*st` to `OSAL_FILE_STATUS_OK` |
| `osal_file_ops_t::read()` is called multiple times on the same open `OSAL_FILE`, first with a request smaller than the available file content, then again with another request smaller than the remaining file content | the first call returns the requested element count;<br>the second call returns the requested element count;<br>each call copies the expected data from the current file position into `ptr`;<br>each call sets `*st` to `OSAL_FILE_STATUS_OK` |
| `osal_file_ops_t::read()` is called after end-of-file has already been reached on the same open `OSAL_FILE` | returns `0`;<br>reads no additional data into `ptr`;<br>sets `*st` to `OSAL_FILE_STATUS_OK` |
| `osal_file_ops_t::read()` is called with valid arguments and `size == 0` | returns `0`;<br>reads no data into `ptr`;<br>sets `*st` to `OSAL_FILE_STATUS_OK` |
| `osal_file_ops_t::read()` is called with valid arguments and `nmemb == 0` | returns `0`;<br>reads no data into `ptr`;<br>sets `*st` to `OSAL_FILE_STATUS_OK` |
| `osal_file_ops_t::read()` is called with valid arguments, `size > 1`, and the requested element count is smaller than the number of complete elements available in the file | returns `nmemb`;<br>copies the requested complete elements into `ptr`;<br>sets `*st` to `OSAL_FILE_STATUS_OK` |
| `osal_file_ops_t::read()` is called with valid arguments, `size > 1`, and the requested element count exactly matches the number of complete elements available in the file | returns `nmemb`;<br>copies the requested complete elements into `ptr`;<br>sets `*st` to `OSAL_FILE_STATUS_OK` |
| `osal_file_ops_t::read()` is called with valid arguments, `size > 1`, and the requested element count is greater than the number of complete elements available in the file | returns a value smaller than `nmemb`;<br>copies the available complete elements into `ptr`;<br>sets `*st` to `OSAL_FILE_STATUS_OK` |

## Notes

- These tests validate the public read contract of `osal_file_ops_t::read()`.
- The initial file content is prepared before the tested read sequence starts.
- The tested `read()` calls are exercised through the default operations table returned by `osal_file_default_ops()`.
- The file preparation path uses the public OSAL file operations table together with the injected memory operations table.
- These tests exercise deterministic reads on real temporary files rather than backend doubles.
- The successful short-read scenarios covered here are end-of-file driven cases, not backend failure cases.
- For `size > 1` scenarios, expectations are expressed in complete elements, not bytes.

---

@anchor testing_foundation_osal_file_unit_write
# osal_file_ops_t::write() unit tests

See @ref specifications_osal_file_write "osal_file_ops_t::write specifications"

## Functions under test

~~~c
size_t (*write)(
    const void *ptr,
    size_t size,
    size_t nmemb,
    OSAL_FILE *stream,
    osal_file_status_t *st);
~~~

## Test doubles

- fake memory provider via `osal_mem_test_fake_ops()`
- `fake_memory`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_file_ops_t::write()` is called with `ptr == NULL` | returns `0`;<br>sets `*st` to `OSAL_FILE_STATUS_INVALID` if `st != NULL` |
| `osal_file_ops_t::write()` is called with `stream == NULL` | returns `0`;<br>sets `*st` to `OSAL_FILE_STATUS_INVALID` if `st != NULL` |
| `osal_file_ops_t::write()` is called with `st == NULL` | returns `0`;<br>no status can be stored |
| `osal_file_ops_t::write()` is called with `size == 0` | returns `0`;<br>sets `*st` to `OSAL_FILE_STATUS_OK`;<br>writes no data |
| `osal_file_ops_t::write()` is called with `nmemb == 0` | returns `0`;<br>sets `*st` to `OSAL_FILE_STATUS_OK`;<br>writes no data |
| `osal_file_ops_t::write()` is called with `nmemb == 1` on a valid writable stream | returns `1`;<br>sets `*st` to `OSAL_FILE_STATUS_OK`;<br>writes the requested data |
| `osal_file_ops_t::write()` is called with multiple elements of size `1` on a valid writable stream | returns `nmemb`;<br>sets `*st` to `OSAL_FILE_STATUS_OK`;<br>writes the requested data |
| `osal_file_ops_t::write()` is called with multiple elements of size greater than `1` on a valid writable stream | returns `nmemb`;<br>sets `*st` to `OSAL_FILE_STATUS_OK`;<br>writes the requested data |
| `osal_file_ops_t::write()` is called twice successively on the same valid writable stream | each call returns its requested element count;<br>each call sets `*st` to `OSAL_FILE_STATUS_OK`;<br>the final file content reflects both writes in order |

## Notes

- These tests validate the public write contract of `osal_file_ops_t::write()`.
- The initial file content is prepared before the tested write sequence starts.
- The tested `write()` calls are exercised through the default operations table returned by `osal_file_default_ops()`.
- The file verification path reopens the temporary file in read mode after the tested write sequence completes.
- The file preparation and verification steps use the public OSAL file operations table together with the injected memory operations table.
- These tests exercise deterministic writes on real temporary files rather than backend doubles.
- For `size > 1` scenarios, expectations are expressed in complete elements, not bytes.
- The sequential scenario validates cumulative effects on the same writable stream across successive `write()` calls.

---

@anchor testing_foundation_osal_file_unit_flush
# osal_file_ops_t::flush() unit tests

See @ref specifications_osal_file_flush "osal_file_ops_t::flush specifications"

## Functions under test

~~~c
osal_file_status_t (*flush)(OSAL_FILE *stream);
~~~

## Test doubles

- fake memory provider via `osal_mem_test_fake_ops()`
- `fake_memory`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_file_ops_t::flush()` is called with `stream == NULL` | returns `OSAL_FILE_STATUS_INVALID` |
| `osal_file_ops_t::flush()` is called on a valid writable stream | returns `OSAL_FILE_STATUS_OK` |

## Notes

- These tests validate the public flush contract of `osal_file_ops_t::flush()`.
- The initial file content is written before the tested `flush()` call when
  the scenario uses a valid writable stream.
- The tested `flush()` call is exercised through the default operations table
  returned by `osal_file_default_ops()`.
- The file preparation path uses the public OSAL file operations table together
  with the injected memory operations table.
- These tests exercise deterministic flush calls on real temporary files rather
  than backend doubles.
- The current suite validates the observable status contract of `flush()`;
  it does not attempt to prove backend-specific buffering effects beyond that
  public result.

---

@anchor testing_foundation_osal_file_unit_close
# osal_file_ops_t::close() unit tests

See @ref specifications_osal_file_close "osal_file_ops_t::close specifications"

## Functions under test

~~~c
osal_file_status_t (*close)(OSAL_FILE *stream);
~~~

## Test doubles

- fake memory provider via `osal_mem_test_fake_ops()`
- `fake_memory`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_file_ops_t::close()` is called with `stream == NULL` | returns `OSAL_FILE_STATUS_INVALID` |
| `osal_file_ops_t::close()` is called on a valid open stream | returns `OSAL_FILE_STATUS_OK` |

## Notes

- These tests validate the public close contract of `osal_file_ops_t::close()`.
- The tested `close()` call is exercised through the default operations table
  returned by `osal_file_default_ops()`.
- The stream under test is acquired during fixture setup through
  `osal_file_ops_t::open()`.
- The fixture teardown closes the stream only if it was not already consumed by
  the tested `close()` call.
- The setup and teardown paths use the public OSAL file operations table
  together with the injected memory operations table.
- These tests exercise deterministic close calls on real temporary files rather
  than backend doubles.
- The current suite validates the observable status contract of `close()`;
  it does not attempt to prove backend-specific close failure mappings beyond
  that public result.

---

@anchor testing_foundation_osal_file_unit_gets
# osal_file_gets() unit tests

See @ref specifications_osal_file_gets "osal_file_gets specifications"

## Functions under test

~~~c
char *osal_file_gets(
    char *out,
    size_t out_size,
    OSAL_FILE *stream,
    osal_file_status_t *st);
~~~

## Test doubles

- fake memory provider via `osal_mem_test_fake_ops()`
- `fake_memory`

## Tested scenarios

| WHEN                                                                                     | EXPECT                                                      |
|------------------------------------------------------------------------------------------|-------------------------------------------------------------|
| `osal_file_gets()` is called with `out == NULL`                                          | returns `NULL`;<br>sets `*st` to `OSAL_FILE_STATUS_INVALID` |
| `osal_file_gets()` is called with `stream == NULL`                                       | returns `NULL`;<br>sets `*st` to `OSAL_FILE_STATUS_INVALID` |
| `osal_file_gets()` is called with `st == NULL`                                           | returns `NULL`;<br>sets `*st` to `OSAL_FILE_STATUS_INVALID` |
| `osal_file_gets()` is called on a valid readable stream with no data to read             | returns `NULL`;<br>sets `*st` to `OSAL_FILE_STATUS_OK`      |
| `osal_file_gets()` is called on a valid readable stream with a string but no newline     | returns `out`;<br>sets `*st` to `OSAL_FILE_STATUS_OK`;<br>the read string is copied at out       |
| `osal_file_gets()` is called on a valid readable stream with a string ending with `\n`   | returns `out`;<br>sets `*st` to `OSAL_FILE_STATUS_OK`;<br>the read string is copied at out       |
| `osal_file_gets()` is called twice on a valid readable stream containing "abc\n123\nABC" | returns `out`;<br>sets `*st` to `OSAL_FILE_STATUS_OK`;<br>the read string is copied at out       |

---

@anchor testing_foundation_osal_file_unit_mkdir
# osal_file_mkdir() unit tests

See @ref specifications_osal_file_mkdir "osal_file_mkdir specifications"

## Functions under test

~~~c
osal_file_status_t osal_file_mkdir(const char *pathname);
~~~

## Test doubles

- None

## Tested scenarios

| WHEN                                                                                    | EXPECT                                                      |
|-----------------------------------------------------------------------------------------|-------------------------------------------------------------|
| `osal_file_mkdir()` is called with `pathname == NULL` | returns `OSAL_FILE_STATUS_INVALID` |
| `osal_file_mkdir()` is called with `pathname == ""` | returns `OSAL_FILE_STATUS_INVALID` |
| `osal_file_mkdir()` is called with valid arguments. | returns `OSAL_FILE_STATUS_OK` |

---
