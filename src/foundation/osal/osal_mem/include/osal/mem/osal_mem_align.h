#ifndef LEXLEO_OSAL_MEM_ALIGN_H
#define LEXLEO_OSAL_MEM_ALIGN_H

#include "policy/lexleo_cstd_types.h"

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
	#include <stdalign.h>
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

/* Return the smallest multiple of `a` greater than or equal to `x`.
 * `a` must be a power of 2 (an alignment value).
 */
#ifndef OSAL_ALIGN_UP
	#define OSAL_ALIGN_UP(x, a) (((x) + (a) - 1) & ~((a) - 1))
#endif

#endif // LEXLEO_OSAL_MEM_ALIGN_H
