@page specifications_lexleo_app_destroy lexleo_app_destroy() specifications

# Signature

```c
void lexleo_app_destroy(lexleo_app_t **app);
```

# Purpose

Destroy a LexLeo application handle and release every owned runtime resource
attached to it.

# Preconditions

- `app` may be `NULL`.
- `*app` may be `NULL` if `app != NULL`.
- If `app != NULL` and `*app != NULL`:
    - `*app` must designate a valid handle created by `lexleo_app_create()`
      or `lexleo_app_create_init()`, whether initialization is absent,
      partial, or complete.
    - The borrowed memory operations must remain valid and provide `free`.
    - `stream_owner_opaque` must designate the valid container allocated
      during handle creation.
    - Attached resources must satisfy their ownership and destruction
      contracts.
    - The only permitted aliases between stream fields are `vm_in == in`
      and `vm_out == out`.
    - Each distinct attached stream must be owned by the application.
    - Borrowed dependencies must remain valid until destruction completes.

# Success

- Does nothing if `app == NULL` or `*app == NULL`.
- Otherwise:
    - Destroys the VM before the streams and logger it borrows.
    - Destroys each distinct owned stream exactly once.
    - Does not separately destroy `vm_in` when it aliases `in`,
      or `vm_out` when it aliases `out`.
    - Destroys the logger before its stream.
    - Destroys the streams before the stream creators and factory.
    - Destroys the stream creators before the factory.
    - Releases the `stream_owner_opaque` container.
    - Releases the application handle itself.
    - Sets `*app` to `NULL`.

# Failure

- This function does not return an error status.

# Ownership

- Releases ownership of the application handle and all owned resources
  attached to it, including resources injected with ownership transferred
  to the application.
- Does not destroy borrowed runtime operations.
- Does not free the borrowed strings designated by `input_paths`,
  `input_dynamic_libraries`, or `output_path`.
- Borrowed aliases introduce no additional destruction.
- Other pointers held by the caller to the destroyed handle or its
  destroyed resources become invalid and are not reset by this function.

# Notes

- This function is idempotent when repeatedly called with the same
  pointer variable: after the first destruction, that variable is NULL.
- Idempotence does not permit destruction through another pointer
  retaining the address of an already destroyed handle.
- It is safe to call this function after a failed
  `lexleo_app_complete_default_init()` call.
