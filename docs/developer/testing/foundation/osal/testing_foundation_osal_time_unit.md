@page testing_foundation_osal_time_unit osal_time unit tests

It covers:
- `osal_time_default_ops()`
- `osal_time_ops_t::now()`

---

@anchor testing_foundation_osal_time_unit_default_ops
# osal_time_default_ops() unit tests

See @ref specifications_osal_time_default_ops "osal_time_default_ops() specifications"

## Functions under test

~~~c
const osal_time_ops_t *osal_time_default_ops(void);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_time_default_ops()` is called | returns a non-`NULL` pointer to an `osal_time_ops_t`;<br>`ret->now != NULL` |

---

@anchor testing_foundation_osal_time_unit_now
# osal_time_ops_t::now() unit tests

See @ref specifications_osal_time_ops_now "osal_time_ops_t::now() specifications"

## Functions under test

~~~c
osal_time_status_t (*now)(osal_time_t *out);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `now()` is called with `out == NULL` | returns `OSAL_TIME_STATUS_INVALID` |
| `now()` is called with a valid `osal_time_t` output pointer | returns `OSAL_TIME_STATUS_OK`;<br>stores a non-zero value in `out->epoch_seconds` |
