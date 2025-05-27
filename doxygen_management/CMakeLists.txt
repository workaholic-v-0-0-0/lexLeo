# doxygen_management/CMakeLists.txt

#------------------------------------------------------------------------------
# doxygen_management/CMakeLists.txt
#
# This CMakeLists.txt is dedicated to the configuration and execution of
# Doxygen within the project. It manages both dynamic and static documentation
# tasks, ensuring all necessary files and directories are processed and
# updated accordingly.
#
# Files not directly parsable by Doxygen are managed by creating a .dox copy
# that encapsulates the original content between @htmlonly and @endhtmlonly
# tags to ensure proper inclusion in the generated documentation pages. These
# .dox files are stored within ${FOR_UNPARSABLE_IN_SOURCE_DIR}, maintaining the
# original directory structure. Each .dox file is assigned a unique identifier
# through a @page tag based on its relative path to this directory. Identifiers
# replace '/' with '_2' and '.' with '_8', ensuring compatibility with
# Doxygen's file handling. The copying and tagging are managed by the script
# scripts/make_parsable_not_source_files.cmake.
#
# author: Sylvain Labopin
# date: 2025-05-27
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
    CACHE PATH "Dynamic doc pages dir"
)

file(
    MAKE_DIRECTORY
    "${DYNAMIC_DOC_PAGES_DIR}"
)

set(DOXYGEN_EXAMPLE_PATH "${CMAKE_SOURCE_DIR}")

# temporary documentation generation to fetch links
set(
    FOR_UNPARSABLE_IN_SOURCE_DIR
    "${DYNAMIC_DOC_PAGES_DIR}/for_unparsable_in_source_dir"
)

file(
    MAKE_DIRECTORY
    "${FOR_UNPARSABLE_IN_SOURCE_DIR}"
)

set(
    DOXYGEN_TEMPLATE
    "${DOXYGEN_DIR}/Doxyfile_to_fetch_links.template"
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
    ${FOR_UNPARSABLE_IN_SOURCE_DIR}
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
    DOXYGEN_CONFIG_TO_MAKE_PARSABLE_ALL_FILES
    "${CMAKE_SOURCE_DIR}/Doxyfile_to_fetch_links"
)

configure_file(
    ${DOXYGEN_TEMPLATE}
    ${DOXYGEN_CONFIG_TO_MAKE_PARSABLE_ALL_FILES}
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
    #"${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/build/doxygen_management/dynamic_doc_pages ${CMAKE_SOURCE_DIR}/doxygen_management/static_doc_pages"
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
# DOXYGEN DOCUMENTATION PAGES GENERATION FOR NOT SOURCE FILES
#------------------------------------------------------------------------------

file(
    MAKE_DIRECTORY
    "${DOC_TO_FETCH_LINKS_TMP}/docs"
)

# all files found in source directory except those which are
# already parsable by Doxygen
add_custom_target(
    make_parsable_not_source_files
    ALL
    COMMAND ${CMAKE_COMMAND}
    -DSOURCE_DIR=${CMAKE_SOURCE_DIR}
    -DOUTPUT_DIR="${FOR_UNPARSABLE_IN_SOURCE_DIR}"
    -P ${DOXYGEN_DIR}/scripts/make_parsable_not_source_files.cmake
    COMMENT "Generating doc pages in ${FOR_UNPARSABLE_IN_SOURCE_DIR} for \
Doxygen unparsable files of the source directory"
)


#------------------------------------------------------------------------------
# DOXYGEN DYNAMIC DOCUMENTATION GENERATION TO FETCH LINKS
#------------------------------------------------------------------------------

add_custom_target(
    dynamic_doc
    ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG_TO_MAKE_PARSABLE_ALL_FILES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating dynamic documentation pages with Doxygen"
    VERBATIM
)
add_dependencies(dynamic_doc make_parsable_not_source_files)


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
    -DFOR_UNPARSABLE_IN_SOURCE_DIR="${FOR_UNPARSABLE_IN_SOURCE_DIR}"
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
add_dependencies(static-doc make_parsable_not_source_files)
add_dependencies(copy_doc_into_sources_directory map_dox_to_html)
