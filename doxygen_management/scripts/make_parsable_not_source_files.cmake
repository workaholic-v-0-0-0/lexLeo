if (EXISTS ${OUTPUT_DIR})
    file(REMOVE_RECURSE ${OUTPUT_DIR})
endif ()

set(DIRS_TO_BE_PARSED # to avoid parsing unexpected artifact
    "${SOURCE_DIR}/doxygen_management"
    "${SOURCE_DIR}/src"
    "${SOURCE_DIR}/tests"
)
set(FILES "${SOURCE_DIR}")
foreach (DIR IN LISTS DIRS_TO_BE_PARSED)
    file(
        GLOB_RECURSE
        L
        LIST_DIRECTORIES true
        ${DIR}/*
    )
    list(
        APPEND
        FILES
        ${DIR}
        ${L}
    )
endforeach ()
list(
    APPEND
    FILES
    "${SOURCE_DIR}/CMakeLists.txt"
    "${SOURCE_DIR}/LICENSE"
    "${SOURCE_DIR}/README.md"
    "${SOURCE_DIR}/Doxyfile" # a build artifact
    "${SOURCE_DIR}/Doxyfile_to_fetch_links" # a build artifact
    "${SOURCE_DIR}"
)

#[[file(
    GLOB_RECURSE
    FILES
    LIST_DIRECTORIES true
    ${SOURCE_DIR}/*
)

list(
    APPEND
    FILES
    ${SOURCE_DIR}
)]]

message(
    STATUS
    "FILES: ${FILES}"
)

#[[list(
    FILTER
    FILES
    EXCLUDE
    REGEX
    "\\.git.*|\\.idea.*|.*/docs(/.*)?|.*/build(/.*)?|.*/\
cmake-build-debug(/.*)?|.*/Testing(/.*)?|.*/doc_to_fetch_links_tmp(/.*)?"
    )]]

list(
    FILTER
    FILES
    EXCLUDE
    REGEX
    ".*/\\.gitkeep"
)

message(
    STATUS
    "FILES: ${FILES}"
)

foreach(FILE IN LISTS FILES)
    message(STATUS "FILE: ${FILE}")
    file(
        RELATIVE_PATH
        RELATIVE_PATH_WITH_RESPECT_TO_OUTPUT_DIR
        "${SOURCE_DIR}/.." # to start with "lexLeo/"
        "${FILE}"
    )
    get_filename_component(BASE_NAME ${FILE} NAME)
    set(
        FILE_TO_BE_CREATED
        "${OUTPUT_DIR}/${RELATIVE_PATH_WITH_RESPECT_TO_OUTPUT_DIR}"
    )
    string(
        REPLACE
        "/"
        "_2"
        PAGE_IDENTIFIER
        ${RELATIVE_PATH_WITH_RESPECT_TO_OUTPUT_DIR}
    )
    string(
        REPLACE # to afford Doxygen parsing
        "."
        "_8"
        PAGE_IDENTIFIER
        ${PAGE_IDENTIFIER}
    )
    if (IS_DIRECTORY ${FILE})
        set(
            PAGE_TITLE
            "Directory ${RELATIVE_PATH_WITH_RESPECT_TO_OUTPUT_DIR}"
        )
        if("${PAGE_TITLE}" STREQUAL "Directory lexLeo")
            set(
                PAGE_TITLE
                "All the files of the project"
            )
        endif ()
        file(MAKE_DIRECTORY ${FILE_TO_BE_CREATED})
        file(GLOB FILES_IN_DIR LIST_DIRECTORIES true "${FILE}/*")
        list(
            FILTER
            FILES_IN_DIR
            EXCLUDE
            REGEX
            "\\.git.*|\\.idea.*|.*/docs(/.*)?|.*/build(/.*)?|.*/\
cmake-build-debug(/.*)?|.*/Testing(/.*)?|.*/doc_to_fetch_links_tmp(/.*)?"
        )
        file(
            WRITE
            ${FILE_TO_BE_CREATED}.dox
            "/**\n\
 * @page ${PAGE_IDENTIFIER} ${PAGE_TITLE}\n\
 * @htmlonly\n\
   <!-- PageID: ${PAGE_IDENTIFIER} -->\n\
 * @endhtmlonly\n"
        )
        foreach (FILE_IN_DIR ${FILES_IN_DIR})
            get_filename_component(FILE_IN_DIR_BASE_NAME ${FILE_IN_DIR} NAME)
            string(
                REPLACE
                "."
                "_8"
                FILE_IN_DIR_IDENTIFIER
                ${FILE_IN_DIR_BASE_NAME}
            )
            set(
                FILE_IN_DIR_IDENTIFIER
                "${PAGE_IDENTIFIER}_2${FILE_IN_DIR_INDENTIFIER}\
${FILE_IN_DIR_IDENTIFIER}"
            )
            file(
                APPEND
                ${FILE_TO_BE_CREATED}.dox
                "\n * - @subpage ${FILE_IN_DIR_IDENTIFIER}"
            )
        endforeach ()
        file(APPEND ${FILE_TO_BE_CREATED}.dox "\n */\n")
    else ()
        file(
            WRITE
            ${FILE_TO_BE_CREATED}.dox
            "/**\n\
 * @page ${PAGE_IDENTIFIER} ${RELATIVE_PATH_WITH_RESPECT_TO_OUTPUT_DIR}\n\
 * @htmlonly\n\
   <!-- PageID: ${PAGE_IDENTIFIER} -->\n\
 * @endhtmlonly\n\
 * @verbinclude ${FILE}\n\
 */\n"
        )
    endif ()
endforeach()