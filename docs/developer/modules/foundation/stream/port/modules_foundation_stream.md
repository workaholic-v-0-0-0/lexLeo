@page modules_foundation_stream stream

# Purpose

The `stream` port defines the common byte-stream abstraction.

It provides:

- a borrower-facing runtime API for reading, writing, and flushing streams;
- an owner-facing API for destroying streams and creating them through
  specialized creators;
- an adapter-facing API for implementing concrete `stream` adapters;
- Composition Root services for constructing streams, registering adapters,
  and wiring stream creation.

# Public API

- @ref stream_api "stream API"

Sub-APIs:

- @ref stream_borrowers_api "borrowers API"
- @ref stream_owners_api "owners API"
- @ref stream_adapters_api "adapters API"
- @ref stream_cr_api "Composition Root API"

# Architectural role

The `stream` module is a foundation port.

It defines the public byte-stream abstraction used by runtime code, while
adapter modules provide backend-specific implementations and the Composition
Root assembles both into complete creation workflows.

Typical responsibilities:

- expose the public `stream_t` abstraction and status model;
- dispatch borrower-facing operations through adapter-bound virtual tables;
- manage stream lifetime;
- define the adapter provider contract used to register concrete adapters;
- expose Composition Root facilities for stream construction, adapter
  registration, and specialized creator construction.

# Main concepts

## Borrower-facing usage

Runtime code manipulates `stream_t` handles through:

- `stream_read()`
- `stream_write()`
- `stream_flush()`

## Owner-facing usage

Owners manage stream lifetime and create streams through specialized creators:

- `stream_destroy()`
- `stream_buffer_creator_create()`
- `stream_regular_file_creator_create()`
- `stream_io_creator_create()`

## Adapter implementation

Concrete adapters implement the generic `stream` contract through:

- `stream_vtbl_t`
- `stream_adapter_provider_t`

Adapter providers bind backend construction to the virtual table implementing
the `stream` port.

## Composition Root wiring

The Composition Root constructs streams through:

- `stream_default_env()`
- `stream_create()`
- `stream_complete_default_init()`

It constructs and configures factories through:

- `stream_default_factory_cfg()`
- `stream_create_factory()`
- `stream_destroy_factory()`
- `stream_factory_add_adapter()`
- `stream_factory_create_stream()`
- `stream_destroy_adapter_provider()`

It also constructs the specialized creators exposed to owners:

- `stream_create_buffer_creator()`
- `stream_destroy_buffer_creator()`
- `stream_create_regular_file_creator()`
- `stream_destroy_regular_file_creator()`
- `stream_create_io_creator()`
- `stream_destroy_io_creator()`

# Related modules

Concrete adapters implementing this port are documented in the surrounding
`stream` family, including `fs_stream` and `stdio_stream`.

# See also

- @ref specifications_stream "stream specifications"
- @ref testing_foundation_stream_family_page "stream family tests"
- @ref stream_tests_group "stream tests"
