@page development_workflow Development workflow

# Documentation for one module

- Doxygen groups for "Reference":
  - "API"
    - mod/mod_group.dox
      - mod/include/mod_api.dox
        - mod/include/mod/scope_1/mod_scope_1_api.dox
        - mod/include/mod/scope_2/mod_scope_2_api.dox
  - "Private API"
    - mod/src/mod_internal_group.dox
  - "Tests"
    - mod/tests/mod_test_group.dox
      - mod_unit_tests.dox
      - mod_integration_tests.dox

- hand-written documentation pages describing tested scenarios:
  - docs/developer/testing/.../mod/testing_..._mod_..._unit.md
  - docs/developer/testing/.../mod/testing_..._mod_..._integration.md

- hand-written documentation page describing function specifications:
  - docs/developer/specifications/.../mod/specifications_mod.md

- hand-written module page with links to "Reference" and "Specifications":
  - docs/developer/modules/.../mod/mod.md (module page)

# Build, test and documentation commands

## Rebuild, generate documentation and open it with a web browser

   ```bash
   ./scripts/re_debug_build-make_and_open_doc.sh
   ```

## Rebuild and run unit tests without memory checking

   ```bash
   ./scripts/re_debug_build-check.sh 
   ```

## Rebuild and run unit tests with memory checking

   ```bash
   ./scripts/re_debug_build-check_memory.sh
   ```

## Rebuild and update documentation on droplet (if SSH access is configured) :

   ```bash
   ./scripts/update_docs_on_droplet.sh 
   ```
