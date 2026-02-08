// src/foundation/stream/port/include/adapters/stream_constructor.h

#ifndef LEXLEO_STREAM_CONSTRUCTOR_H
#define LEXLEO_STREAM_CONSTRUCTOR_H

#include "stream/borrowers/stream_types.h"

// constructor that an implement must implement
typedef stream_status_t (*stream_ctor_fn_t)(
	void *ud,
	const void *args,
	stream_t **out);

#endif //LEXLEO_STREAM_CONSTRUCTOR_H