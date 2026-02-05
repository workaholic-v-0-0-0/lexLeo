// src/foundation/stream/include/lifecycle/stream_lifecycle.h

#ifndef LEXLEO_STREAM_LIFECYCLE_H
#define LEXLEO_STREAM_LIFECYCLE_H

#include "stream/borrowers/stream_types.h"

void stream_destroy(stream_t *s); // stream_t creation via adapter or factory

#endif //LEXLEO_STREAM_LIFECYCLE_H