#------------------------------------------------------------------------------
# doxygen_management/CMakeLists.txt
#
# CMake configuration for managing Doxygen documentation in the lexLeo project.
#
# This file sets up and orchestrates all documentation-related tasks,
# including both static and dynamic documentation generation. It ensures that
# **all project files tracked by git** are processed and wrapped as .dox files,
# not just those that are not directly parsable by Doxygen.
#
# For every git-tracked file, a corresponding .dox file is generated that
# encapsulates its content for verbatim inclusion in the documentation. This
# allows every file (source, configuration, static doc page etc.) to be browsed
# directly in the Doxygen-generated docs, regardless of whether Doxygen can
# parse the file natively.
#
# Each .dox file is assigned a unique @page tag derived from its relative
# path (using '_2' for '/' and '_8' for '.') to ensure compatibility with
# Doxygen's file handling. These generated .dox files are stored within
# ${VERBINCLUDE_WRAPPED_PROJECT_FILES_DIR}, preserving the original project
# structure. The process is automated by the script
# scripts/verbinclude_all_git_tracked_files.cmake
#
# The configuration supports:
#   - Wrapping and verbatim inclusion of all git-tracked files in the docs,
#     even those already parsable by Doxygen.
#   - Standard static documentation generation for source files.
#   - Dynamic documentation targets for link fetching and cross-referencing.
#   - Automated project tree visualization (JSON & Doxygen page).
#   - Synchronization of generated documentation into the source tree.
#
# author: Sylvain Labopin
# date: 2025-06-19
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# DOXYGEN CONFIGURATION
#------------------------------------------------------------------------------


set(
    DOXYGEN_DIR
    "${CMAKE_SOURCE_DIR}/doxygen_management"
)

set(
    DOXYGEN_HEADER
    "${DOXYGEN_DIR}/header.html"
)

set(
    STATIC_DOC_PAGES_DIR
    "${DOXYGEN_DIR}/static_doc_pages"
)

set(
    DYNAMIC_DOC_PAGES_DIR
    "${CMAKE_BINARY_DIR}/doxygen_management/dynamic_doc_pages"
)

set(DOXYGEN_EXAMPLE_PATH "${CMAKE_SOURCE_DIR}")

# temporary documentation generation to fetch links
set(
    DOXYGEN_TEMPLATE
    "${DOXYGEN_DIR}/Doxyfile_to_fetch_links.template"
)

set(
    VERBINCLUDE_WRAPPED_PROJECT_FILES_DIR
    "${DYNAMIC_DOC_PAGES_DIR}/verbinclude_wrapped_project_files"
)

file(
    MAKE_DIRECTORY
    "${VERBINCLUDE_WRAPPED_PROJECT_FILES_DIR}"
)

set(
    DOC_TO_FETCH_LINKS_TMP
    "${CMAKE_SOURCE_DIR}/doc_to_fetch_links_tmp"
)

file(
    MAKE_DIRECTORY
    "${DOC_TO_FETCH_LINKS_TMP}"
)

set(
    DOXYGEN_INPUT_DIR
    ${VERBINCLUDE_WRAPPED_PROJECT_FILES_DIR}
)

set(
    DOXYGEN_OUTPUT_DIR
    "${DOC_TO_FETCH_LINKS_TMP}/docs"
)

set(
    DOXYGEN_EXCLUDE_PATTERN
    ""
)

set(
    DOXYGEN_CONFIG_TO_VERBINCLUDE_GIT_TRACKED_FILES
    "${CMAKE_SOURCE_DIR}/Doxyfile_to_fetch_links"
)

configure_file(
    ${DOXYGEN_TEMPLATE}
    ${DOXYGEN_CONFIG_TO_VERBINCLUDE_GIT_TRACKED_FILES}
    @ONLY
)

# final documentation generation
set(
    DOXYGEN_TEMPLATE
    "${DOXYGEN_DIR}/Doxyfile.template"
)

set(
    DOXYGEN_INPUT_DIR
    "${CMAKE_SOURCE_DIR}"
)

set(
    DOXYGEN_EXAMPLE_PATH
    "${CMAKE_SOURCE_DIR}"
)
set(
    DOXYGEN_OUTPUT_DIR
    "${CMAKE_BINARY_DIR}/docs"
)
file(
    MAKE_DIRECTORY
    "${DOXYGEN_OUTPUT_DIR}"
)

set(
    DOXYGEN_CONFIG
    "${CMAKE_SOURCE_DIR}/Doxyfile"
)

configure_file(${DOXYGEN_TEMPLATE} ${DOXYGEN_CONFIG} @ONLY)

find_package(Doxygen)


#------------------------------------------------------------------------------
# VERBINCLUDE WRAPPED PROJECT FILE PAGES GENERATION
#------------------------------------------------------------------------------

file(
    MAKE_DIRECTORY
    "${DOC_TO_FETCH_LINKS_TMP}/docs"
)

# all files found in source directory except those which are
# already parsable by Doxygen
add_custom_target(
    verbinclude_all_git_tracked_files
    ALL
    COMMAND ${CMAKE_COMMAND}
    -DSOURCE_DIR=${CMAKE_SOURCE_DIR}
    -DOUTPUT_DIR="${VERBINCLUDE_WRAPPED_PROJECT_FILES_DIR}"
    -P ${DOXYGEN_DIR}/scripts/verbinclude_all_git_tracked_files.cmake
    COMMENT "Generating doc pages in ${VERBINCLUDE_WRAPPED_PROJECT_FILES_DIR} for \
Doxygen unparsable files of the source directory"
)


