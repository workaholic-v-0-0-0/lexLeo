@page testing_foundation_osal_stdio_unit osal_stdio unit tests

It covers:
- `osal_stdio_default_ops()`
- `osal_stdio_ops_t::get_stdin()`
- `osal_stdio_ops_t::get_stdout()`
- `osal_stdio_ops_t::get_stderr()`

---

@anchor testing_foundation_osal_stdio_unit_default_ops
# osal_stdio_default_ops() unit tests

See @ref specifications_osal_stdio_default_ops "osal_stdio_default_ops() specifications"

## Functions under test

~~~c
const osal_stdio_ops_t *osal_stdio_default_ops(void);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_stdio_default_ops()` is called | returns a non-`NULL` pointer to an `osal_stdio_ops_t`;<br>`ret->get_stdin != NULL`;<br>`ret->get_stdout != NULL`;<br>`ret->get_stderr != NULL`;<br>`ret->read != NULL`;<br>`ret->write != NULL`;<br>`ret->flush != NULL`;<br>`ret->error != NULL`;<br>`ret->eof != NULL` |

---

@anchor testing_foundation_osal_stdio_unit_get_stdin
# osal_stdio_ops_t::get_stdin() unit tests

See @ref specifications_osal_stdio_get_stdin "osal_stdio_ops_t::get_stdin() specifications"

## Functions under test

~~~c
OSAL_STDIO *(*get_stdin)(void);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_stdio_ops_t::get_stdin()` is called twice through the default operations table | each call returns a non-`NULL` `OSAL_STDIO *`;<br>both calls return the same handle instance |

---

@anchor testing_foundation_osal_stdio_unit_get_stdout
# osal_stdio_ops_t::get_stdout() unit tests

See @ref specifications_osal_stdio_get_stdout "osal_stdio_ops_t::get_stdout() specifications"

## Functions under test

~~~c
OSAL_STDIO *(*get_stdout)(void);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_stdio_ops_t::get_stdout()` is called twice through the default operations table | each call returns a non-`NULL` `OSAL_STDIO *`;<br>both calls return the same handle instance |

---

@anchor testing_foundation_osal_stdio_unit_get_stderr
# osal_stdio_ops_t::get_stderr() unit tests

See @ref specifications_osal_stdio_get_stderr "osal_stdio_ops_t::get_stderr() specifications"

## Functions under test

~~~c
OSAL_STDIO *(*get_stderr)(void);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_stdio_ops_t::get_stderr()` is called twice through the default operations table | each call returns a non-`NULL` `OSAL_STDIO *`;<br>both calls return the same handle instance |
