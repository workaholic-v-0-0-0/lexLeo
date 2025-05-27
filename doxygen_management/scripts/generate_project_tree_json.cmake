file(
    READ
    ${DOX_TO_HTML_MAP_JSON_FILE}
    DOX_TO_HTML_MAP_JSON
)

set(I "    ")
set(I2 "${I}${I}")
set(I3 "${I2}${I}")
set(I4 "${I3}${I}")

function(getLink ID OUTPUT)
    string(
        REGEX
        MATCH
        "\"key\": ${ID},\n${I2}\"value\": [^ \n\t]+"
        LINK
        ${DOX_TO_HTML_MAP_JSON}
    )
    if (LINK)
        string(REPLACE "\"key\": ${ID},\n${I2}\"value\": " "" LINK ${LINK})
        set(${OUTPUT} ${LINK} PARENT_SCOPE)
    else ()
        set(${OUTPUT} "" PARENT_SCOPE)
    endif ()
endfunction()

set(
    EXCLUDE_DIRS
    "build"
    "doc_to_fetch_links_tmp"
    "docs"
    "cmake-build-debug"
    ".idea"
    ".git"
    "Testing"
)

file(
    GLOB_RECURSE
    PROJECT_FILES
    LIST_DIRECTORIES true
    "${SOURCE_DIR}/*"
)
list(APPEND PROJECT_FILES "${SOURCE_DIR}")

# Filtrer les fichiers exclus dans une nouvelle liste
set(FILTERED_PROJECT_FILES "")
foreach(FILE ${PROJECT_FILES})
    set(EXCLUDED FALSE)
    foreach(EXCLUDE_DIR ${EXCLUDE_DIRS})
        if(FILE MATCHES "${SOURCE_DIR}/${EXCLUDE_DIR}")
            set(EXCLUDED TRUE)
        endif()
    endforeach()
    if(NOT EXCLUDED)
        list(APPEND FILTERED_PROJECT_FILES "${FILE}")
    endif()
endforeach()

# write project_tree.json
file(WRITE "${PROJECT_TREE_JSON_FILE}" "[\n")

set(FIRST_ELEMENT TRUE)

foreach(FILE ${FILTERED_PROJECT_FILES})
    message(STATUS "FILE: ${FILE}")

    # id
    file(RELATIVE_PATH REL_PATH "${SOURCE_DIR}/.." "${FILE}")
    string(REPLACE "/" "_2" ID "${REL_PATH}")
    string(REPLACE "." "_8" ID "${ID}")
    if (NOT ID)
        set(ID "lexLeo")
    endif ()

    # parent
    get_filename_component(PARENT_DIR "${REL_PATH}" DIRECTORY)
    if("${PARENT_DIR}" STREQUAL "")
        set(PARENT_ID "#")
    else()
        string(REPLACE "/" "_2" PARENT_ID "${PARENT_DIR}")
        string(REPLACE "." "_8" PARENT_ID "${PARENT_ID}")
    endif()

    # text
    get_filename_component(BASE_NAME "${FILE}" NAME)

    # type
    if(IS_DIRECTORY "${FILE}")
        set(TYPE "folder")
    else()
        set(TYPE "file")
    endif()

    # a_attr
    getLink("\"${ID}\"" LINK)
    message(STATUS "LINK: ${LINK}")

    # Ajouter l'entrée JSON
    if (NOT FIRST_ELEMENT)
        file(APPEND ${PROJECT_TREE_JSON_FILE} ",\n")
    else()
        set(FIRST_ELEMENT FALSE)
    endif()
    file(
        APPEND
        "${PROJECT_TREE_JSON_FILE}"
        "${I2}{\n\
${I3}\"id\": \"${ID}\",\n\
${I3}\"parent\": \"${PARENT_ID}\",\n\
${I3}\"text\": \"${BASE_NAME}\",\n\
${I3}\"type\": \"${TYPE}\""
    )
    if(LINK)
        file(
            APPEND
            "${PROJECT_TREE_JSON_FILE}"
            ",\n${I3}\"a_attr\": {\n\
${I4}\"href\": ${LINK}\n${I4}}\n"
        )
    else ()
        file(APPEND "${PROJECT_TREE_JSON_FILE}" "\n")
    endif()

    file(APPEND "${PROJECT_TREE_JSON_FILE}" "${I2}}")

endforeach()

file(APPEND "${PROJECT_TREE_JSON_FILE}" "\n]")
