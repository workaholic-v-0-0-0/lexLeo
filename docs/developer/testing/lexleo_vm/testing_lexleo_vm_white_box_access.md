@page testing_lexleo_vm_white_box_access lexleo_vm white-box test access

# Purpose

Provide test-only helpers used to observe and replace selected internal
owned-resource fields of LexLeo VM handles.

These helpers support white-box tests that need to inspect default
owned-resource initialization or inject controlled dependencies into internal
VM fields.

# Scope

This support API is limited to tests.

It is not part of the production API and must not be used by production code.

# Injected resources

White-box tests may inject resources into internal owned-resource fields of a
LexLeo VM handle.

Even when stored in internal owned-resource fields, injected resources remain
owned by the test.

# Ownership rule

Before calling `lexleo_vm_destroy()`, a test that injected a resource must
destroy the injected resource with its matching idempotent destructor.

The destructor must reset the corresponding VM field to `NULL`.

This prevents `lexleo_vm_destroy()` from destroying a resource still owned by
the test.

# Observed resources

Getter helpers expose selected internal owned-resource fields so that tests can
verify whether default initialization created, reused, or skipped a resource.

Returned pointers are borrowed observations only.

Tests must not destroy resources obtained only through getter helpers unless
the test also owns those resources.

# Notes

- White-box access helpers may include private LexLeo VM headers.
- These helpers intentionally expose implementation details to tests.
- Changes to internal VM fields may require updating this support API.
