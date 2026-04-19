# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/lexleo.conf.in"
    "${CMAKE_CURRENT_BINARY_DIR}/lexleo.conf"
    @ONLY
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
    RENAME copyright
)

set(CPACK_GENERATOR "DEB")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sylvain Labopin")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/workaholic-v-0-0-0/lexLeo")

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6")