#------------------------------------------------------------------------------
# DOXYGEN DYNAMIC DOCUMENTATION GENERATION TO FETCH LINKS
#------------------------------------------------------------------------------

add_custom_target(
    dynamic_doc
    ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG_TO_VERBINCLUDE_GIT_TRACKED_FILES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating dynamic documentation pages with Doxygen"
    VERBATIM
)
add_dependencies(dynamic_doc verbinclude_all_git_tracked_files)


#------------------------------------------------------------------------------
# MAPPING dynamic dox pages to their related html page in dox_to_html_map.json
#------------------------------------------------------------------------------

set(
    DOX_TO_HTML_MAP_JSON_FILE
    "${DOC_TO_FETCH_LINKS_TMP}/dox_to_html_map.json"
)

add_custom_target(
    map_dox_to_html
    ALL
    COMMAND ${CMAKE_COMMAND}
    -DVERBINCLUDE_WRAPPED_PROJECT_FILES_DIR="${VERBINCLUDE_WRAPPED_PROJECT_FILES_DIR}"
    -DHTML_DIR=${DOC_TO_FETCH_LINKS_TMP}/docs
    -DDOX_TO_HTML_MAP_JSON_FILE=${DOX_TO_HTML_MAP_JSON_FILE}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/scripts/map_dox_to_html.cmake
    COMMENT "mapping dox -> html"
)
add_dependencies(map_dox_to_html dynamic_doc)


#------------------------------------------------------------------------------
# json project tree
#------------------------------------------------------------------------------

set(
    PROJECT_TREE_JSON_FILE
    "${DYNAMIC_DOC_PAGES_DIR}/project_tree.json"
)

file(
    MAKE_DIRECTORY
    "${DYNAMIC_DOC_PAGES_DIR}"
)

# project_tree.json generation
add_custom_target(
    generate_project_tree_json
    ALL
    COMMAND ${CMAKE_COMMAND}
    -DSOURCE_DIR=${CMAKE_SOURCE_DIR}
    -DBUILT_DIR=${CMAKE_BINARY_DIR}
    -DDOX_TO_HTML_MAP_JSON_FILE=${DOX_TO_HTML_MAP_JSON_FILE}
    -DPROJECT_TREE_JSON_FILE=${PROJECT_TREE_JSON_FILE}
    -P ${DOXYGEN_DIR}/scripts/generate_project_tree_json.cmake
    COMMENT "Generating project tree in json format in \
${DYNAMIC_DOC_PAGES_DIR}/project_tree.json"
)
add_dependencies(generate_project_tree_json map_dox_to_html)


#------------------------------------------------------------------------------
# TEMPORARY BUILD SUPPRESSION
#------------------------------------------------------------------------------

add_custom_target(
    remove_doc_to_fetch_links_tmp
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${DOC_TO_FETCH_LINKS_TMP}"
    COMMENT "Removing ${DOC_TO_FETCH_LINKS_TMP}"
)
add_dependencies(remove_doc_to_fetch_links_tmp generate_project_tree_json)


#------------------------------------------------------------------------------
# PROJECT TREE DOCUMENTATION PAGE CONFIGURATION
#------------------------------------------------------------------------------

# project_tree.dox generation
add_custom_target(
    generate_project_tree_dox
    ALL
    COMMAND ${CMAKE_COMMAND}
    -DSOURCE_DIR=${CMAKE_SOURCE_DIR}
    -DBUILT_DIR=${CMAKE_BINARY_DIR}
    -DPROJECT_TREE_JSON_FILE=${PROJECT_TREE_JSON_FILE}
    -P ${DOXYGEN_DIR}/scripts/generate_project_tree_dox.cmake
    COMMENT "Generating doc page with the project tree in dox format in \
${DYNAMIC_DOC_PAGES_DIR}/project_tree.dox"
)
add_dependencies(
    generate_project_tree_dox
    generate_project_tree_json
    map_dox_to_html
)


#------------------------------------------------------------------------------
# DOXYGEN STATIC DOCUMENTATION GENERATION
#------------------------------------------------------------------------------

# Create a custom target to generate API documentation using Doxygen.
add_custom_target(static-doc
    ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating static documentation pages with Doxygen"
    VERBATIM
)
add_dependencies(static-doc generate_project_tree_dox)

# copy doc in sources directory
add_custom_target(
    copy_doc_into_sources_directory
    ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_BINARY_DIR}/docs/html
    ${CMAKE_SOURCE_DIR}/docs
    COMMENT "Copying documentation from build to source directory."
)
add_dependencies(copy_doc_into_sources_directory static-doc)
add_dependencies(static-doc verbinclude_all_git_tracked_files)
add_dependencies(copy_doc_into_sources_directory map_dox_to_html)

# open documentation in a new web browser window
add_custom_target(open_doc
    COMMAND ${CMAKE_COMMAND} -E echo "Opening documentation via Firefox..."
    COMMAND $<TARGET_FILE:open_doc_executable> "${CMAKE_BINARY_DIR}/docs/html/index.html"
    DEPENDS open_doc static-doc
    COMMENT "Opening documentation via Firefox."
)
