set(PROJECT_TREE_DOX_FILE
    "${BUILT_DIR}/docs/dynamic_doc_pages/project_tree.dox"
)

if (NOT EXISTS "${PROJECT_TREE_JSON_FILE}")
    message(FATAL_ERROR "JSON file '${PROJECT_TREE_JSON_FILE}' not found.")
endif()

file(READ "${PROJECT_TREE_JSON_FILE}" PROJECT_TREE_JSON)

configure_file(
    "${SOURCE_DIR}/docs/manual/project_tree.dox.template"
    "${PROJECT_TREE_DOX_FILE}"
    @ONLY
)

message(STATUS "Generated project tree documentation page in ${PROJECT_TREE_DOX_FILE}")
