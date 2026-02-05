#------------------------------------------------------------------------------
# docs/CMakeLists.txt
#
# CMake configuration for managing Doxygen documentation in the lexLeo project.
#
# This file sets up and orchestrates all documentation-related tasks,
# including both static and dynamic documentation generation.
#
# The configuration supports:
#   - Standard static documentation generation for source files.
#   - Automated project tree visualization (JSON & Doxygen page).
#   - Synchronization of generated documentation into the source tree.
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# GITHUB LINK BASE (repo + ref)
#------------------------------------------------------------------------------

execute_process(
    COMMAND git -C "${CMAKE_SOURCE_DIR}" config --get remote.origin.url
    OUTPUT_VARIABLE GIT_REMOTE_URL
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE GIT_REMOTE_URL_RC
)

execute_process(
    COMMAND git -C "${CMAKE_SOURCE_DIR}" rev-parse HEAD
    OUTPUT_VARIABLE GIT_REF
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE GIT_REF_RC
)

execute_process(
    COMMAND git -C "${CMAKE_SOURCE_DIR}" describe --always --dirty
    OUTPUT_VARIABLE GIT_DESCRIBE
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE GIT_DESCRIBE_RC
)

if (NOT GIT_REMOTE_URL_RC EQUAL 0 OR NOT GIT_REF_RC EQUAL 0)
    set(GIT_REMOTE_URL "")
    set(GIT_REF "")
endif()

set(REPO_WEB_URL "${GIT_REMOTE_URL}")
string(REGEX REPLACE "^git@github.com:" "https://github.com/" REPO_WEB_URL "${REPO_WEB_URL}")
string(REGEX REPLACE "^ssh://git@github.com/" "https://github.com/" REPO_WEB_URL "${REPO_WEB_URL}")
string(REGEX REPLACE "\\.git$" "" REPO_WEB_URL "${REPO_WEB_URL}")

# If git is not available / no remote, REPO_WEB_URL may be empty. In that case, links will be omitted.

#------------------------------------------------------------------------------
# DOXYGEN PROJECT_NUMBER (0.0.0-dev+<hash>)
#------------------------------------------------------------------------------

set(DOXYGEN_PROJECT_NUMBER "0.0.0-dev")

if (GIT_DESCRIBE_RC EQUAL 0 AND GIT_DESCRIBE)
    set(DOXYGEN_PROJECT_NUMBER "0.0.0-dev+${GIT_DESCRIBE}")
elseif (GIT_REF)
    string(SUBSTRING "${GIT_REF}" 0 7 GIT_HASH_SHORT)
    set(DOXYGEN_PROJECT_NUMBER "0.0.0-dev+${GIT_HASH_SHORT}")
endif ()

set(DOXYGEN_DIR "${CMAKE_SOURCE_DIR}/docs/doxygen")
set(DOXYGEN_HEADER "${DOXYGEN_DIR}/header.html")
set(DOXYGEN_FOOTER "${DOXYGEN_DIR}/footer.html")
set(DOXYGEN_EXTRA_CSS "${DOXYGEN_DIR}/lexleo.css")

#0.0.0-dev+<hash>
set(DOXYGEN_PROJECT_LOGO "${DOXYGEN_DIR}/leo.png")
set(STATIC_DOC_PAGES_DIR "${CMAKE_SOURCE_DIR}/docs/developer")
set(DYNAMIC_DOC_PAGES_DIR "${CMAKE_BINARY_DIR}/docs/dynamic_doc_pages")

