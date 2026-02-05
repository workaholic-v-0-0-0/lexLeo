// src/foundation/stream/include/owners/stream_buffer_creator.h

#ifndef LEXLEO_STREAM_BUFFER_CREATOR_H
#define LEXLEO_STREAM_BUFFER_CREATOR_H

#include "stream/borrowers/stream_types.h"

// such an object MUST be provide by vm at CR
typedef struct stream_buffer_creator_t {
	stream_status_t (*create)(void *ud, stream_t **out);
	void *ud; // opaque hiding factory and key
} stream_buffer_creator_t;

#endif //LEXLEO_STREAM_BUFFER_CREATOR_H
