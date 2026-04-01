@page testing_foundation_dynamic_buffer_stream_unit dynamic_buffer_stream unit tests

It covers:
- `dynamic_buffer_stream_default_cfg()`
- `dynamic_buffer_stream_default_env()`
- `dynamic_buffer_stream_create_stream()`
- `dynamic_buffer_stream_create_desc()`
- dynamic-buffer-backed runtime behavior through:
    - `stream_write()`
    - `stream_read()`
    - `stream_flush()`
    - `stream_destroy()` (via backend close)

---

@anchor testing_foundation_dynamic_buffer_stream_unit_default_cfg
# dynamic_buffer_stream_default_cfg() unit tests

See @ref specifications_dynamic_buffer_stream_default_cfg "dynamic_buffer_stream_default_cfg() specifications"

## Functions under test

~~~c
dynamic_buffer_stream_cfg_t dynamic_buffer_stream_default_cfg(void);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `dynamic_buffer_stream_default_cfg()` is called | returns a `dynamic_buffer_stream_cfg_t` such that `ret.default_cap > 0` |

## Notes

- This helper establishes the default configuration invariant for the `dynamic_buffer_stream` adapter.
- The exact value of `default_cap` is not part of the public contract; only its validity (non-zero) is guaranteed.

---

@anchor testing_foundation_dynamic_buffer_stream_unit_default_env
# dynamic_buffer_stream_default_env() unit tests

See @ref specifications_dynamic_buffer_stream_default_env "dynamic_buffer_stream_default_env() specifications"

## Functions under test

~~~c
dynamic_buffer_stream_env_t dynamic_buffer_stream_default_env(
    const osal_mem_ops_t *mem,
    const stream_env_t *port_env);
~~~

## Success

- `ret.mem == mem`
- `ret.port_env == *port_env`

## Failure

- None

## Test doubles

- dummy `osal_mem_ops_t *`
- dummy `stream_env_t`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `dynamic_buffer_stream_default_env(mem, port_env)` is called with valid inputs | returns a `dynamic_buffer_stream_env_t` such that `ret.mem == mem` and `ret.port_env == *port_env` |

## Notes

- This helper preserves the injected memory dependency and the forwarded `stream` port environment provided by the caller.

---

@anchor testing_foundation_dynamic_buffer_stream_unit_create_stream
# dynamic_buffer_stream_create_stream() unit tests

See @ref specifications_dynamic_buffer_stream_create_stream "dynamic_buffer_stream_create_stream() specifications"

## Functions under test

~~~c
stream_status_t dynamic_buffer_stream_create_stream(
    stream_t **out,
    const dynamic_buffer_stream_cfg_t *cfg,
    const dynamic_buffer_stream_env_t *env);
~~~

## Test doubles

- fake memory allocator (`fake_memory`)

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| valid inputs | returns `STREAM_STATUS_OK`, stores a non-NULL stream in `*out` |
| `out == NULL` | returns `STREAM_STATUS_INVALID` |
| `cfg == NULL` | returns `STREAM_STATUS_INVALID`, `*out` unchanged |
| `env == NULL` | returns `STREAM_STATUS_INVALID`, `*out` unchanged |
| allocation fails | returns `STREAM_STATUS_OOM`, `*out` unchanged |

## Notes

- Output pointer preservation is explicitly verified using sentinel values.
- Successful creation is followed by basic borrower API checks (`write`, `flush`).

---

@anchor testing_foundation_dynamic_buffer_stream_unit_create_desc
# dynamic_buffer_stream_create_desc() unit tests

See @ref specifications_dynamic_buffer_stream_create_desc "dynamic_buffer_stream_create_desc() specifications"

## Functions under test

~~~c
stream_status_t dynamic_buffer_stream_create_desc(
    stream_adapter_desc_t *out,
    stream_key_t key,
    const dynamic_buffer_stream_cfg_t *cfg,
    const dynamic_buffer_stream_env_t *env,
    const osal_mem_ops_t *mem);
