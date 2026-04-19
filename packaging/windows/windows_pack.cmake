# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/lexleo.ini.in"
    "${CMAKE_CURRENT_BINARY_DIR}/lexleo.ini"
    @ONLY
)

install(
    TARGETS lexleo
    RUNTIME DESTINATION bin
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/lexleo.ini"
    DESTINATION config
)

install(
    FILES "${CMAKE_CURRENT_LIST_DIR}/README.txt"
    DESTINATION doc
)

install(
    FILES "${CMAKE_SOURCE_DIR}/LICENSE"
    DESTINATION doc
    RENAME LICENSE.txt
)

file(READ
    "${CMAKE_CURRENT_LIST_DIR}/nsis_post_install.nsh"
    LEXLEO_NSIS_POST_INSTALL_COMMANDS
)

set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "LexLeo")

set(CPACK_NSIS_DISPLAY_NAME "LexLeo")
set(CPACK_NSIS_PACKAGE_NAME "LexLeo")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS SetShellVarContext\ current\nCreateDirectory\ \\\"$LOCALAPPDATA\\\\LexLeo\\\\logs\\\")
set(CPACK_PACKAGE_EXECUTABLES "lexleo" "LexLeo")
set(CPACK_CREATE_DESKTOP_LINKS "lexleo")
