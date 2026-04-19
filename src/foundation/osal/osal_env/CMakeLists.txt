add_library(
	osal_env
	STATIC
		"${CMAKE_CURRENT_SOURCE_DIR}/src/osal_env.c"
)

target_include_directories(
	osal_env
	PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
	osal_env
	PRIVATE
		lexleo_policy_headers
)
