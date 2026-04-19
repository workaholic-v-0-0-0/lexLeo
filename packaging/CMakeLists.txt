# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

if (WIN32)
    include(windows/windows_pack.cmake)
elseif (APPLE)
    include(macos/macos_pack.cmake)
elseif (UNIX)
    include(linux/linux_pack.cmake)
else ()
    message(FATAL_ERROR "Unsupported platform for packaging")
endif ()

set(CPACK_PACKAGE_NAME "lexleo")
set(CPACK_PACKAGE_VENDOR "Sylvain Labopin")
set(CPACK_PACKAGE_VERSION "0.1.0")
set(CPACK_PACKAGE_CONTACT "Sylvain Labopin")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LexLeo language command-line application")
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_BINARY_DIR}/dist")

include(CPack)