~~~

## Test doubles

- fake memory allocator (`fake_memory`)

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| valid inputs | returns `STREAM_STATUS_OK`, produces a valid descriptor |
| `out == NULL` | returns `STREAM_STATUS_INVALID` |
| `key == NULL` | returns `STREAM_STATUS_INVALID`, resets descriptor |
| `key == ""` | returns `STREAM_STATUS_INVALID`, resets descriptor |
| `cfg == NULL` | returns `STREAM_STATUS_INVALID`, resets descriptor |
| `env == NULL` | returns `STREAM_STATUS_INVALID`, resets descriptor |
| `mem == NULL` | returns `STREAM_STATUS_INVALID`, resets descriptor |
| allocation fails | returns `STREAM_STATUS_OOM`, resets descriptor |

## Notes

- Descriptor validity includes:
    - non-empty key
    - non-NULL constructor
    - non-NULL user data and destructor
- Descriptor reset semantics are enforced on failure.

---

@anchor testing_foundation_dynamic_buffer_stream_unit_write
# dynamic_buffer_stream_write() unit tests

See:
- @ref specifications_dynamic_buffer_stream_write "dynamic_buffer_stream_write() specifications"
- @ref specifications_stream_write "stream_write() specifications"

## Functions under test

~~~c
size_t stream_write(stream_t *s, const void *buf, size_t n, stream_status_t *st);
~~~

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| valid write without growth | writes `n`, appends data, `read_pos` unchanged |
| valid write with growth | writes `n`, buffer grows |
| `n == 0` | returns `0`, no change |
| `buf == NULL && n == 0` | returns `0`, no change |
| `buf == NULL && n > 0` | returns `0`, `STREAM_STATUS_INVALID` |
| reserve fails | returns `0`, `STREAM_STATUS_OOM`, no change |
| size overflow | returns `0`, `STREAM_STATUS_INVALID`, no change |

## Notes

- Backend invariants are validated before and after the call.
- Failure paths ensure strict non-mutation of backend state.

---

@anchor testing_foundation_dynamic_buffer_stream_unit_read
# dynamic_buffer_stream_read() unit tests

See:
- @ref specifications_dynamic_buffer_stream_read "dynamic_buffer_stream_read() specifications"
- @ref specifications_stream_read "stream_read() specifications"

## Functions under test

~~~c
size_t stream_read(stream_t *s, void *buf, size_t n, stream_status_t *st);
~~~

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| read within available data | returns `n`, advances `read_pos` |
| read beyond available data | returns remaining bytes |
| `n == 0` | returns `0`, no change |
| `buf == NULL && n == 0` | returns `0`, no change |
| `buf == NULL && n > 0` | returns `0`, `STREAM_STATUS_INVALID` |
| EOF | returns `0`, `STREAM_STATUS_EOF`, no change |

## Notes

- Read operations never modify `len` or buffer content.
- Only `read_pos` is advanced on success.

---

@anchor testing_foundation_dynamic_buffer_stream_unit_flush
# dynamic_buffer_stream_flush() unit tests

See @ref specifications_dynamic_buffer_stream_flush "dynamic_buffer_stream_flush() specifications"

## Functions under test

~~~c
stream_status_t stream_flush(stream_t *s);
~~~

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| flush is called | returns `STREAM_STATUS_OK`, backend unchanged |

## Notes

- This adapter implements flush as a no-op.

---

@anchor testing_foundation_dynamic_buffer_stream_unit_close
# dynamic_buffer_stream_close() unit tests

See:
- @ref specifications_dynamic_buffer_stream_close "dynamic_buffer_stream_close() specifications"
- @ref specifications_stream_destroy "stream_destroy() specifications"

## Functions under test

~~~c
void stream_destroy(stream_t **s);
~~~

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| destroy is called on a valid stream | releases backend resources, sets `*s` to NULL |

## Notes

- The private `close` callback is exercised indirectly via `stream_destroy()`.
- Memory correctness is validated using `fake_memory` invariants.