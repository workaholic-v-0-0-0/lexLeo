# tools/CMakeLists.txt

add_executable(open_doc_executable open_doc.c)
target_include_directories(open_doc_executable PRIVATE ${CMAKE_SOURCE_DIR}/src/foundation/osal/include)
target_link_libraries(open_doc_executable PRIVATE osal)

add_executable(update_docs_on_droplet_executable update_docs_on_droplet.c)
