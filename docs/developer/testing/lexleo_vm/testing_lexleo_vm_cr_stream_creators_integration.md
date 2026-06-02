@page testing_lexleo_vm_cr_stream_creators_integration lexleo_vm CR stream creators integration tests

This page documents integration tests for the LexLeo VM Composition Root stream
creator wiring.

These tests validate that `lexleo_vm_complete_default_init()` wires the default
stream factory and owner-facing stream creators into a LexLeo VM handle, and
that the resulting resources can be used through the public `stream` APIs.

---

@anchor testing_lexleo_vm_cr_stream_creators_integration_lifecycle
# Default stream creator lifecycle

See:
- @ref specifications_lexleo_vm_create "lexleo_vm_create() specifications"
- @ref specifications_lexleo_vm_complete_default_init "lexleo_vm_complete_default_init() specifications"
- @ref specifications_lexleo_vm_destroy "lexleo_vm_destroy() specifications"

## Functions under test

~~~c
lexleo_vm_status_t lexleo_vm_create(
    lexleo_vm_t **out,
    const lexleo_vm_cfg_t *cfg,
    const lexleo_vm_env_t *env);

lexleo_vm_status_t lexleo_vm_complete_default_init(lexleo_vm_t *vm);

void lexleo_vm_destroy(lexleo_vm_t **vm);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| a VM handle is created, completed with default stream creators, then destroyed | `lexleo_vm_create()` returns `LEXLEO_VM_STATUS_OK`;<br>`lexleo_vm_complete_default_init()` returns `LEXLEO_VM_STATUS_OK`;<br>the VM handle is non-`NULL` before destruction;<br>`lexleo_vm_destroy()` resets the VM handle to `NULL` |

---

@anchor testing_lexleo_vm_cr_stream_creators_integration_stdio
# Stdio stream creator smoke test

See:
- @ref specifications_lexleo_vm_create "lexleo_vm_create() specifications"
- @ref specifications_lexleo_vm_complete_default_init "lexleo_vm_complete_default_init() specifications"
- @ref specifications_stream_io_creator_create "stream_io_creator_create() specifications"
- @ref specifications_stream_write "stream_write() specifications"
- @ref specifications_stream_flush "stream_flush() specifications"
- @ref specifications_stream_destroy "stream_destroy() specifications"

## Functions under test

~~~c
lexleo_vm_status_t lexleo_vm_create(
    lexleo_vm_t **out,
    const lexleo_vm_cfg_t *cfg,
    const lexleo_vm_env_t *env);

lexleo_vm_status_t lexleo_vm_complete_default_init(lexleo_vm_t *vm);

stream_status_t stream_io_creator_create(
    stream_io_creator_t *creator,
    stream_io_kind_t kind,
    stream_t **out);

size_t stream_write(
    stream_t *stream,
    const void *buf,
    size_t len,
    stream_status_t *status);

stream_status_t stream_flush(stream_t *stream);

stream_status_t stream_destroy(stream_t **stream);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| a VM-created stdio stream creator creates an output stream, writes data, flushes it, and destroys it | stream creation returns `STREAM_STATUS_OK`;<br>the created stream is non-`NULL`;<br>`stream_write()` writes the full buffer length;<br>write status is `STREAM_STATUS_OK`;<br>`stream_flush()` returns `STREAM_STATUS_OK`;<br>`stream_destroy()` returns `STREAM_STATUS_OK` and resets the stream to `NULL` |

---

@anchor testing_lexleo_vm_cr_stream_creators_integration_file
# File stream creator smoke test

See:
- @ref specifications_lexleo_vm_create "lexleo_vm_create() specifications"
- @ref specifications_lexleo_vm_complete_default_init "lexleo_vm_complete_default_init() specifications"
- @ref specifications_stream_file_creator_create "stream_file_creator_create() specifications"
- @ref specifications_stream_write "stream_write() specifications"
- @ref specifications_stream_read "stream_read() specifications"
- @ref specifications_stream_destroy "stream_destroy() specifications"

## Functions under test

~~~c
lexleo_vm_status_t lexleo_vm_create(
    lexleo_vm_t **out,
    const lexleo_vm_cfg_t *cfg,
    const lexleo_vm_env_t *env);

lexleo_vm_status_t lexleo_vm_complete_default_init(lexleo_vm_t *vm);

stream_status_t stream_file_creator_create(
    stream_file_creator_t *creator,
    const char *path,
    const char *mode,
    stream_t **out);

size_t stream_write(
    stream_t *stream,
    const void *buf,
    size_t len,
    stream_status_t *status);

size_t stream_read(
    stream_t *stream,
    void *buf,
    size_t len,
    stream_status_t *status);

stream_status_t stream_destroy(stream_t **stream);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| a VM-created file stream creator creates a writable file stream, writes data, destroys it, then creates a readable file stream for the same path and reads the data back | both stream creations return `STREAM_STATUS_OK`;<br>each created stream is non-`NULL`;<br>`stream_write()` writes the full buffer length;<br>write status is `STREAM_STATUS_OK`;<br>`stream_read()` reads the full buffer length;<br>read status is `STREAM_STATUS_OK`;<br>the read buffer matches the written buffer;<br>both `stream_destroy()` calls return `STREAM_STATUS_OK` and reset the stream to `NULL` |

---

@anchor testing_lexleo_vm_cr_stream_creators_integration_buffer
# Buffer stream creator smoke test

See:
- @ref specifications_lexleo_vm_create "lexleo_vm_create() specifications"
- @ref specifications_lexleo_vm_complete_default_init "lexleo_vm_complete_default_init() specifications"
- @ref specifications_stream_buffer_creator_create "stream_buffer_creator_create() specifications"
- @ref specifications_stream_write "stream_write() specifications"
- @ref specifications_stream_read "stream_read() specifications"
- @ref specifications_stream_destroy "stream_destroy() specifications"

## Functions under test

~~~c
lexleo_vm_status_t lexleo_vm_create(
    lexleo_vm_t **out,
    const lexleo_vm_cfg_t *cfg,
    const lexleo_vm_env_t *env);

lexleo_vm_status_t lexleo_vm_complete_default_init(lexleo_vm_t *vm);

stream_status_t stream_buffer_creator_create(
    stream_buffer_creator_t *creator,
    stream_t **out);

size_t stream_write(
    stream_t *stream,
    const void *buf,
    size_t len,
    stream_status_t *status);

size_t stream_read(
    stream_t *stream,
    void *buf,
    size_t len,
    stream_status_t *status);

stream_status_t stream_destroy(stream_t **stream);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| a VM-created buffer stream creator creates a buffer stream, writes data, reads it back, and destroys the stream | stream creation returns `STREAM_STATUS_OK`;<br>the created stream is non-`NULL`;<br>`stream_write()` writes the full buffer length;<br>write status is `STREAM_STATUS_OK`;<br>`stream_read()` reads the full buffer length;<br>read status is `STREAM_STATUS_OK`;<br>the read buffer matches the written buffer;<br>`stream_destroy()` returns `STREAM_STATUS_OK` and resets the stream to `NULL` |
