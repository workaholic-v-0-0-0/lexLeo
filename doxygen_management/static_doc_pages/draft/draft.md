in lexleo_vm:

lexleo_vm_run_file(path)
lexleo_vm_create_session(...) -> session_handle
lexleo_vm_submit_line(session, line)
lexleo_vm_shutdown(vm) (teardown)

in cli:
(make it very thin)
make it do:
lexleo_vm_run_file(path) if lexleo called with a file name argument
OR
lexleo_vm_create_session(...)
lexleo_cli_repl(...)
lexleo_vm_shutdown(vm)

in doc, make a page to explain how making a class diagram for lexLeo
class diagram conventions:

- a "method" of an "class" (a handle) has its first param for the handle itself, so it is not mentionned in param list
  so as in a port module
- if _ctx in param, it is not indicated also
- a module which has no handle will be represented with an interface
- *_ops_t, _deps_t,... will be indicated on "uses" arrow labels
- "adapter/port relation" will be "inheritage relation"
- a "constructor of a port" has params
    - <module port>_t **
    - const <module port>_vtbl_t *vtbl
    - void *backend
    - const <module port>_ctx_t *ctx
      so it is not represented
- in doc, a section to list what diagram does not mention

ownership diagram?

tests of lexleo_flex and lexer:

- to test next and destroy, create a lexer_t even if tests are under adapters/lexleo_flex/
- when testing ports/lexer, do it under ports/lexer but put a fake adapter

