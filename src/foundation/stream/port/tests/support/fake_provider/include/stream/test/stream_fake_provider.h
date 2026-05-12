

#ifndef LEXLEO_STREAM_FAKE_PROVIDER_H
#define LEXLEO_STREAM_FAKE_PROVIDER_H

#include "stream/borrowers/stream.h"
#include "stream/lifecycle/stream_lifecycle.h"
#include "stream/owners/stream_buffer_creator.h"
#include "stream/owners/stream_file_creator.h"
#include "stream/owners/stream_io_creator.h"
#include "stream/cr/stream_cr_api.h"

#include "lexleo/test/fake_stream.h"

#include "osal/mem/osal_mem_ops.h"

stream_t *stream_test_create_fake_stream(const osal_mem_ops_t *mem_ops);

const stream_buffer_creator_t *stream_test_fake_buffer_creator(void);
const stream_file_creator_t *stream_test_fake_file_creator(void);
const stream_io_creator_t *stream_test_fake_io_creator(void);

#endif /* LEXLEO_STREAM_FAKE_PROVIDER_H */
