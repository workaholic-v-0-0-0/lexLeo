@page modules_lexleo_app lexleo_app

# Purpose

The `lexleo_app` module provides the public API used to configure, construct,
run, and destroy LexLeo application handles.

It is responsible for:

- providing default application configuration values;
- creating application handles with the default runtime environment;
- composing application components and their runtime dependencies;
- exposing runtime execution through `lexleo_app_run()`;
- destroying application handles and their owned runtime resources.

The currently supported action is `LEXLEO_APP_ACTION_CONTROL_VM`.

# Public API

- @ref lexleo_app_api "lexleo_app API"

Sub-APIs:

- @ref lexleo_app_borrowers_api "borrowers API"
- @ref lexleo_app_cr_opaque_api "CR API"

# Architectural role

The `lexleo_app` module is the Composition Root of the LexLeo application.

It bridges:

- application configuration supplied by the client;
- default runtime services provided by the OSAL family;
- application components constructed during initialization;
- borrower-facing runtime execution through `lexleo_app_run()`.

The public handle type `lexleo_app_t` is opaque. Its private representation
stores configuration values, borrowed pointers, and owned application resources.

Environment construction and separate creation and initialization services
remain internal. Test support provides access to these services for dependency
injection and unit testing.

# Main concepts

## Application configuration

Client code obtains the default configuration through:

- `lexleo_app_default_cfg()`

The configuration specifies the requested action, input paths,
dynamic-library paths, an output path, and application stream settings.

The default configuration corresponds to invoking `lexleo` without arguments:
it selects `LEXLEO_APP_ACTION_CONTROL_VM`, no paths, and standard input,
standard output, and standard error.

Application streams can be configured to use standard streams, files,
or dynamic buffers.

Path pointers are copied into the handle without duplicating their strings.
The strings remain borrowed; the configuration structure itself is not retained.

Input paths, dynamic-library paths, and the output path are stored but are not
used by the current VM control action.

## Application construction and initialization

Client code creates a fully initialized application through:

- `lexleo_app_create_init()`

This function uses the supplied configuration and the default runtime
environment. It combines handle creation and component initialization without
exposing internal Composition Root details to the client.

If creation or initialization fails, the function releases all resources
acquired for the application and leaves the caller's output handle unchanged.

Construction does not execute the requested action or start the VM.

## Internal composition services

Application composition is implemented through:

- `lexleo_app_default_env()`
- `lexleo_app_create()`
- `lexleo_app_complete_default_init()`

The environment stores borrowed memory, standard I/O, file, string, and time
operations.

Handle creation allocates the handle and its stream-owner container.
It initializes the action to `LEXLEO_APP_ACTION_UNDEFINED` and leaves
runtime resources absent.

Default initialization:

- copies the configured action and path pointers;
- creates missing application streams;
- resolves the log path if it is empty;
- creates a missing logger stream and logger;
- creates the stream factory and its three creators when absent;
- establishes the VM input and output streams;
- creates a missing VM.

Previously initialized or injected resources are preserved. A nonempty
log path is also preserved.

The stream factory and its standard-stream, regular-file, and dynamic-buffer
creators must be supplied together or left entirely absent.

If initialization fails, the handle remains valid for destruction.
Resources retained by completed initialization steps remain attached.
Failure while constructing the factory and creators releases that newly
created group.

## Resource ownership

The application owns its streams, logger, VM, stream factory, stream creators,
and stream-owner container.

For `LEXLEO_APP_ACTION_CONTROL_VM`, missing `vm_in` and `vm_out` fields
become borrowed aliases of `in` and `out`, respectively.

Previously injected VM streams are preserved. A VM stream distinct from its
corresponding application stream is owned by the application.

The only permitted aliases between stream fields are `vm_in == in`
and `vm_out == out`. Each distinct stream is destroyed once.

The logger borrows its target stream. The VM borrows `vm_in`, `vm_out`,
the logger, and the runtime operations.

## Runtime execution

Borrower-facing code runs a fully initialized application through:

- `lexleo_app_run()`

For the currently supported VM control action, this function:

- logs application startup;
- calls `lexleo_vm_run()`;
- logs application shutdown after successful VM execution.

Execution stops at the first logging or VM error and returns the corresponding
application status.

The function does not destroy the handle or its resources. The caller remains
responsible for destruction after either success or failure.

## Lifecycle management

Application handles are destroyed through:

- `lexleo_app_destroy()`

Destruction releases all owned resources, respecting their dependencies:

- the VM is destroyed before its borrowed streams and logger;
- the logger is destroyed before its target stream;
- streams are destroyed before creators and the factory;
- creators are destroyed before the factory;
- the stream-owner container and application handle are released last.

The caller's handle pointer is then set to NULL. Borrowed runtime operations
and borrowed path strings are not destroyed.

Destruction supports handles whose initialization is absent, partial,
or complete. Passing NULL or a pointer to a NULL handle has no effect.

# Related modules

Default application initialization composes resources from the stream,
logger, LexLeo VM, and OSAL families.

See:

- @ref modules_foundation_stream_family "stream family"
- @ref modules_foundation_logger_family "logger family"
- @ref modules_lexleo_vm_family "LexLeo VM family"
- @ref modules_foundation_osal_family "OSAL family"

# See also

- @ref specifications_lexleo_app "lexleo_app specifications"
- @ref testing_lexleo_app_page "lexleo_app tests"
- @ref lexleo_app_tests_group "lexleo_app tests group"
