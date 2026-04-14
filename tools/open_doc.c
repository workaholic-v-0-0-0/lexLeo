/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

#include "tools/osal/osal_tools.h"

int main(int argc, char **argv) {
    if (argc != 2) return 1;
    osal_open_in_web_browser(argv[1]);
    return 0;
}
