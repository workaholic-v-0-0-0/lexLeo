/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file main.c
 * @ingroup cli_group
 * @brief Process entry point for the `lexleo` command-line interface.
 *
 * @details
 * This translation unit defines the program entry point and delegates
 * execution to the CLI module through `cli_run()`.
 */

#include "cli.h"

int main() {
	return cli_main();
}
