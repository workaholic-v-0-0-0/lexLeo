@page testing_fake_file fake_file

# Purpose

`fake_file` is a fake backend for file-dependent unit tests.

It allows tests to configure fake `open()` and `mkdir()` results, prepare fake
file handles, control operation statuses, inspect buffered and sink data, and
observe file operation calls.

# Usage

`fake_file` is intended to be injected through a module-specific fake provider,
for example an `osal_file_ops_t` provider used by tests.

# Reference

See @ref test_support_fake_file "fake_file reference".