set(DOXYGEN_TEMPLATE "${DOXYGEN_DIR}/Doxyfile.in")
set(
    DOXYGEN_INPUT_DIR
        "${CMAKE_SOURCE_DIR}/docs/reference/mainpage.md"
        "${CMAKE_SOURCE_DIR}/docs/reference/reference.dox"
        "${CMAKE_SOURCE_DIR}/docs/user/user_guide.md"
        "${CMAKE_SOURCE_DIR}/src"
        "${STATIC_DOC_PAGES_DIR}"
        "${DYNAMIC_DOC_PAGES_DIR}"
)
string(JOIN " " DOXYGEN_INPUT_DIR_STR ${DOXYGEN_INPUT_DIR})
set(DOXYGEN_MAINPAGE "${CMAKE_SOURCE_DIR}/docs/reference/mainpage.md")
set(DOXYGEN_LAYOUT "${DOXYGEN_DIR}/layout.xml")
set(DOXYGEN_EXAMPLE_PATH "${CMAKE_SOURCE_DIR}")
set(DOXYGEN_IMAGE_PATH "${CMAKE_SOURCE_DIR}/docs/developer/architecture/diagrams/png")
set(DOXYGEN_OUTPUT_DIR "${CMAKE_BINARY_DIR}/docs")
file(MAKE_DIRECTORY "${DOXYGEN_OUTPUT_DIR}")
set(DOXYGEN_CONFIG "${CMAKE_BINARY_DIR}/docs/doxygen/Doxyfile")

configure_file(${DOXYGEN_TEMPLATE} ${DOXYGEN_CONFIG} @ONLY)

find_package(Doxygen)
if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen not found")
endif()

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

add_custom_target(
    generate_project_tree_json
    COMMAND ${CMAKE_COMMAND}
        -DSOURCE_DIR=${CMAKE_SOURCE_DIR}
        -DPROJECT_TREE_JSON_FILE=${PROJECT_TREE_JSON_FILE}
        -DREPO_WEB_URL="${REPO_WEB_URL}"
        -DREPO_REF="${GIT_REF}"
        -P ${DOXYGEN_DIR}/scripts/generate_project_tree_json.cmake
        COMMENT "Generating project tree JSON in ${PROJECT_TREE_JSON_FILE}"
)

#------------------------------------------------------------------------------
# PROJECT TREE DOCUMENTATION PAGE CONFIGURATION
#------------------------------------------------------------------------------

# project_tree.dox generation
add_custom_target(
    generate_project_tree_dox
    COMMAND ${CMAKE_COMMAND}
        -DSOURCE_DIR=${CMAKE_SOURCE_DIR}
        -DBUILT_DIR=${CMAKE_BINARY_DIR}
        -DPROJECT_TREE_JSON_FILE=${PROJECT_TREE_JSON_FILE}
        -P ${DOXYGEN_DIR}/scripts/generate_project_tree_dox.cmake
        COMMENT "Generating doc page with the project tree in dox format in ${DYNAMIC_DOC_PAGES_DIR}/project_tree.dox"
)
add_dependencies(
    generate_project_tree_dox
    generate_project_tree_json
)

#------------------------------------------------------------------------------
# DOXYGEN STATIC DOCUMENTATION GENERATION
#------------------------------------------------------------------------------

# Create a custom target to generate API documentation using Doxygen.
add_custom_target(docs
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating static documentation pages with Doxygen"
    VERBATIM
)
add_dependencies(docs generate_project_tree_dox)

# copy doc in sources directory
add_custom_target(
    copy_doc_into_sources_directory
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_BINARY_DIR}/docs/html
    ${CMAKE_SOURCE_DIR}/site
    COMMENT "Copying documentation from build to source directory."
)
add_dependencies(copy_doc_into_sources_directory docs)

# open documentation in a new web browser window
add_custom_target(open_doc
    COMMAND ${CMAKE_COMMAND} -E echo "Opening documentation via Firefox..."
    COMMAND $<TARGET_FILE:open_doc_executable> "${CMAKE_BINARY_DIR}/docs/html/index.html"
    DEPENDS docs
    COMMENT "Opening documentation via Firefox."
)

# update documentation on droplet
add_custom_target(update_docs_on_droplet
    COMMAND ${CMAKE_COMMAND} -E echo "Updating documentation on droplet..."
    COMMAND $<TARGET_FILE:update_docs_on_droplet_executable> "${CMAKE_BINARY_DIR}"
    DEPENDS docs
    COMMENT "Updating documentation on droplet."
)
