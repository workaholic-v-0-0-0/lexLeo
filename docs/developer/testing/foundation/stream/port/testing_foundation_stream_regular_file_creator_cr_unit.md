@page testing_foundation_stream_regular_file_creator_cr_unit stream_regular_file_creator_cr.c unit tests

---

@anchor testing_foundation_stream_regular_file_creator_t_lifecycle

# `stream_regular_file_creator_t` lifecycle unit tests

See:

- @ref specifications_stream_create_regular_file_creator "stream_create_regular_file_creator() specifications"
- @ref specifications_stream_destroy_regular_file_creator "stream_destroy_regular_file_creator() specifications"

## Functions under test

```c
stream_factory_status_t stream_create_regular_file_creator(
    stream_regular_file_creator_t **out,
    stream_factory_t *factory,
    stream_adapter_id_t adapter_id,
    const osal_mem_ops_t *mem);

void stream_destroy_regular_file_creator(
    stream_regular_file_creator_t **creator);
```

## Test doubles

- fake adapter
- fake memory operations

## Tested scenarios

| WHEN | EXPECT |
| ---- | ------ |
| allocation of the `stream_regular_file_creator_t` handle fails | `stream_create_regular_file_creator()` returns `STREAM_FACTORY_STATUS_OOM`, leaves the creator handle unchanged, and causes no memory leak, invalid free, or double free |
| creation succeeds | `stream_create_regular_file_creator()` returns `STREAM_FACTORY_STATUS_OK`, stores a non-`NULL` creator handle in `*out` different from its initial value; `stream_destroy_regular_file_creator()` sets `*out` to `NULL`; the complete lifecycle causes no memory leak, invalid free, or double free |
| creation succeeds and `stream_destroy_regular_file_creator()` is called twice | the first destruction sets `*out` to `NULL`; the second destruction leaves `*out` equal to `NULL`; the complete lifecycle causes no memory leak, invalid free, or double free |
