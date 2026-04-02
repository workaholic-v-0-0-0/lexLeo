@page specifications_logger_default_default_cfg logger_default_default_cfg() specifications

# Signature

    logger_default_cfg_t logger_default_default_cfg(void);

# Purpose

Return a default configuration for the `logger_default` adapter.

# Preconditions

- None.

# Invalid arguments

- None.

# Success

- Returns a well-formed default `logger_default_cfg_t`.
- `ret.append_newline == true`.

# Failure

- None.

# Notes

- This helper provides a baseline configuration that the Composition Root may
  override.
- This helper does not allocate.
