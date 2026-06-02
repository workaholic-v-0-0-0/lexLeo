@page modules_lexleo_vm lexleo_vm

# Purpose

The `lexleo_vm` module provides the public API used to construct, initialize,
run, and destroy LexLeo VM handles.

It is responsible for:
- creating VM handles from configuration values and borrowed runtime
  dependencies;
- storing borrowed runtime services required by the VM;
- completing default initialization by creating internal owned runtime
  resources;
- exposing the borrower-facing runtime function used to run a VM;
- destroying VM handles and their owned runtime resources.

# Public API

- @ref lexleo_vm_api "lexleo_vm API"

Sub-APIs:
- @ref lexleo_vm_borrowers_api "borrowers API"
- @ref lexleo_vm_cr_api "CR API"
- @ref lexleo_vm_lifecycle_api "lifecycle API"

# Architectural role

The `lexleo_vm` module is the Composition Root boundary of the LexLeo virtual
machine runtime.

It bridges:
- borrowed runtime dependencies provided by the application environment;
- owned runtime resources created during VM initialization;
- borrower-facing runtime execution through `lexleo_vm_run()`.

The public handle type `lexleo_vm_t` is opaque. Its private representation
stores both borrowed dependencies and owned runtime resources.

# Main concepts

## VM handle creation

Composition Root code creates a VM handle through:

- `lexleo_vm_default_cfg()`
- `lexleo_vm_default_env()`
- `lexleo_vm_create()`

`lexleo_vm_default_env()` stores borrowed dependencies in an explicit
environment value. These dependencies are not owned by the VM.

## Default owned-resource initialization

After creation, Composition Root code completes the default runtime wiring
through:

- `lexleo_vm_complete_default_init()`

This creates the internal owned resources required by the default VM runtime,
including the default stream factory and owner-facing stream creators.

## Runtime execution

Borrower-facing code runs a previously created and initialized VM through:

- `lexleo_vm_run()`

This function is currently a placeholder runtime entry point.

## Lifecycle management

VM handles are destroyed through:

- `lexleo_vm_destroy()`

This releases the VM handle and every owned runtime resource attached to it.
Borrowed dependencies stored in the VM environment are not destroyed.

# Related modules

The current default VM initialization wires resources from the `stream` family.

See:
- @ref modules_foundation_stream_family "stream family"
- @ref modules_foundation_logger_family "logger family"
- @ref modules_foundation_osal_family "OSAL family"

# See also

- @ref specifications_lexleo_vm "lexleo_vm specifications"
- @ref testing_lexleo_vm_page "lexleo_vm tests"
- @ref lexleo_vm_tests_group "lexleo_vm tests group"