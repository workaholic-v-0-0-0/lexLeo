add_library(
	osal_str
	STATIC
		"${CMAKE_CURRENT_SOURCE_DIR}/src/osal_str.c"
)

target_include_directories(
	osal_str
	PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
	osal_str
	PRIVATE
		osal_mem
		lexleo_policy_headers
)
