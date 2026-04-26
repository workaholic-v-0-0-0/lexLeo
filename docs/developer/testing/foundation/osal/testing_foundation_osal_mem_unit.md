@page testing_foundation_osal_mem_unit osal_mem unit tests

It covers:
- `osal_mem_default_ops()`
- `osal_malloc()`
- `osal_calloc()`
- `osal_free()`
- `osal_memcpy()`
- `osal_memset()`
- `osal_memmove()`

---

@anchor testing_foundation_osal_mem_unit_default_ops
# osal_mem_default_ops() unit tests

See @ref specifications_osal_mem_default_ops "osal_mem_default_ops() specifications"

## Functions under test

~~~c
const osal_mem_ops_t *osal_mem_default_ops(void);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_mem_default_ops()` is called | returns a non-`NULL` pointer to an `osal_mem_ops_t`;<br>`ret->malloc != NULL`;<br>`ret->free != NULL`;<br>`ret->calloc != NULL`;<br>`ret->realloc != NULL`;<br>`ret->malloc == osal_malloc`;<br>`ret->free == osal_free`;<br>`ret->calloc == osal_calloc`;<br>`ret->realloc == osal_realloc` |

## Notes

- This helper exposes the default low-level OSAL memory operations table.
- The returned table is expected to provide a well-formed set of callable
  low-level memory operations.
- Each function pointer in the returned table dispatches to the corresponding
  public default `osal_mem` primitive:
  - `malloc` dispatches to `osal_malloc()`
  - `free` dispatches to `osal_free()`
  - `calloc` dispatches to `osal_calloc()`
  - `realloc` dispatches to `osal_realloc()`

---

@anchor testing_foundation_osal_mem_unit_osal_malloc_osal_free
# osal_malloc() and osal_free() unit tests

See:
- @ref specifications_osal_malloc "osal_malloc() specifications"
- @ref specifications_osal_free "osal_free() specifications"

## Functions under test

~~~c
void *osal_malloc(size_t size);
void osal_free(void *ptr);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_malloc(4)` is called and the returned pointer is passed to `osal_free()` after writable access to the allocated bytes | returns a non-`NULL` pointer;<br>the allocated block is writable for the tested bytes;<br>`osal_free()` completes without reported failure |

## Notes

- These tests validate the public default allocation/deallocation path.
- The current suite exercises a successful allocation and release scenario.
- Allocation failure scenarios depend on the underlying C runtime allocator and are not deterministically forced in this unit test.

---

@anchor testing_foundation_osal_mem_unit_osal_calloc_osal_free
# osal_calloc() and osal_free() unit tests

See:
- @ref specifications_osal_calloc "osal_calloc() specifications"
- @ref specifications_osal_free "osal_free() specifications"

## Functions under test

~~~c
void *osal_calloc(size_t nmemb, size_t size);
void osal_free(void *ptr);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_calloc(3, sizeof(uint16_t))` is called and the returned pointer is passed to `osal_free()` after zero-initialization checks and writable access | returns a non-`NULL` pointer;<br>the allocated block is initialized to zero for the tested elements;<br>the allocated block is writable for the tested elements;<br>`osal_free()` completes without reported failure |

## Notes

- These tests validate the public default zero-initialized allocation/deallocation path.
- The current suite exercises a successful allocation, zero-initialization, writable access, and release scenario.
- Allocation failure scenarios depend on the underlying C runtime allocator and are not deterministically forced in this unit test.

---

@anchor testing_foundation_osal_mem_unit_osal_memcpy
# osal_memcpy() unit tests

See @ref specifications_osal_memcpy "osal_memcpy() specifications"

## Functions under test

~~~c
void *osal_memcpy(void *dest, const void *src, size_t n);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_memcpy()` is called with valid non-overlapping source and destination buffers | returns `dest`;<br>copies the requested bytes from `src` to `dest` |

## Notes

- These tests validate the public default raw memory copy path.
- The current suite exercises a successful copy between valid non-overlapping buffers.
- Overlapping memory areas are not covered here; callers should use `osal_memmove()` for overlapping ranges.

---

@anchor testing_foundation_osal_mem_unit_osal_memset
# osal_memset() unit tests

See @ref specifications_osal_memset "osal_memset() specifications"

## Functions under test

~~~c
void *osal_memset(void *s, int c, size_t n);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_memset()` is called with a valid writable memory area, a byte value, and `n == sizeof(uint32_t)` | returns `s`;<br>writes the requested byte value into each of the tested bytes |

## Notes

- These tests validate the public default raw memory fill path.
- The current suite exercises a successful byte-oriented fill on a valid writable memory area.
- The test verifies that `osal_memset()` writes bytes, regardless of the object type used to size the memory area.

---

@anchor testing_foundation_osal_mem_unit_osal_memmove
# osal_memmove() unit tests

See @ref specifications_osal_memmove "osal_memmove() specifications"

## Functions under test

~~~c
void *osal_memmove(void *dest, const void *src, size_t n);
~~~

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `osal_memmove()` is called with overlapping source and destination ranges inside the same buffer | returns `dest`;<br>moves the requested bytes from `src` to `dest`;<br>correctly handles the overlap |

## Notes

- These tests validate the public default raw memory move path.
- The current suite exercises a successful move with overlapping memory ranges.
- This scenario covers the key behavioral difference between `osal_memmove()`
  and `osal_memcpy()`.
