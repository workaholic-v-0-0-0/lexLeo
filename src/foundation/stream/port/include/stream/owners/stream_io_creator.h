// src/foundation/stream/include/owners/stream_io_creator.h

#ifndef LEXLEO_STREAM_IO_CREATOR_H
#define LEXLEO_STREAM_IO_CREATOR_H

#include "stream/borrowers/stream_types.h"

typedef enum stream_io_kind_t {
	STREAM_IO_INPUT = 0,
	STREAM_IO_OUTPUT,
	STREAM_IO_ERR
} stream_io_kind_t;

// such an object MUST be pprovide by vm at CR
typedef struct stream_io_creator_t {
	stream_status_t (*create)(
		void *ud,
		stream_io_kind_t kind,
		stream_t **out );
	void *ud; // opaque hiding factory and key
} stream_io_creator_t;

#endif //LEXLEO_STREAM_IO_CREATOR_H