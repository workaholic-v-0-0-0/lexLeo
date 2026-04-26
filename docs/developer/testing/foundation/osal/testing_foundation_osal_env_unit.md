@page testing_foundation_osal_env_unit osal_env unit tests

It covers:
- `osal_getenv()`

---

@anchor testing_foundation_osal_getenv
# osal_getenv() unit tests

See @ref specifications_osal_getenv "osal_getenv() specifications"

## Functions under test

~~~c
const char *osal_getenv(const char *name);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN                                                                                                    | EXPECT                                                |
|---------------------------------------------------------------------------------------------------------|-------------------------------------------------------|
| `osal_getenv()` is called with the name of an environment variable known to exist for this test process | ret != NULL;<br>ret points to the expected value |
| `osal_getenv()` is called with the name of an environment variable known not to exist for this test process | ret == NULL |
