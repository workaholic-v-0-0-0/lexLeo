@page specifications_stream_destroy_factory stream_destroy_factory() specifications

# Signature

~~~c
void stream_destroy_factory(stream_factory_t **fact);
~~~

# Purpose

Destroy a stream factory.

# Success

- If `fact == NULL`, the function does nothing.
- If `fact != NULL` and `*fact == NULL`, the function does nothing.
- Otherwise, the function releases the factory object referenced by `*fact`.
- After destruction, `*fact` is set to `NULL`.

# Failure

- None.

# Ownership

- If a factory object is present, ownership is released by this function.
- Descriptor-owned user data registered in the factory is released through its
  associated `ud_dtor`, when provided.

# Notes

- `stream_destroy_factory()` is the matching destructor for handles created by
  `stream_create_factory()`.
- Passing `NULL` or a pointer to `NULL` is accepted as a no-op.
