// src/osal/include/osal.h

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/// Pause the thread for ms milliseconds.
void osal_sleep(int ms);

void osal_open_in_web_browser(const char *filepath);

#ifdef __cplusplus
}
#endif
