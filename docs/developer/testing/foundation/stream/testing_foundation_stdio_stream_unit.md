@page testing_foundation_stdio_stream_unit stdio_stream unit tests

It covers:
- `stdio_stream_default_cfg()`
- `stdio_stream_default_env()`

---

@anchor testing_foundation_stdio_stream_unit_default_cfg
# stdio_stream_default_cfg() unit tests

See @ref specifications_stdio_stream_default_cfg "stdio_stream_default_cfg() specifications"

## Functions under test

~~~c
stdio_stream_cfg_t stdio_stream_default_cfg(void);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `stdio_stream_default_cfg()` is called | returns a value-initialized `stdio_stream_cfg_t`;<br>`ret.reserved == 0` |

## Notes

- This helper establishes the default configuration invariant for the `stdio_stream` adapter.

---

@anchor testing_foundation_stdio_stream_unit_default_env
# stdio_stream_default_env() unit tests

See @ref specifications_stdio_stream_default_env "stdio_stream_default_env() specifications"

## Functions under test

~~~c
stdio_stream_env_t stdio_stream_default_env(
    const osal_stdio_ops_t *stdio_ops,
    const osal_mem_ops_t *mem,
    const stream_env_t *port_env);
~~~

## Success

- `env.stdio_ops == stdio_ops`.
- `env.mem == mem`.
- `env.port_env == *port_env`.

## Failure

- None.

## Test doubles

- dummy `osal_stdio_ops_t *`
- dummy `osal_mem_ops_t *`
- dummy `stream_env_t`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `stdio_stream_default_env(stdio_ops, mem, port_env)` is called with valid input | returns a `stdio_stream_env_t` such that `env.stdio_ops == stdio_ops`, `env.mem == mem`, and `env.port_env == *port_env` |

## Notes

- This helper preserves the borrowed OSAL stdio operations table provided by
  the caller.
- This helper preserves the borrowed adapter memory operations table provided
  by the caller.
- This helper preserves the injected `stream` port environment provided by the
  caller.
- The returned environment does not take ownership of any borrowed dependency.

---

@anchor testing_foundation_stdio_stream_unit_create_stream
# stdio_stream_create_stream() unit tests

See @ref specifications_stdio_stream_create_stream "stdio_stream_create_stream() specifications"

## Functions under test

~~~c
stream_status_t stdio_stream_create_stream(
    stream_t **out,
    const stdio_stream_args_t *args,
    const stdio_stream_cfg_t *cfg,
    const stdio_stream_env_t *env);
~~~

## Success

- Returns `STREAM_STATUS_OK`.
- Stores a non-NULL stream handle in `*out`.
- The produced stream forwards borrower-side operations to the standard stream
  designated by `args->kind`.

## Failure

- Returns `STREAM_STATUS_INVALID` when one or more input arguments are invalid.
- Returns `STREAM_STATUS_OOM` when allocation required during stream creation
  fails.
- Leaves `*out` unchanged on failure when `out != NULL`.

## Test doubles

