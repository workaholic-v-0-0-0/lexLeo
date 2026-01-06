// src/policy/scr/lexleo_panic.c

#include "lexleo_panic.h"
#include "lexleo_assert.h"

#include <stdio.h>
#include <stdlib.h>

void lexleo_panic(const char *msg) {
	// todo logging
	fprintf(stderr, "LexLeo panic: %s\n", msg ? msg : "(no message)");
	abort();
}

void lexleo_panic_oom(void) {
	lexleo_panic("out of memory");
}
