// src/policy/include/policy/lexleo_panic.h

#ifndef LEXLEO_PANIC_H
#define LEXLEO_PANIC_H

#ifdef __cplusplus
extern "C" {
#endif

void lexleo_panic(const char *msg) __attribute__((noreturn));
void lexleo_panic_oom(void) __attribute__((noreturn));

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_PANIC_H