@page specifications_stream_destroy_factory stream_destroy_factory() specifications

# Signature

~~~c
void stream_destroy_factory(stream_factory_t **fact);
~~~

# Purpose

Destroy a stream factory.

# Preconditions

If `fact != NULL` and `*fact != NULL`, `*fact` must point to a valid
`stream_factory_t`.

# Success

- If `fact == NULL`, the function does nothing.
- If `fact != NULL` and `*fact == NULL`, the function does nothing.
- Otherwise, the function releases the factory object referenced by `*fact`.
- After destruction, `*fact` is set to `NULL`.

# Failure

- None.

# Ownership

- If a factory object is present, ownership is released by this function.

# Notes

- `stream_destroy_factory()` is the matching destructor for handles created by
  `stream_create_factory()`.
