@page development_workflow Development workflow

# Documentation for one module

A module is documented through four complementary layers:

- **Reference (Doxygen)**
  - **API**
    - `mod/mod_group.dox`
      - `mod/include/mod_api.dox`
        - `mod/include/mod/scope_1/mod_scope_1_api.dox`
        - `mod/include/mod/scope_2/mod_scope_2_api.dox`
  - **Private API**
    - `mod/src/mod_internal_group.dox`
  - **Tests**
    - `mod/tests/mod_test_group.dox`
      - `mod_unit_tests.dox`
      - `mod_integration_tests.dox`

- **Hand-written testing pages**
  - `docs/developer/testing/.../mod/testing_..._mod_..._unit.md`
  - `docs/developer/testing/.../mod/testing_..._mod_..._integration.md`

- **Hand-written specification pages**
  - `docs/developer/specifications/.../mod/specifications_mod.md`

- **Hand-written module page**
  - `docs/developer/modules/.../mod/mod.md`

The module page links to the corresponding **Reference**, **Specifications**,
and **Tests**.

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

# steps for a commit for a module:

## 1. define or update the public API surface
- include/... headers
- include/... *.dox group pages

## 2. write or update the hand-written specifications
- docs/developer/specifications/...

## 3. for each TDD cycle:
- update tests/...
- update src/...
- update Doxygen comments in headers/sources when the contract stabilizes
- update docs/developer/specifications/... if the contract changed
- compile and run tests

## 4. once the tested behavior is stable:
- write or update docs/developer/testing/...

## 5. once the module shape is clear enough:
- write or update docs/developer/modules/...
    (hand-written module page with links to Reference, Specifications, and Tests)

## 6. final validation before commit
- compile
- run tests
- generate/check Doxygen
- verify main refs/subpages/groups

## Brief

API, specifications, scenarios, TDD implementation, testing page, module page
