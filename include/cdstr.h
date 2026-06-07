#ifndef CDSTR_H
#define CDSTR_H

#define DEFAULT_CAPACITY 16

/* ============================================================================

    VALID STRING:

    s != NULL

    Either:

        ptr == NULL
        len == 0
        cap == 0

    Or:

        ptr != NULL
        cap >= len + 1
        ptr[len] == '\0'

    EMPTY STIRNG:


========================================================================== */

#include <stdbool.h>
#include <stdlib.h>

typedef enum {
  SUCCESS = 0,
  ERR_ALLOC_FAILED,
  ERR_NULL_POINTER,
  ERR_NULL_ARGUMENT,
  ERR_STRING_INIT_FAILED,
  ERR_INDEX_OUT_OF_LEN,
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

// implement later
static size_t cdstr_memmove();

// implemented
Err cdstr_init(String *s, const char *cstr);
Err cdstr_destroy(String *s);
Err cdstr_clear(String *s);

Err cdstr_append(String *s, const char *slice);
Err cdstr_insert(String *s, size_t index, const char *slice);

size_t cdstr_len(const String *s);
size_t cdstr_cap(const String *s);

Err cdstr_copy(String *s_dest, const String *s_src);
Err cdstr_reserve(String *s, size_t count);

bool cdstr_empty(const String *s);

Err cdstr_shrink(String *s);

// have to implement
Err cdstr_append_n(String *s, const char *slice, size_t len);

Err cdstr_assign_n(String *s, const char *slice, size_t len);

#endif /* CDSTR_H */
