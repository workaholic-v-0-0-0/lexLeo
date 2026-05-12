@page testing_fake_stream fake_stream

# Purpose

`fake_stream` provides a deterministic fake stream backend used by unit tests
to exercise stream-dependent code through dependency injection.

The module allows tests to:

- create fake stream handles,
- inject fake backends into stream creators,
- configure read/write/flush/close behavior,
- simulate stream operation failures,
- observe stream operation calls,
- inspect buffered and flushed data,
- and validate stream interaction behavior.

# Provided fake components

The module provides:

- fake stream backends,
- fake stream buffer creators,
- fake stream file creators,
- fake stream io creators,
- backend configuration helpers,
- and spy helpers.

# Backend behavior

Fake stream backends operate on in-memory backing buffers:

- `buffered_backing`
    - Represents buffered stream content.

- `sink_backing`
    - Represents flushed stream content.

Read and write operations update the buffered backing.
Flush operations synchronize buffered content into the sink backing.

# Failure injection

Tests can configure operation failures independently for:

- read,
- write,
- flush,
- and close.

The module also supports configurable no-op behavior for selected operations.

# Spy support

The module records:

- stream creator calls,
- operation call counts,
- last operation arguments,
- and stream backend state.

Tests can inspect these values through accessor helpers.

# Notes

The fake backend is intended for deterministic unit testing only.

Backends created by `fake_stream_create_fake_backend()` are owned by the
test environment and are typically released during test teardown.
