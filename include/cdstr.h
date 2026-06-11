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
========================================================================== */

#include <stdbool.h>
#include <stdlib.h>

typedef enum {
  SUCCESS = 0,
  ERR_ALLOC_FAILED,
  ERR_NULL_POINTER,
  ERR_OUT_OF_MEMORY,
  ERR_NULL_ARGUMENT,
  ERR_STRING_INIT_FAILED,
  ERR_INDEX_OUT_OF_LEN,
} Err;

typedef struct {
  char *ptr;
  size_t len;
  size_t cap;
} String;

// Helper functions
static inline size_t min_size(size_t a, size_t b) { return a < b ? a : b; }

size_t cdstr_strlen(const char *string);

static size_t cdstr_memcpy(char *restrict to, const void *restrict from,
                           size_t bytes_num);

static size_t cdstr_memmove(char *to, const void *from, size_t bytes_num);

// implement later
static size_t cdstr_memmove();

// implemented
Err cdstr_init(String *s, const char *cstr);
Err cdstr_destroy(String *s);
Err cdstr_clear(String *s);

Err cdstr_append(String *s, const char *slice);
Err cdstr_append_n(String *s, const char *slice, size_t len);

Err cdstr_insert(String *s, size_t index, const char *slice);

size_t cdstr_len(const String *s);
size_t cdstr_cap(const String *s);

Err cdstr_copy(String *s_dest, const String *s_src);
Err cdstr_reserve(String *s, size_t count);

bool cdstr_empty(const String *s);

Err cdstr_shrink(String *s);

// have to implement

char *cdstr_c_str(const String *s);

Err cdstr_at(const String *s); // bounds-checked index access

Err cdstr_cmp(const String *a, const String *b);

Err cdstr_find(const String *s, const char *substring);

// trim ltrim rtrim
Err cdstr_trim(String *s);
Err cdstr_ltrim(String *s);
Err cdstr_rtrim(String *s);

Err cdstr_tolower(String *s);
Err cdstr_toupper(String *s);

#endif /* CDSTR_H */
