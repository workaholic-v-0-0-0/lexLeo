@page modules_lexleo_app lexleo_app

# Purpose

The `lexleo_app` module provides the public API used to configure, construct,
run, and destroy LexLeo application handles.

It is responsible for:

- providing default application configuration values;
- creating application handles with the default runtime environment;
- composing application components and their runtime dependencies;
- exposing the borrower-facing runtime function used to run the application;
- destroying application handles and their owned runtime resources.

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
stores borrowed runtime operations and owned application resources.

Environment construction and separate creation and initialization services
remain internal. Test support provides access to these services for dependency
injection and unit testing.

# Main concepts

## Application configuration

Client code obtains the default configuration through:

- `lexleo_app_default_cfg()`

The configuration selects standard streams, files, or dynamic buffers for
application input, output, and error streams.

## Application construction and initialization

Client code creates a fully initialized application through:

- `lexleo_app_create_init()`

This function uses the supplied configuration and the default runtime
environment. It combines handle creation and component initialization without
exposing internal Composition Root details to the client.

If creation or initialization fails, the function releases its temporary
application handle and leaves the caller's output handle unchanged.

## Internal composition services

Application composition is implemented through:

- `lexleo_app_default_env()`
- `lexleo_app_create()`
- `lexleo_app_complete_default_init()`

The environment stores borrowed memory, standard I/O, file, string, and time
operations.

Default initialization creates missing application streams, resolves the log
path, constructs the logger stream and logger, and creates the VM. Previously
initialized or injected fields are preserved.

The application owns its streams, logger, and VM. The logger borrows its target
stream, and the VM borrows the application streams, logger, and runtime
operations.

## Runtime execution

Borrower-facing code runs a previously created and initialized application
through:

- `lexleo_app_run()`

Its execution behavior remains to be defined.

## Lifecycle management

Application handles are destroyed through:

- `lexleo_app_destroy()`

This releases the application handle and every owned runtime resource attached
to it. Borrowed runtime operations are not destroyed.

Destruction also supports partially initialized application handles.

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
