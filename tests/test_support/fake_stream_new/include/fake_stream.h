

#ifndef LEXLEO_FAKE_STREAM_H
#define LEXLEO_FAKE_STREAM_H

#include "stream/"
//<here>
typedef struct fake_stream_ctrl_t {

	/* cfg */


	/* spy */


} fake_stream_ctrl_t;

typedef struct fake_stream_backend_t {

	/* cfg */


	/* spy */


} fake_stream_backend_t;

fake_stream_vtbl_t

typedef struct fake_stream_t {

	/** To be bound to the fake implementations. */
	const stream_vtbl_t *vtbl;

	/** Borrowed memory operations used to destroy the handle. */
	const osal_mem_ops_t *mem;

	/** Owned opaque adapter backend instance. */
	void *backend;

} fake_stream_t;

typedef struct fake_stream_creator_t {

	/* cfg */


	/* spy */


} fake_stream_creator_t;
//<here>
#endif /* LEXLEO_FAKE_STREAM_H */
