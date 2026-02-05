@page testing_foundation_stream_factory_integration stream factory integration tests

It covers:
- `stream_create_factory()` / `stream_destroy_factory()`
- `stream_factory_add_adapter()`

Minimal local test adapters are used only as test doubles to observe registration
and resolution behavior through the public factory API.

@anchor testing_foundation_stream_integration_stream_create_factory_stream_destroy_factory
# stream_create_factory() / stream_destroy_factory() integration tests

## Functions under test

```c
stream_status_t stream_create_factory(
    stream_factory_t **out,
    const stream_factory_cfg_t *cfg,
    const stream_env_t *env);

void stream_destroy_factory(stream_factory_t **fact);
```

## Invalid arguments

- `out`, `cfg`, and `env` must not be `NULL`.
- `env->mem` must not be `NULL`.
- `env->mem->calloc` and `env->mem->free` must not be `NULL`.

## Success

- Returns `STREAM_STATUS_OK`.
- Stores a valid factory handle in `*out`.
- The produced factory must be destroyed via `stream_destroy_factory()`.

## Failure

- Returns:
    - `STREAM_STATUS_INVALID` for invalid arguments
- Leaves `*out` unchanged if `out` is not `NULL`.

## Lifecycle

- `stream_destroy_factory()` does nothing if `fact == NULL` or `*fact == NULL`.
- Otherwise, it releases the factory object and sets `*fact` to `NULL`.

## Test doubles

- none

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `stream_create_factory(out, cfg, env)` is called with valid arguments | returns `STREAM_STATUS_OK`;<br>stores a non-`NULL` factory handle in `*out`;<br>the produced handle is eligible for destruction by `stream_destroy_factory()` |
| `out == NULL` | returns `STREAM_STATUS_INVALID`;<br>no factory handle is produced |
| `cfg == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `env == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `env != NULL` but `env->mem == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `env->mem != NULL` but `env->mem->calloc == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `env->mem != NULL` but `env->mem->free == NULL` and `out != NULL` | returns `STREAM_STATUS_INVALID`;<br>leaves `*out` unchanged |
| `stream_create_factory()` succeeds and `stream_destroy_factory()` is called twice | first `stream_destroy_factory(&fact)` releases the handle and sets `fact` to `NULL`;<br>second `stream_destroy_factory(&fact)` is a no-op and keeps `fact` as `NULL` |

## Notes

- This contract validates the basic lifecycle guarantees of the stream factory.
- Output-handle preservation on failure is part of the tested behavioral contract.

---

@anchor testing_foundation_stream_integration_stream_factory_add_adapter
# stream_factory_add_adapter() integration tests

## Function under test

```c
stream_status_t stream_factory_add_adapter(
    stream_factory_t *fact,
    const stream_adapter_desc_t *desc);
```

## Precondition

- `fact` designates a valid factory instance previously created by
  `stream_create_factory()`.

## Invalid arguments

- `fact` and `desc` must not be `NULL`.
- `desc->key` and `desc->ctor` must not be `NULL`.
- `desc->key` must not be empty.

## Success

- Returns `STREAM_STATUS_OK`.
- A later call to `stream_factory_create_stream()` with the registered key
  can resolve the descriptor.

## Failure

- Returns:
    - `STREAM_STATUS_INVALID` for invalid arguments
    - `STREAM_STATUS_ALREADY_EXISTS` if the key is already registered
    - `STREAM_STATUS_FULL` if the factory capacity is exhausted

## Test doubles

- minimal local adapter constructors:
    - `test_stream_ctor_1()`
    - `test_stream_ctor_2()`

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| `stream_factory_add_adapter(fact, desc)` is called with a valid factory and a valid descriptor whose key is not yet registered, and the factory has not reached its registration capacity | returns `STREAM_STATUS_OK`;<br>a later call to `stream_factory_create_stream()` with that key returns `STREAM_STATUS_OK`;<br>the produced stream corresponds to the registered descriptor |
| `fact == NULL` | returns `STREAM_STATUS_INVALID` |
| `desc == NULL` | returns `STREAM_STATUS_INVALID` |
| `desc != NULL` but `desc->key == NULL` | returns `STREAM_STATUS_INVALID` |
| `desc != NULL` but `desc->key` is an empty string | returns `STREAM_STATUS_INVALID` |
| `desc != NULL` but `desc->ctor == NULL` | returns `STREAM_STATUS_INVALID` |
| a descriptor is added with a key that is already registered | returns `STREAM_STATUS_ALREADY_EXISTS`;<br>a later call to `stream_factory_create_stream()` with that key returns `STREAM_STATUS_OK`;<br>the produced stream corresponds to the previously registered descriptor |
| the factory has reached its registration capacity and a new descriptor is added | returns `STREAM_STATUS_FULL`;<br>a later call to `stream_factory_create_stream()` with the new key returns `STREAM_STATUS_NOT_FOUND`;<br>no stream handle is produced |

## Notes

- Registration effects are observed indirectly through
  `stream_factory_create_stream()`.
- Minimal local adapter constructors are sufficient here because the tested
  contract concerns factory registration and key-based resolution, not real
  stream runtime behavior.
- Distinct sentinel stream handles are used to distinguish which registered
  descriptor is actually resolved.
