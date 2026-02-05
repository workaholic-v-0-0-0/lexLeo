// src/lexleo_vm/src/stream_creator.c

// implement src/foundation/stream/port/include/stream/owners/stream_*_creator.h

// todo

/* draft
 * REGISTRATION OF ADAPTERS WITHOUT GUARDS
const osal_mem_ops_t *mem = osal_mem_default_ops();
stream_env_t stream_env = stream_default_env(mem);

stream_factory_t *fact = NULL;
stream_factory_cfg_t stream_factory_cfg = stream_default_factory_cfg();
stream_status_t st =
	stream_create_factory(
		&fact,
		&stream_factory_cfg,
		&stream_env );

dynamic_buffer_stream_cfg_t dbs_cfg = dynamic_buffer_stream_default_cfg();
dynamic_buffer_stream_env_t dbs_env =
	dynamic_buffer_stream_default_env(mem, stream_env);
stream_adapter_desc_t dbs_desc = (stream_adapter_desc_t){0};
st =
	dynamic_buffer_stream_create_desc(
		&dbs_desc,
		"dbs",
		&dbs_cfg,
		&dbs_env,
		mem ); // MUST be factory allocator
st =
	stream_factory_add_adapter(
		fact,
		&dbs_desc );

fs_stream_cfg_t fs_cfg = fs_stream_default_cfg();
fs_stream_env_t fs_env =
	fs_stream_default_env(mem, stream_env);
stream_adapter_desc_t fs_desc = (stream_adapter_desc_t){0};
st =
	fs_stream_create_desc(
		&fs_desc,
		"fs",
		&fs_cfg,
		&fs_env,
		mem ); // MUST be factory allocator
st =
	stream_factory_add_adapter(
		fact,
		&fs_desc );

//...
*/