# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/lexleo.conf.in"
    "${CMAKE_CURRENT_BINARY_DIR}/lexleo.conf"
    @ONLY
)

install(
    TARGETS lexleo
    RUNTIME DESTINATION bin
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/lexleo.conf"
    DESTINATION etc/lexleo
)

install(
    FILES "${CMAKE_CURRENT_LIST_DIR}/README.txt"
    DESTINATION share/doc/lexleo
)

install(
    FILES "${CMAKE_CURRENT_LIST_DIR}/lexleo.1"
    DESTINATION share/man/man1
)

install(
    FILES "${CMAKE_SOURCE_DIR}/LICENSE"
    DESTINATION share/doc/lexleo
    RENAME LICENSE.txt
)

set(CPACK_GENERATOR "productbuild")

set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")

set(CPACK_PRODUCTBUILD_IDENTIFIER "org.sylvainlabopin.lexleo")
set(CPACK_PRODUCTBUILD_PACKAGE_NAME "LexLeo")

set(CPACK_PACKAGE_FILE_NAME "lexleo-${CPACK_PACKAGE_VERSION}-macos")
