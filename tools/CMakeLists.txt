# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_subdirectory(osal_tools)

add_executable(
    open_doc_executable
    "open_doc.c"
)

target_link_libraries(
    open_doc_executable
    PRIVATE
        osal_tools
)

add_executable(
    update_docs_on_droplet_executable
    "update_docs_on_droplet.c"
)

target_link_libraries(
    update_docs_on_droplet_executable
    PRIVATE
        osal_process
        osal_stdio
)
