@page specifications_stdio_stream_ctor stdio_stream_ctor() specifications

# Signature

```c
stream_status_t stdio_stream_ctor(
    const void *ud,
    const void *args,
    stream_t **out);
```

# Purpose

Create a `stdio_stream` instance through the factory-compatible constructor
contract.

# Preconditions

* If `ud != NULL`, `ud` must point to a valid
  `stdio_stream_ctor_ud_t`.
* If `args != NULL`, `args` must point to a valid
  `stream_io_creator_args_t`.

# Invalid arguments

* `ud` must not be `NULL`.
* `args` must not be `NULL`.
* `out` must not be `NULL`.
* `((const stream_io_creator_args_t *)args)->kind` must be one of:

    * `STREAM_IO_INPUT`
    * `STREAM_IO_OUTPUT`
    * `STREAM_IO_ERR`

# Success

* Returns `STREAM_STATUS_OK`.
* Stores a valid newly created stream handle in `*out`.
* The created stream wraps one of:

    * `stdin` when `((const stream_io_creator_args_t *)args)->kind ==
    STREAM_IO_INPUT`
    * `stdout` when `((const stream_io_creator_args_t *)args)->kind ==
    STREAM_IO_OUTPUT`
    * `stderr` when `((const stream_io_creator_args_t *)args)->kind ==
    STREAM_IO_ERR`

# Failure

* Returns `STREAM_STATUS_INVALID` for invalid arguments.
* Returns `STREAM_STATUS_OOM` if memory allocation fails.
* Propagates any non-success status returned by
  `stdio_stream_create_stream()`.
* Leaves `*out` unchanged if `out` is not `NULL`.

# Ownership

* Ownership of `ud` and `args` is not transferred.
* On success, ownership of the newly created stream handle is transferred to
  the caller.
* The produced stream handle must later be destroyed via `stream_destroy()`.
* On failure, no stream ownership is transferred.

# Notes

* This function is intended to be stored in a `stream_adapter_desc_t` and
  invoked through the `stream` factory contract.
* The adapter wraps existing standard C streams (`stdin`, `stdout`, `stderr`)
  and does not take ownership of them.
* The constructor user data designated by `ud` is borrowed for the duration of
  the call.
* The creation arguments designated by `args` are borrowed for the duration of
  the call.
