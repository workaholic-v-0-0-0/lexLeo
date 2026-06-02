@page specifications_lexleo_vm_default_cfg lexleo_vm_default_cfg() specifications

# Signature

```c
lexleo_vm_cfg_t lexleo_vm_default_cfg(void);
```

# Purpose

Return the default configuration value used to create a LexLeo VM handle.

# Success

- Returns a `lexleo_vm_cfg_t` value initialized with the module defaults.

# Failure

- This function cannot fail.

# Ownership

- The returned configuration value is returned by value.
- The caller owns the returned value.
- No destructor is required for `lexleo_vm_cfg_t`.

# Notes

- The configuration structure is currently reserved for future options.
- The returned value may be adjusted by Composition Root code before being
  passed to `lexleo_vm_create()`.
