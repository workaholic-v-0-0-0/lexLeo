set(I "    ")
set(I2 "${I}${I}")
set(I3 "${I2}${I}")
set(I4 "${I3}${I}")
set(
    EXCLUDE_DIRS
        "build"
        "site"
        "cmake-build-debug"
        ".idea"
        ".git"
        "Testing"
)

function(json_escape INPUT OUTPUT)
    set(s "${INPUT}")
    string(REPLACE "\\" "\\\\" s "${s}")
    string(REPLACE "\"" "\\\"" s "${s}")
    string(REPLACE "\n" "\\n" s "${s}")
    string(REPLACE "\r" "\\r" s "${s}")
    string(REPLACE "\t" "\\t" s "${s}")
    set(${OUTPUT} "${s}" PARENT_SCOPE)
endfunction()

file(GLOB_RECURSE PROJECT_FILES LIST_DIRECTORIES true "${SOURCE_DIR}/*")
list(APPEND PROJECT_FILES "${SOURCE_DIR}")

# Filter out excluded directories and files, storing the result in a new list
set(FILTERED_PROJECT_FILES "")
foreach (FILE ${PROJECT_FILES})
    set(EXCLUDED FALSE)
    foreach (EXCLUDE_DIR ${EXCLUDE_DIRS})
        if (FILE MATCHES "^${SOURCE_DIR}/${EXCLUDE_DIR}(/|$)")
            set(EXCLUDED TRUE)
            break()
        endif ()
    endforeach ()
    if (NOT EXCLUDED)
        list(APPEND FILTERED_PROJECT_FILES "${FILE}")
    endif ()
endforeach ()

# write project_tree.json
file(WRITE "${PROJECT_TREE_JSON_FILE}" "[\n")

set(FIRST_ELEMENT TRUE)

foreach (FILE ${FILTERED_PROJECT_FILES})
    # id
    file(RELATIVE_PATH REL_PATH "${SOURCE_DIR}/.." "${FILE}")
    string(REPLACE "/" "_2" ID "${REL_PATH}")
    string(REPLACE "." "_8" ID "${ID}")
    if (NOT ID)
        set(ID "lexLeo")
    endif ()

    # parent
    get_filename_component(PARENT_DIR "${REL_PATH}" DIRECTORY)
    if ("${PARENT_DIR}" STREQUAL "")
        set(PARENT_ID "#")
    else ()
        string(REPLACE "/" "_2" PARENT_ID "${PARENT_DIR}")
        string(REPLACE "." "_8" PARENT_ID "${PARENT_ID}")
    endif ()

    # text
    get_filename_component(BASE_NAME "${FILE}" NAME)
    json_escape("${BASE_NAME}" BASE_NAME_JSON)

    # type
    if (IS_DIRECTORY "${FILE}")
        set(TYPE "folder")
    else ()
        set(TYPE "file")
    endif ()

    # a_attr (GitHub links)
    set(LINK "")
    if (NOT IS_DIRECTORY "${FILE}" AND REPO_WEB_URL AND REPO_REF)
        file(RELATIVE_PATH REPO_REL_PATH "${SOURCE_DIR}" "${FILE}")
        string(REPLACE "\\" "/" REPO_REL_PATH "${REPO_REL_PATH}")
        set(LINK "\"${REPO_WEB_URL}/blob/${REPO_REF}/${REPO_REL_PATH}\"")
    endif ()

    if (NOT FIRST_ELEMENT)
        file(APPEND ${PROJECT_TREE_JSON_FILE} ",\n")
    else ()
        set(FIRST_ELEMENT FALSE)
    endif ()
    file(
        APPEND
        "${PROJECT_TREE_JSON_FILE}"
        "${I2}{\n\
${I3}\"id\": \"${ID}\",\n\
${I3}\"parent\": \"${PARENT_ID}\",\n\
${I3}\"text\": \"${BASE_NAME_JSON}\",\n\
${I3}\"type\": \"${TYPE}\""
    )
    if (LINK)
        file(
            APPEND
            "${PROJECT_TREE_JSON_FILE}"
            ",\n${I3}\"a_attr\": {\n\
${I4}\"href\": ${LINK}\n${I4}}\n"
        )
    else ()
        file(APPEND "${PROJECT_TREE_JSON_FILE}" "\n")
    endif ()

    file(APPEND "${PROJECT_TREE_JSON_FILE}" "${I2}}")

endforeach ()

file(APPEND "${PROJECT_TREE_JSON_FILE}" "\n]")
