# tools/CMakeLists.txt

add_executable(open_doc_executable open_doc.c)
target_include_directories(open_doc_executable PRIVATE ${CMAKE_SOURCE_DIR}/src/osal/include)
target_link_libraries(open_doc_executable PRIVATE osal)
