set(
    PROJECT_TREE_DOX_FILE
    "${BUILT_DIR}/doxygen_management/dynamic_doc_pages/project_tree.dox"
)

if(EXISTS "${PROJECT_TREE_JSON_FILE}")
    if(EXISTS "${PROJECT_TREE_DOX_FILE}")
        file(REMOVE "${PROJECT_TREE_DOX_FILE}")
    endif()
    file(READ "${PROJECT_TREE_JSON_FILE}" JSON_CONTENT)
    set(PROJECT_TREE_JSON "${JSON_CONTENT}")
    configure_file(
        "${SOURCE_DIR}/doxygen_management/static_doc_pages/project_tree.dox.template"
        "${PROJECT_TREE_DOX_FILE}"
    )
else()
    message(FATAL_ERROR "JSON file '${PROJECT_TREE_JSON_FILE}' not found.")
endif()

message(
    STATUS
    "Generated project tree documentation page in ${PROJECT_TREE_DOX_FILE}"
)
