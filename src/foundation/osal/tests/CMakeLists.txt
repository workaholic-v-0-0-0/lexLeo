# src/foundation/osal/tests/CMakeLists.txt

# OSAL tests are contract-based and run against the real platform.
# OSAL submodules do not use dependency doubles.
# Dependency faking starts strictly above OSAL (consumer modules).

add_subdirectory(support/fake_provider)
