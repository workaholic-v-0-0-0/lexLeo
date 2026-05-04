@page testing_fake_time fake_time

# Purpose

`fake_time` is a fake backend for time-dependent unit tests.

It allows tests to configure the time returned by the fake, force status
results, reset fake state, and observe call counts.

# Usage

`fake_time` is intended to be injected through a module-specific fake provider,
for example an `osal_time_ops_t` provider used by tests.

# Reference

See @ref test_support_fake_time "fake_time reference".
