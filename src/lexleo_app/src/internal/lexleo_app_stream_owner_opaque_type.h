/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_app_stream_owner_opaque_type.h
 * @ingroup lexleo_app_internal_group
 * @brief Private definition of the stream owner resources for lexleo_app.
 *
 * @details
 * Defines the container holding the stream factory and stream creators
 * owned by the lexleo_app handle.
 * This definition is exposed only to implementation files requiring
 * access to these resources.
 */

#ifndef LEXLEO_LEXLEO_APP_STREAM_OWNER_OPAQUE_TYPE_H
#define LEXLEO_LEXLEO_APP_STREAM_OWNER_OPAQUE_TYPE_H

#include "stream/common/stream_factory_opaque_type.h"
#include "stream/owners/stream_creators_api.h"

struct lexleo_app_stream_owner_opaque_t {

	/** Owned stream factory. */
	stream_factory_t *stream_factory;

	/** Owned stdio stream creator. */
	stream_standard_stream_creator_t *stream_standard_stream_creator;

	/** Owned file stream creator. */
	stream_regular_file_creator_t *stream_regular_file_creator;

	/** Owned buffer stream creator. */
	stream_dynamic_buffer_creator_t *stream_dynamic_buffer_creator;

};

#endif /* LEXLEO_LEXLEO_APP_STREAM_OWNER_OPAQUE_TYPE_H */
