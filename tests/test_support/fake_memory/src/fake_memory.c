// tests/test_support/fake_memory/src/fake_memory.c

#include "fake_memory.h"

#include "osal.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <stdio.h>
static void log_alloc(const char *what, size_t idx) {
  fprintf(stderr, "FAKE_ALLOC #%zu: %s\n", idx, what);
}

#ifndef TEST_ARENA_SIZE
  #define TEST_ARENA_SIZE (64u * 1048576u)
#endif

OSAL_ALIGNED_MAX static uint8_t g_arena[TEST_ARENA_SIZE];

static size_t g_off;
static size_t g_alloc_count; // counts *API-level* allocation calls (malloc/calloc/realloc/strdup)
static size_t g_in_use; // bytes currently allocated (aligned payload)

#define FAKE_MAGIC_ALLOC 0xC0DEFACEu
#define FAKE_MAGIC_FREE  0xDEADFA11u

static size_t g_invalid_free_count;
static size_t g_double_free_count;

#define MAX_FAILS 64
static size_t g_fail_points[MAX_FAILS];
static size_t g_fail_points_len;

typedef struct {
  uint32_t magic;
  uint32_t _pad;
  size_t   size_aligned;
  size_t   size_requested;
} fake_hdr;

// -----------------------------------------------------------------------------
// Failure scheduling (1-based call indices)
// -----------------------------------------------------------------------------

static bool should_fail_now(size_t cur) {
  for (size_t i = 0; i < g_fail_points_len; ++i) {
    if (g_fail_points[i] == cur) {
      // remove by swap-with-last
      g_fail_points[i] = g_fail_points[--g_fail_points_len];
      return true;
    }
  }
  return false;
}

void fake_memory_fail_on_calls(size_t n, const size_t *idxs) {
  if (n > MAX_FAILS) n = MAX_FAILS;
  for (size_t i = 0; i < n; ++i) g_fail_points[i] = idxs[i];
  g_fail_points_len = n;
  g_alloc_count = 0;
}

void fake_memory_fail_only_on_call(size_t n) {
  fake_memory_fail_on_calls(1, &n);
}

void fake_memory_fail_on_all_call(void) {
  g_fail_points_len = MAX_FAILS;
  for (size_t i = 0; i < MAX_FAILS; ++i) g_fail_points[i] = i + 1; // 1..MAX_FAILS
  g_alloc_count = 0;
}

void fake_memory_fail_since(size_t n) {
  g_fail_points_len = MAX_FAILS;
  for (size_t i = 0; i < MAX_FAILS; ++i) g_fail_points[i] = n + i;
  g_alloc_count = 0;
}

// -----------------------------------------------------------------------------
// Reset / checks
// -----------------------------------------------------------------------------

void fake_memory_reset(void) {
  g_off = 0;
  g_alloc_count = 0;
  g_in_use = 0;
  g_invalid_free_count = 0;
  g_double_free_count = 0;
  memset(g_arena, 0, sizeof(g_arena));
  memset(g_fail_points, 0, sizeof(g_fail_points));
  g_fail_points_len = 0;
}

bool fake_memory_no_leak(void) {
  return g_in_use == 0;
}

bool fake_memory_no_invalid_free(void) {
  return g_invalid_free_count == 0;
}

bool fake_memory_no_double_free(void) {
  return g_double_free_count == 0;
}

// -----------------------------------------------------------------------------
// Internal arena allocator (NO failure scheduling, NO alloc_count increment)
// -----------------------------------------------------------------------------

static void *raw_alloc(size_t size) {
  if (size == 0) size = 1; // simplify

  const size_t a = OSAL_ALIGNOF_MAX;
  const size_t hdr_size = OSAL_ALIGN_UP(sizeof(fake_hdr), a);
  const size_t payload  = OSAL_ALIGN_UP(size, a);

  if (hdr_size + payload > (TEST_ARENA_SIZE - g_off)) return NULL;

  fake_hdr *h = (fake_hdr *)(g_arena + g_off);
  h->magic = FAKE_MAGIC_ALLOC;
  h->_pad = 0;
  h->size_aligned = payload;
  h->size_requested = size;

  g_off += hdr_size + payload;
  g_in_use += payload;

  return (void *)((uint8_t *)h + hdr_size);
}

