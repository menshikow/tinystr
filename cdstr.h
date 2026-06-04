#ifndef CDSTR_H
#define CDSTR_H

#define DEFAULT_CAPACITY 16

/* ============================================================================
  VALID STRING INVARIANT:
  s != NULL
  ptr != NULL
  cap >= len + 1
  len >= 0
  ptr[len] == '\0'

  INVALID STATE:
  does not exist
 * ========================================================================== */

#include <stdbool.h>
#include <stdlib.h>

typedef enum {
  SUCCESS = 0,
  ERR_ALLOC_FAILED,
  ERR_NULL_ARGUMENT,
  ERR_STRING_INIT_FAILED,
  ERR,
} Err;

typedef struct {
  char *ptr;
  size_t len;
  size_t cap;
} String;

static inline size_t min_size(size_t a, size_t b) { return a < b ? a : b; }

size_t cdstr_strlen(const char *string);

static size_t cdstr_memcpy(char *restrict dest, const void *restrict src,
                           size_t count);

Err cdstr_init(String *s, const char *cstr);
Err cdstr_destroy(String *s);
Err cdstr_clear(String *s);

Err cdstr_append(String *s, const char *slice);
Err cdstr_append_n(String *s, const char *slice, size_t len);

Err cdstr_assign(String *s, const char *slice);
Err cdstr_assign_n(String *s, const char *slice, size_t len);

Err cdstr_len(const String *s);
Err cdstr_cap(const String *s);
bool cdstr_empty(const String *s);

Err cdstr_copy(String *s_dest, const String *s_src);
Err cdstr_reserve(String *s, size_t count);
Err cdstr_shrink(String *s);

#endif /* CDSTR_H */
