// src/foundation/stream/include/owners/stream_file_creator.h

#ifndef LEXLEO_STREAM_FILE_CREATOR_H
#define LEXLEO_STREAM_FILE_CREATOR_H

#include "stream/borrowers/stream_types.h"

#include <stdint.h>
#include <stdbool.h>

// such an object MUST be provide by vm at CR
typedef struct stream_file_creator_t {
	stream_status_t (*create)(
		void *ud,
		const char *path,
		uint32_t flags,
		bool autoclose,
		stream_t **out );
	void *ud; // opaque hiding factory and key
} stream_file_creator_t;

#endif //LEXLEO_STREAM_FILE_CREATOR_H
