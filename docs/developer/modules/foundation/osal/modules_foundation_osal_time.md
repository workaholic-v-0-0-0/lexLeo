@page modules_foundation_osal_time osal_time

# Purpose

`osal_time` is the low-level OSAL time module.

It provides portable wall-clock time access through injectable time operations.

# Public API

See @ref osal_time_api "osal_time API".

# Specifications

See @subpage specifications_osal_time "osal_time specifications".

# Tests

See @subpage testing_foundation_osal_time_unit "osal_time unit tests".

# Responsibilities

- Provide default wall-clock time retrieval.
- Represent wall-clock time as Unix epoch seconds.
- Provide an injectable `osal_time_ops_t` table for modules that need time
  dependency injection.
- Report invalid arguments and backend time query failures through
  `osal_time_status_t`.
