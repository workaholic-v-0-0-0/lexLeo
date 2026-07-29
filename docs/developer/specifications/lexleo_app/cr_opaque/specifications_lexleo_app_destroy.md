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
- If `app != NULL` and `*app != NULL`, `*app` must be either:
    - a valid handle created by `lexleo_app_create()`, or
    - a valid partially initialized handle left after a failed
      `lexleo_app_complete_default_init()` call, or
    - a valid fully initialized handle created by `lexleo_app_create_init()`.

# Success

- Releases every owned runtime resource attached to the application handle.
- Releases the application handle itself.
- Resets `*app` to `NULL` if `app != NULL`.

# Failure

- This function cannot fail.

# Ownership

- After the call, the caller no longer owns the application handle.
- Borrowed dependencies stored in the application environment are not destroyed.

# Notes

- This function is idempotent.
- It is safe to call this function after a failed
  `lexleo_app_complete_default_init()` call.
