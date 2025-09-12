// tests/test_support/fake_memory/src/fake_memory.c

#include "fake_memory.h"

#include "osal.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

#ifndef TEST_ARENA_SIZE
  #define TEST_ARENA_SIZE (64 * 1024)
#endif

OSAL_ALIGNED_MAX static uint8_t g_arena[TEST_ARENA_SIZE];

static size_t g_off;
static size_t g_alloc_count;
static size_t g_in_use; // nb of octets currently allocated

#define FAKE_MAGIC_ALLOC 0xC0DEFACEu // for allocated block
#define FAKE_MAGIC_FREE  0xDEADFA11u // for freed block

static size_t g_invalid_free_count;
static size_t g_double_free_count;

#define MAX_FAILS 64
static size_t g_fail_points[MAX_FAILS];
static size_t g_fail_points_len;

static int cmp_size(const void *a, const void *b) {
  size_t lhs = *(const size_t *)a;
  size_t rhs = *(const size_t *)b;
  return (lhs > rhs) - (lhs < rhs);
}

void fake_memory_fail_on_calls(size_t n, const size_t *idxs) {
  if (n > MAX_FAILS) n = MAX_FAILS;
  for (size_t i = 0; i < n; ++i) g_fail_points[i] = idxs[i];
  qsort(g_fail_points, n, sizeof(size_t), cmp_size);
  g_fail_points_len = n;
  g_alloc_count = 0;
}

void fake_memory_fail_only_on_call(size_t n) {
  fake_memory_fail_on_calls(1, &n);
}

static bool should_fail_now(size_t cur) {
  for (size_t i = 0; i < g_fail_points_len; ++i) {
    if (g_fail_points[i] == cur) {
      g_fail_points[i] = g_fail_points[--g_fail_points_len];
      return true;
    }
  }
  return false;
}

typedef struct {
  uint32_t magic;
  uint32_t _pad;
  size_t size_aligned;
  size_t size_requested;
} fake_hdr;

void fake_memory_reset(void) {
  g_off = 0;
  g_alloc_count = 0;
  g_in_use = 0;
  g_invalid_free_count = 0;
  g_double_free_count = 0;
  memset(g_arena, 0, sizeof(g_arena));
  memset(g_fail_points, 0, MAX_FAILS * sizeof(size_t));
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

void *fake_malloc(size_t size) {
  if (should_fail_now(++g_alloc_count)) return NULL;
  if (size == 0) size = 1; // to simplify

  const size_t a = OSAL_ALIGNOF_MAX;
  const size_t hdr_size = OSAL_ALIGN_UP(sizeof(fake_hdr), a);
  size_t payload = OSAL_ALIGN_UP(size, a);

  if (hdr_size + payload > (TEST_ARENA_SIZE - g_off)) return NULL;

  fake_hdr *h = (fake_hdr *) (g_arena + g_off);
  h->magic = FAKE_MAGIC_ALLOC;
  h->_pad = 0;
  h->size_aligned = payload;
  h->size_requested = size;

  g_off += hdr_size + payload;
  g_in_use += payload;

  return (void *) ((uint8_t*) h + hdr_size);
}

void fake_free(void *ptr) {
  if (!ptr) return;
  const size_t a = OSAL_ALIGNOF_MAX;
  const size_t hdr_size = OSAL_ALIGN_UP(sizeof(fake_hdr), a);
  fake_hdr *h = (fake_hdr *) ((uint8_t*) ptr - hdr_size);

  switch (h->magic) {
    case FAKE_MAGIC_ALLOC:
      // valid free
      h->magic = FAKE_MAGIC_FREE;
      if (g_in_use >= h->size_aligned) g_in_use -= h->size_aligned;
      else g_in_use = 0; // prevent from underflow
      break;
    case FAKE_MAGIC_FREE:
      g_double_free_count++;
      break;
    default: // corrupted or not owned pointer
      g_invalid_free_count++;
  }
}

char *fake_strdup(const char *s) {
  if (!s) return NULL;
  size_t n = strlen(s) + 1;
  char *p = (char *) fake_malloc(n);
  if (!p) return NULL;
  memcpy(p, s, n);
  return p;
}
