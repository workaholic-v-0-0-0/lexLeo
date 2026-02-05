@page modules_core_lexer_lexleo_flex lexleo_flex module

This module provides a Flex-based lexer backend for lexLeo.

## Design principles

- The lexer core is independent from Flex.
- Flex is used only as a backend implementation.
- Memory management is fully delegated to OSAL memory ops.
- Input is abstracted via input_provider.

## Internal structure

- lexleo_flex_ctx_t: wiring and dependency injection
- lexleo_flex_backend: Flex scanner and runtime state

This module is not exposed as part of the public API.

## Internal structure

The Flex integration is structured as a thin adapter around a reentrant
Flex scanner. The module follows the standard lexLeo separation between
wiring, runtime state, and execution logic.

The main internal components are:

- `lexleo_flex_ctx_t`  
  Wiring context used at creation time.  
  It aggregates all injected dependencies (`lexleo_flex_deps_t`) and
  provides the lexer virtual table (`lexer_vtbl_t`) consumed by the core
  lexer module.  
  The context is *consumed during construction* and is not retained by
  the runtime instance.

- `lexleo_flex_deps_t`  
  Value object grouping external dependencies required by the adapter:
    - OSAL memory operations (`osal_mem_ops_t`)
    - Input provider operations (`input_provider_ops_t`)  
      These dependencies are borrowed and remain owned by the caller.

- `lexleo_flex_t`  
  Concrete lexer backend instance.  
  It owns the Flex runtime state and implements the lexer port through
  the provided virtual table.

- `lexleo_flex_state_t`  
  Runtime execution state owned by the backend.  
  It embeds the Flex scanner handle and the associated `yyextra` data.

- `lexleo_flex_scanner_extra_t`  
  Flex `yyextra` structure carrying:
    - a borrowed `input_provider` instance,
    - the injected read callback,
    - OSAL memory operations,
    - pending token buffering state.

The following diagram summarizes the structural relationships and ownership
rules inside the Flex adapter:

![lexleo_flex internal structure](class_diagram_lexleo_flex.png)
