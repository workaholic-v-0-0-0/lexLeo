file(GLOB_RECURSE HTML_FILES "${HTML_DIR}/*.html")

set(I "    ")
set(I2 "${I}${I}")

file(
    WRITE
    ${DOX_TO_HTML_MAP_JSON_FILE}
    "[\n"
)

set(FIRST_ELEMENT TRUE)

foreach (HTML_FILE ${HTML_FILES})
    file(READ ${HTML_FILE} HTML_FILE_CONTENT)
    set(IDENTIFIER "")
    string(
        REGEX
        MATCH
        "<!-- PageID: [^ ]+ -->"
        FOUND
        "${HTML_FILE_CONTENT}"
    )
    if (FOUND)
        string(REPLACE "<!-- PageID: " "" IDENTIFIER ${FOUND})
        string(REPLACE " -->" "" IDENTIFIER ${IDENTIFIER})
        if (NOT FIRST_ELEMENT)
            file(APPEND ${DOX_TO_HTML_MAP_JSON_FILE} ",\n")
        else()
            set(FIRST_ELEMENT FALSE)
        endif()
        file(
            RELATIVE_PATH
            LINK
            "${HTML_DIR}/html"
            ${HTML_FILE}
        )
        file(
            APPEND
            ${DOX_TO_HTML_MAP_JSON_FILE}
            "${I}{\n\
${I2}\"key\": \"${IDENTIFIER}\",\n\
${I2}\"value\": \"${LINK}\"\n${I}}"
        )
    endif ()
endforeach ()

file(
    APPEND
    ${DOX_TO_HTML_MAP_JSON_FILE}
    "\n]\n"
)
