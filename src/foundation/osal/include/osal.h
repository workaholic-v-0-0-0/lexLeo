// src/foundation/osal/include/osal.h

#include <stdio.h>

#ifndef LEXLEO_OSAL_H
#define LEXLEO_OSAL_H

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #include <stdalign.h>
    #include <stddef.h>
    #define OSAL_ALIGNOF_MAX alignof(max_align_t)
    #define OSAL_ALIGNED_MAX _Alignas(max_align_t)
#else
    #if defined(_MSC_VER)
        #define OSAL_ALIGNOF_MAX 16
        #define OSAL_ALIGNED_MAX __declspec(align(16))
    #else
        #define OSAL_ALIGNOF_MAX 16
        #define OSAL_ALIGNED_MAX __attribute__((aligned(16)))
    #endif
#endif

// return the smallest multiple of `a` greater than or equal to `x` ;
// `a` must be a power of 2 (an alignment value)
#ifndef OSAL_ALIGN_UP
    #define OSAL_ALIGN_UP(x,a) ( ((x) + (a) - 1) & ~((a) - 1) )
#endif

#include <string.h>
#if defined(_MSC_VER)
    #define osal_strdup _strdup
#else
    #define osal_strdup strdup
#endif

/// Pause the thread for ms milliseconds.
void osal_sleep(int ms);

void osal_open_in_web_browser(const char *filepath);

FILE *osal_fmemopen_ro(const char *data, size_t len);

FILE *osal_open_memstream(char **out_buf, size_t *out_len);


#endif //LEXLEO_OSAL_H