// -----------------------------------------------------------------------------
// Public fake OSAL API
//  - Exactly ONE ++g_alloc_count per API call.
//  - Failure scheduling applies at the API boundary only.
// -----------------------------------------------------------------------------

void *fake_malloc(size_t size) {
  /*
  size_t idx = ++g_alloc_count;
  log_alloc("malloc", idx);
  */
  if (should_fail_now(++g_alloc_count)) return NULL;
  return raw_alloc(size);
}

void *fake_calloc(size_t nmemb, size_t size) {
  /*
  size_t idx = ++g_alloc_count;
  log_alloc("malloc", idx);
  */
  if (should_fail_now(++g_alloc_count)) return NULL;

  // overflow guard
  if (nmemb != 0 && size > (SIZE_MAX / nmemb)) return NULL;

  const size_t total = nmemb * size;
  void *p = raw_alloc(total);
  if (!p) return NULL;

  memset(p, 0, total);
  return p;
}

void fake_free(void *ptr) {
  if (!ptr) return;

  const size_t a = OSAL_ALIGNOF_MAX;
  const size_t hdr_size = OSAL_ALIGN_UP(sizeof(fake_hdr), a);
  fake_hdr *h = (fake_hdr *)((uint8_t *)ptr - hdr_size);

  switch (h->magic) {
    case FAKE_MAGIC_ALLOC:
      h->magic = FAKE_MAGIC_FREE;
      if (g_in_use >= h->size_aligned) g_in_use -= h->size_aligned;
      else g_in_use = 0; // avoid underflow
      break;
    case FAKE_MAGIC_FREE:
      g_double_free_count++;
      break;
    default:
      g_invalid_free_count++;
      break;
  }
}

char *fake_strdup(const char *s) {
  /*
  size_t idx = ++g_alloc_count;
  log_alloc("malloc", idx);
  */
  if (should_fail_now(++g_alloc_count)) return NULL;
  if (!s) return NULL;

  const size_t n = strlen(s) + 1;
  char *p = (char *)raw_alloc(n);
  if (!p) return NULL;

  memcpy(p, s, n);
  return p;
}

void *fake_realloc(void *ptr, size_t size) {
  /*
  size_t idx = ++g_alloc_count;
  log_alloc("malloc", idx);
  */
  if (should_fail_now(++g_alloc_count)) return NULL;

  // realloc(NULL, size) => malloc(size)
  if (!ptr) {
    return raw_alloc(size);
  }

  // realloc(ptr, 0) => free(ptr) and return NULL
  if (size == 0) {
    fake_free(ptr);
    return NULL;
  }

  const size_t a = OSAL_ALIGNOF_MAX;
  const size_t hdr_size = OSAL_ALIGN_UP(sizeof(fake_hdr), a);
  fake_hdr *oldh = (fake_hdr *)((uint8_t *)ptr - hdr_size);

  if (oldh->magic != FAKE_MAGIC_ALLOC) {
    // signal invalid pointer (glibc would be UB; in tests we want a detectable failure)
    g_invalid_free_count++;
    return NULL;
  }

  const size_t old_req = oldh->size_requested;

  void *newp = raw_alloc(size);
  if (!newp) {
    // fail atomically: keep old block intact
    return NULL;
  }

  const size_t ncopy = (old_req < size) ? old_req : size;
  memcpy(newp, ptr, ncopy);

  fake_free(ptr);
  return newp;
}

void *fake_memcpy(void *dst, const void *src, size_t n) { return memcpy(dst, src, n); }
void *fake_memset(void *dst, int c, size_t n) { return memset(dst, c, n); }
