@page testing_fake_stdio fake_stdio

# Purpose

`fake_stdio` is a fake backend for stdio-dependent unit tests.

It provides fake `stdin`, `stdout`, and `stderr` streams that can be configured,
observed, and injected instead of real runtime stdio dependencies.

# Usage

Tests can use `fake_stdio` to:
- configure readable stream contents,
- observe read, write, and flush calls,
- inspect buffered data,
- inspect flushed sink data,
- reset fake state between tests.

`fake_stdio` is intended to be injected through a module-specific fake provider,
for example an `osal_stdio_ops_t` provider used by tests.

# Reference

See @ref test_support_fake_stdio "fake_stdio reference".
