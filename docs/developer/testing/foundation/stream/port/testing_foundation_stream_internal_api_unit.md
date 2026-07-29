@page testing_foundation_stream_internal_api_unit stream_internal_api.c unit tests

---

@anchor testing_foundation_stream_unit_stream_factory_create_stream

# `stream_factory_create_stream()` unit tests

See:

- @ref specifications_stream_factory_create_stream "stream_factory_create_stream() specifications"

## Function under test

~~~c
stream_factory_status_t stream_factory_create_stream(
    const stream_factory_t *factory,
    stream_adapter_id_t adapter_id,
    const void *args,
    stream_t **out);
~~~

## Test doubles

- fake memory
- fake stream adapter

## Tested scenarios

| WHEN                                                        | EXPECT                                                                                                                                                                                                                                                                                                                                 |
| ----------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| no adapter provider is registered under `adapter_id`        | `stream_factory_create_stream()` returns `STREAM_FACTORY_STATUS_NOT_FOUND`; the adapter backend constructor is not called; `*out` remains unchanged                                                                                                                                                                                    |
| the adapter backend constructor returns `STREAM_STATUS_OOM` | the adapter backend constructor is called once with the expected creation arguments and a non-`NULL` backend output address; `stream_factory_create_stream()` returns `STREAM_FACTORY_STATUS_OOM`; `*out` remains unchanged                                                                                                            |
| allocation of the `stream_t` handle fails                   | `stream_factory_create_stream()` returns `STREAM_FACTORY_STATUS_OOM`; `*out` remains unchanged                                                                                                                                                                                                                                       |
| stream creation succeeds                                    | `stream_factory_create_stream()` returns `STREAM_FACTORY_STATUS_OK`; the adapter backend constructor is called once with the expected creation arguments and a non-`NULL` backend output address; `*out` receives a valid `stream_t` whose backend, memory operations, and virtual table are those expected from the selected adapter provider |
