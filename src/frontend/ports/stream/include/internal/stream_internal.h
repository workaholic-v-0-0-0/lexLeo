// src/frontend/ports/stream/include/internal/stream_internal.h

/*
    ARCHITECTURE OF THE STREAM MODULE — HOW IT WORKS

    This module provides a generic stream abstraction with read, write,
    flush, and close operations. The goal is to expose a single unified
    interface while allowing multiple concrete backends (stdio_stream, OSAL
    files, memory buffers, fake streams for testing, etc.).

    -------------------------------------------------------------------------
    1) 'stream' is a completely opaque type
    -------------------------------------------------------------------------

    The struct 'stream' is NOT defined in any public header. It is defined
    only in stream.c:

        struct stream {
            const stream_vtable *ops;
            void *ctx;
        };

    The outside world only sees:

        typedef struct stream stream;

    and can only call:

        stream_read(),
        stream_write(),
        stream_flush(),
        stream_close()

    This is equivalent to a C++ *abstract class* with a public interface and
    private data members.

    -------------------------------------------------------------------------
    2) The vtable: "virtual methods" in C
    -------------------------------------------------------------------------

    The internal header (stream_internal.h) exposes stream_vtable to
    adapters. It contains pointers to the backend's implementation of:

        read(ctx, ...), write(ctx, ...), flush(ctx), close(ctx)

    This is the C equivalent of virtual methods in an object-oriented class.

    -------------------------------------------------------------------------
    3) ctx: backend-specific context
    -------------------------------------------------------------------------

    Each backend defines its own context structure, for example:

        typedef struct { FILE *fp; } stream_fp_ctx;

    The stream object stores this context as a void*, and never inspects it.
    Only the backend knows its true type.

    -------------------------------------------------------------------------
    4) stream_create: internal constructor
    -------------------------------------------------------------------------

    stream_create() is INTERNAL. It combines:
        - a vtable (function pointers)
        - a context pointer (backend state)
    into a fully constructed stream* stream.

    Only adapters call stream_create().
    Application code should never include stream_internal.h.

    -------------------------------------------------------------------------
    5) Adapter factories (public constructors)
    -------------------------------------------------------------------------

    Users obtain a stream* by calling PUBLIC factory functions provided
    by each adapter, such as:

        stream *fs_stream_from_file(const char *path);
        stream *stdio_stream_from_stdin(void);

    These factory functions:
        - allocate and initialize the backend context,
        - define the backend's vtable,
        - call stream_create() internally.

    The user only includes:
        - stream.h (for the generic interface)
        - the adapter's factory header (e.g. stdio_stream.h)

    -------------------------------------------------------------------------
    Summary:

        - stream.h:
            Public API. 'stream' is opaque. Only read/write/flush/close are visible.

        - stream_internal.h:
            For adapter implementors only.
            Contains vtable definition + stream_create().

        - stream.c:
            Contains the real definition of struct stream (private) and the
            implementation of the generic interface.

        • Users of a stream:
            Include stream.h + an adapter’s factory header.
            Never see the vtable or stream_create.

        • Implementors of a backend:
            Include stream_internal.h, implement read/write/flush/close, then
            call stream_create().

    This pattern is the exact C equivalent of an abstract class with pure
    virtual methods and multiple concrete subclasses.
*/


#ifndef LEXLEO_STREAM_INTERNAL_H
#define LEXLEO_STREAM_INTERNAL_H

#include "stream.h"
#include "internal/stream_memory_allocator.h"

#include <stddef.h>

typedef struct stream_vtable {
	size_t (*read)(void *ctx, void* buf, size_t n);
	size_t (*write)(void *ctx, const void* buf, size_t n);
	int (*flush)(void *ctx);
	int (*close)(void *ctx);
} stream_vtable;

stream *stream_create(const stream_vtable *vtbl, void *ctx);

#endif //LEXLEO_STREAM_INTERNAL_H
