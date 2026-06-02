@page testing_lexleo_vm_cr_stream_creators_unit lexleo_vm CR stream creators unit tests

This page documents unit tests for the LexLeo VM Composition Root stream
creator wiring.

@anchor testing_lexleo_vm_cr_stream_creators_unit_complete_default_init_oom
# lexleo_vm_complete_default_init() OOM unit tests

See @ref specifications_lexleo_vm_complete_default_init "lexleo_vm_complete_default_init() specifications"

## Functions under test

~~~c
lexleo_vm_status_t lexleo_vm_complete_default_init(lexleo_vm_t *vm);
~~~

## Test doubles

- fake_memory

## Tested scenarios

| WHEN | EXPECT |
|---|---|
| an allocation fails during default stream-resource initialization | returns one of the documented stream-resource initialization OOM statuses;<br>the VM remains safely destroyable with `lexleo_vm_destroy()`;<br>no memory leak, invalid free, or double free is reported by `fake_memory` |
