/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_vm_owned_resources_type.h
 * @ingroup lexleo_vm_internal_group
 * @brief Private owned-resource container for lexleo_vm.
 *
 * @details
 * Defines the fields holding the stream factory and stream creators
 * owned by the VM.
 */

#ifndef LEXLEO_LEXLEO_VM_OWNED_RESOURCES_TYPE_H
#define LEXLEO_LEXLEO_VM_OWNED_RESOURCES_TYPE_H

#include "stream/common/stream_factory_opaque_type.h"

#include "stream/owners/stream_creators_api.h"

struct lexleo_vm_owned_resources_t {

	/** Owned stream factory. */
	stream_factory_t *stream_factory;

	/** Owned stdio stream creator. */
	stream_standard_stream_creator_t *stream_standard_stream_creator;

	/** Owned file stream creator. */
	stream_regular_file_creator_t *stream_regular_file_creator;

	/** Owned buffer stream creator. */
	stream_dynamic_buffer_creator_t *stream_dynamic_buffer_creator;

};

#endif /* LEXLEO_LEXLEO_VM_OWNED_RESOURCES_TYPE_H */
