// src/logger/tests/fake_time.c

#include "fake_time.h"
#include <time.h>

static time_t fake_time = 0;

void set_fake_time(time_t time) {
    fake_time = time;
}

// overload libc definition of time
time_t time(time_t *t) {
    if (t) *t = fake_time;
    return fake_time;
}
