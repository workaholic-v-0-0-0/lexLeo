@page specifications_stream_destroy_standard_stream_creator stream_destroy_standard_stream_creator() specifications

# Signature

~~~c
void stream_destroy_standard_stream_creator(
    stream_standard_stream_creator_t **creator);
~~~

# Purpose

Destroy a standard stream creator.

# Preconditions

If `creator != NULL` and `*creator != NULL`, `*creator` must point to a valid
`stream_standard_stream_creator_t`.

# Success

- If `creator == NULL`, the function does nothing.
- If `creator != NULL` and `*creator == NULL`, the function does nothing.
- Otherwise, the function releases the creator object referenced by `*creator`.
- After destruction, `*creator` is set to `NULL`.

# Failure

- None.

# Ownership

- If a creator object is present, ownership is released by this function.

# Notes

- `stream_destroy_standard_stream_creator()` is the matching destructor for
  handles created by `stream_create_standard_stream_creator()`.
