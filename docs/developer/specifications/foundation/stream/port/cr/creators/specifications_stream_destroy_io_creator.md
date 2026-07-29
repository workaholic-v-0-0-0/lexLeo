@page specifications_stream_destroy_io_creator stream_destroy_io_creator() specifications

# Signature

~~~c
void stream_destroy_io_creator(
    stream_io_creator_t **creator);
~~~

# Purpose

Destroy an I/O stream creator.

# Success

- If `creator == NULL`, the function does nothing.
- If `*creator == NULL`, the function does nothing.
- Otherwise:
    - stores `*creator` in a temporary variable;
    - sets `*creator` to `NULL`;
    - releases the creator object using the memory operations stored in the
      creator.

# Failure

- None

# Preconditions

When `creator != NULL` and `*creator != NULL`:

- The creator must have been created by `stream_create_io_creator()`.
- The creator must contain valid memory operations.
- The stored memory operations must provide a non-`NULL` `free` function.

# Ownership

- If a creator object is present, ownership is released by this function.
- After destruction, `*creator == NULL`.