- fake_stdio
- fake_memory

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `stdio_stream_create_stream(out, args, cfg, env)` is called with valid arguments and `args->kind == STDIO_STREAM_KIND_STDOUT` | returns `STREAM_STATUS_OK`;<br>stores a non-NULL stream handle in `*out`;<br>the produced stream forwards borrower-side write operations to the standard output stream |
| `out == NULL` | returns `STREAM_STATUS_INVALID`;<br>no stream handle is produced |
| `args == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `cfg == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `env == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `args != NULL` but `args->kind` does not designate a supported `stdio_stream_kind_t` value and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| allocation required by `stdio_stream_create_stream()` fails | returns `STREAM_STATUS_OOM`;<br>leaves `*out` unchanged |

## Notes

- The success scenario is exercised with `STDIO_STREAM_KIND_STDOUT` and
  validates that the created stream is operational through the public
  borrower-side `stream_write()` API.
- The OOM scenario is exercised by configuring `fake_memory` to fail the
  allocation performed during stream creation.
- Invalid-argument scenarios preserve the caller-visible output handle when
  `out != NULL`.

---

@anchor testing_foundation_stdio_stream_unit_create_desc
# stdio_stream_create_desc() unit tests

See @ref specifications_stdio_stream_create_desc "stdio_stream_create_desc() specifications"

## Functions under test

~~~c
stream_status_t stdio_stream_create_desc(
    stream_adapter_desc_t *out,
    stream_key_t key,
    const stdio_stream_cfg_t *cfg,
    const stdio_stream_env_t *env,
    const osal_mem_ops_t *mem);
~~~

## Success

- Returns `STREAM_STATUS_OK`.
- Stores a valid adapter descriptor in `*out`.
- The produced descriptor is eligible for later destruction via
  `out->ud_dtor()`.

## Failure

- Returns `STREAM_STATUS_INVALID` for invalid arguments.
- Returns `STREAM_STATUS_OOM` on allocation failure.
- If `out != NULL`, resets `*out` to an empty descriptor on failure.

## Test doubles

- fake_stdio
- fake_memory

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `stdio_stream_create_desc(out, key, cfg, env, mem)` is called with valid arguments | returns `STREAM_STATUS_OK`;<br>stores a valid adapter descriptor in `*out`;<br>the produced descriptor is eligible for later destruction via `out->ud_dtor()` |
| `out == NULL` | returns `STREAM_STATUS_INVALID`;<br>no descriptor is produced |
| `key == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>resets `*out` to an empty descriptor |
| `key` is an empty string and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>resets `*out` to an empty descriptor |
| `cfg == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>resets `*out` to an empty descriptor |
| `env == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>resets `*out` to an empty descriptor |
| `mem == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>resets `*out` to an empty descriptor |
| allocation required by `stdio_stream_create_desc()` fails | returns `STREAM_STATUS_OOM`;<br>resets `*out` to an empty descriptor |

## Notes

- The success scenario validates that the produced descriptor is structurally
  well formed and contains non-NULL `key`, `ctor`, `ud`, and `ud_dtor`
  fields.
- The OOM scenario is exercised by configuring `fake_memory` to fail the
  allocation performed during descriptor construction.
- Failure scenarios reset the caller-visible descriptor to an empty value when
  `out != NULL`.

---

@anchor testing_foundation_stdio_stream_unit_ctor
# stdio_stream_ctor() unit tests

See @ref specifications_stdio_stream_ctor "stdio_stream_ctor() specifications"

## Functions under test

~~~c
stream_status_t stdio_stream_ctor(
    const void *ud,
    const void *args,
    stream_t **out);
~~~

## Success

- Returns `STREAM_STATUS_OK`.
- Stores a non-NULL stream handle in `*out`.
- The produced stream forwards borrower-side operations to the standard stream
  designated by `((const stdio_stream_args_t *)args)->kind`.

## Failure

- Returns `STREAM_STATUS_INVALID` when one or more input arguments are invalid.
- Returns `STREAM_STATUS_OOM` when allocation required during delegated stream
  creation fails.
- Leaves `*out` unchanged on failure when `out != NULL`.

## Test doubles

- fake_stdio
- fake_memory

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `stdio_stream_ctor(ud, args, out)` is called with valid arguments and `args->kind == STDIO_STREAM_KIND_STDOUT` | returns `STREAM_STATUS_OK`;<br>stores a non-NULL stream handle in `*out`;<br>the produced stream forwards borrower-side write operations to the standard output stream |
| `out == NULL` | returns `STREAM_STATUS_INVALID`;<br>no stream handle is produced |
| `args == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `ud == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `args != NULL` but `args->kind` does not designate a supported `stdio_stream_kind_t` value and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| allocation required by `stdio_stream_ctor()` fails | returns `STREAM_STATUS_OOM`;<br>leaves `*out` unchanged |

## Notes

- The success scenario is exercised with `STDIO_STREAM_KIND_STDOUT` and
  validates that the constructed stream is operational through the public
  borrower-side `stream_write()` API.
- The OOM scenario is exercised by configuring `fake_memory` to fail the
  allocation performed during delegated stream creation.
- Invalid-argument scenarios preserve the caller-visible output handle when
  `out != NULL`.
- This constructor delegates stream creation to
  `stdio_stream_create_stream()` using the configuration and environment
  snapshot stored in `ud`.

---

@anchor testing_foundation_stdio_stream_unit_write
# stdio_stream_write() unit tests

See @ref specifications_stdio_stream_write "stdio_stream_write() specifications"

## Functions under test

~~~c
size_t stream_write(
    stream_t *s,
    const void *buf,
    size_t n,
    stream_status_t *st);
~~~

Internally exercised backend callback:

~~~c
static size_t stdio_stream_write(
    void *backend,
    const void *buf,
    size_t n,
    stream_status_t *st);
~~~

## Success

- Returns the number of bytes written.
- Appends written bytes to the fake buffered backing of the bound standard
  stream when that stream supports writing.
- Stores `STREAM_STATUS_OK` in `*st` when `st != NULL`.

## Failure

- Returns `0` and stores `STREAM_STATUS_INVALID` in `*st` for invalid
  arguments when `st != NULL`.
- Returns `0` and stores `STREAM_STATUS_IO_ERROR` in `*st` when the bound
  standard stream does not support writing.
- Leaves the fake buffered state unchanged on failure paths covered here.

## Test doubles

- fake_stdio
- fake_memory

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `stream_write(s, buf, n, &st)` is called with valid arguments and `args->kind == STDIO_STREAM_KIND_STDOUT` | returns `n`;<br>`st == STREAM_STATUS_OK`;<br>the `n` bytes from `buf` are appended to the buffered backing of the bound standard output stream |
| `stream_write(s, buf, n, &st)` is called with valid arguments and `args->kind == STDIO_STREAM_KIND_STDERR` | returns `n`;<br>`st == STREAM_STATUS_OK`;<br>the `n` bytes from `buf` are appended to the buffered backing of the bound standard error stream |
| `stream_write(s, buf, 0, &st)` is called and `args->kind` designates an output-oriented standard stream and `buf != NULL` | returns `0`;<br>`st == STREAM_STATUS_OK`;<br>the buffered backing of the bound standard stream is unchanged |
| `stream_write(s, NULL, 0, &st)` is called and `args->kind` designates an output-oriented standard stream | returns `0`;<br>`st == STREAM_STATUS_OK`;<br>the buffered backing of the bound standard stream is unchanged |
| `stream_write(s, NULL, n, &st)` is called with `n > 0` and `args->kind` designates an output-oriented standard stream | returns `0`;<br>`st == STREAM_STATUS_INVALID`;<br>the buffered backing of the bound standard stream is unchanged |
| `stream_write(s, buf, n, NULL)` is called with valid arguments and `args->kind == STDIO_STREAM_KIND_STDOUT` | returns `n`;<br>the `n` bytes from `buf` are appended to the buffered backing of the bound standard output stream;<br>no status is written |
| `stream_write(s, buf, n, NULL)` is called with valid arguments and `args->kind == STDIO_STREAM_KIND_STDERR` | returns `n`;<br>the `n` bytes from `buf` are appended to the buffered backing of the bound standard error stream;<br>no status is written |
| `stream_write(s, buf, n, &st)` is called with valid arguments and `args->kind == STDIO_STREAM_KIND_STDIN` | returns `0`;<br>`st == STREAM_STATUS_IO_ERROR`;<br>the buffered backing of the bound standard stream is unchanged |

## Notes

- This test suite intentionally exercises the private `stdio_stream_write()`
  callback through the public `stream_write()` wrapper.
- Success scenarios validate the buffered-backing mutation performed by
  `fake_stdio`.
- Failure scenarios that expect invariance preserve and compare the whole fake
  buffered backing state before and after the call under test.
- No implicit flush is expected during write operations in this suite.

---

@anchor testing_foundation_stdio_stream_unit_read
# stdio_stream_read() unit tests

See @ref specifications_stdio_stream_read "stdio_stream_read() specifications"

## Functions under test

~~~c
size_t stream_read(
    stream_t *s,
    void *buf,
    size_t n,
    stream_status_t *st);
~~~

Internally exercised backend callback:

~~~c
static size_t stdio_stream_read(
    void *backend,
    void *buf,
    size_t n,
    stream_status_t *st);
~~~

## Success

- Returns the number of bytes read.
- Copies bytes from the fake buffered backing of the bound standard stream
  into `buf` when that stream supports reading.
- Advances the fake read position by the number of bytes actually read.
- Stores `STREAM_STATUS_OK` in `*st` when `st != NULL`.

## Failure

- Returns `0` and stores `STREAM_STATUS_INVALID` in `*st` for invalid
  arguments when `st != NULL`.
- Returns `0` and stores `STREAM_STATUS_IO_ERROR` in `*st` when the bound
  standard stream does not support reading.
- Leaves the fake buffered backing unchanged on all paths covered here.

## Test doubles

- fake_stdio
- fake_memory

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `stream_read(s, buf, 0, &st)` is called and `args->kind == STDIO_STREAM_KIND_STDIN` and `buf != NULL` | returns `0`;<br>`st == STREAM_STATUS_OK`;<br>the fake read position of the bound standard input stream is unchanged;<br>`buf` is unchanged;<br>the buffered backing of the bound standard input stream is unchanged |
| `stream_read(s, NULL, 0, &st)` is called and `args->kind == STDIO_STREAM_KIND_STDIN` | returns `0`;<br>`st == STREAM_STATUS_OK`;<br>the fake read position of the bound standard input stream is unchanged;<br>the buffered backing of the bound standard input stream is unchanged |
| `stream_read(s, NULL, n, &st)` is called with `n > 0` and `args->kind == STDIO_STREAM_KIND_STDIN` | returns `0`;<br>`st == STREAM_STATUS_INVALID`;<br>the fake read position of the bound standard input stream is unchanged;<br>the buffered backing of the bound standard input stream is unchanged |
| `stream_read(s, buf, n, &st)` is called with valid arguments and `args->kind == STDIO_STREAM_KIND_STDOUT` | returns `0`;<br>`st == STREAM_STATUS_IO_ERROR`;<br>the fake read position of the bound standard stream is unchanged;<br>`buf` is unchanged;<br>the buffered backing of the bound standard stream is unchanged |
| `stream_read(s, buf, n, &st)` is called with valid arguments and `args->kind == STDIO_STREAM_KIND_STDERR` | returns `0`;<br>`st == STREAM_STATUS_IO_ERROR`;<br>the fake read position of the bound standard stream is unchanged;<br>`buf` is unchanged;<br>the buffered backing of the bound standard stream is unchanged |
| `stream_read(s, buf, n, NULL)` is called with valid arguments and `args->kind == STDIO_STREAM_KIND_STDIN` | returns the number of bytes actually read, up to `n`;<br>the bytes read from the buffered backing of the bound standard input stream are copied into `buf`;<br>the fake read position of the bound standard input stream is advanced by the number of bytes actually read;<br>the buffered backing of the bound standard input stream is unchanged;<br>no status is written |
| `stream_read(s, buf, n, &st)` is called with valid arguments and `args->kind == STDIO_STREAM_KIND_STDIN` | returns the number of bytes actually read, up to `n`;<br>`st == STREAM_STATUS_OK`;<br>the bytes read from the buffered backing of the bound standard input stream are copied into `buf`;<br>the fake read position of the bound standard input stream is advanced by the number of bytes actually read;<br>the buffered backing of the bound standard input stream is unchanged |
| `stream_read(s, buf, n, &st)` is called with valid arguments and `args->kind == STDIO_STREAM_KIND_STDIN` and `n` is greater than the number of bytes currently available in the buffered backing | returns the number of bytes actually available in the buffered backing;<br>`st == STREAM_STATUS_OK`;<br>only the available bytes are copied into `buf`;<br>the fake read position of the bound standard input stream is advanced by the number of bytes actually read;<br>the buffered backing of the bound standard input stream is unchanged |

## Notes

- This test suite intentionally exercises the private `stdio_stream_read()`
  callback through the public `stream_read()` wrapper.
- `fake_stdio` models readable standard-stream data through an explicit
  internal `buffered_backing` and a tracked read cursor.
- Read operations are validated both through copied output bytes and through
  the resulting fake read position.
- The buffered backing is asserted unchanged on all paths covered by this
  suite.

---

@anchor testing_foundation_stdio_stream_unit_flush
# stdio_stream_flush() unit tests

See @ref specifications_stdio_stream_flush "stdio_stream_flush() specifications"

## Functions under test

~~~c
stream_status_t stream_flush(stream_t *s);
~~~

Internally exercised backend callback:

~~~c
static stream_status_t stdio_stream_flush(void *backend);
~~~

## Success

- Returns `STREAM_STATUS_OK`.
- Transfers buffered data from the fake buffered backing of the bound standard
  stream to its fake sink backing when that stream supports flushing.

## Failure

- Returns `STREAM_STATUS_IO_ERROR` when the bound standard stream does not
  support flushing.
- Leaves the fake buffered state unchanged on failure paths covered here.

## Test doubles

- fake_stdio
- fake_memory

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `stream_flush(s)` is called with valid arguments and `args->kind == STDIO_STREAM_KIND_STDIN` | returns `STREAM_STATUS_IO_ERROR`;<br>the buffered backing of the bound standard stream is unchanged;<br>the sink backing of the bound standard stream is unchanged |
| `stream_flush(s)` is called with valid arguments and `args->kind == STDIO_STREAM_KIND_STDOUT` | returns `STREAM_STATUS_OK`;<br>buffered data from the bound standard output stream is transferred to its sink backing according to the fake flush behavior |
| `stream_flush(s)` is called with valid arguments and `args->kind == STDIO_STREAM_KIND_STDERR` | returns `STREAM_STATUS_OK`;<br>buffered data from the bound standard error stream is transferred to its sink backing according to the fake flush behavior |

## Notes

- This test suite intentionally exercises the private `stdio_stream_flush()`
  callback through the public `stream_flush()` wrapper.
- `fake_stdio` models standard streams with an explicit `buffered_backing`
  and a flushed-output `sink_backing`.
- Flush operations are validated through both the final buffered state and the
  final sink state.
- According to the fake flush behavior, transferred bytes are removed from the
  buffered backing and appended to the sink backing.
- If the bound standard stream does not support flushing, the fake buffered
  state is expected to remain unchanged on the paths covered here.

---

@anchor testing_foundation_stdio_stream_unit_close
# stdio_stream_close() unit tests

See @ref specifications_stdio_stream_close "stdio_stream_close() specifications"

## Functions under test

~~~c
void stream_destroy(stream_t **s);
~~~

Internally exercised backend callback:

~~~c
static stream_status_t stdio_stream_close(void *backend);
~~~

## Success

- Releases the `stdio_stream` backend container through its injected memory
  operations.
- Destroys the public stream handle.
- Sets the caller-visible handle to `NULL`.

## Failure

- None on the paths covered by this test suite.

## Test doubles

- fake_memory

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `stream_destroy(&s)` is called with a valid stream handle and `args->kind == STDIO_STREAM_KIND_STDIN` | the backend container bound to the stream is released;<br>the stream handle is destroyed;<br>the caller-visible handle is set to `NULL` |
| `stream_destroy(&s)` is called with a valid stream handle and `args->kind == STDIO_STREAM_KIND_STDOUT` | the backend container bound to the stream is released;<br>the stream handle is destroyed;<br>the caller-visible handle is set to `NULL` |
| `stream_destroy(&s)` is called with a valid stream handle and `args->kind == STDIO_STREAM_KIND_STDERR` | the backend container bound to the stream is released;<br>the stream handle is destroyed;<br>the caller-visible handle is set to `NULL` |

## Notes

- This test suite intentionally exercises the private `stdio_stream_close()`
  callback through the public `stream_destroy()` wrapper.
- The wrapped standard stream is borrowed by the backend and is not owned by
  the `stdio_stream` backend container.
- Therefore, these tests validate backend-container release and public handle
  destruction, not closure of the underlying standard stream itself.
- Memory invariants are verified at teardown through `fake_memory`.

