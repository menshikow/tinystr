#include "../include/cdstr.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t cdstr_strlen(const char *string_start) {
  if (string_start == NULL) {
    return ERR_NULL_ARGUMENT;
  }
  const char *string_end = string_start;
  while (*string_end != '\0') {
    ++string_end;
  }

  return string_end - string_start;
}

size_t cdstr_memcpy(char *restrict dest, const void *restrict src,
                    size_t count) {

  size_t bytes_to_copy = count;
  size_t copied = bytes_to_copy;

  const unsigned char *s = src;
  while (bytes_to_copy--) {
    *dest++ = *s++;
  }

  return copied;
}

Err cdstr_init(String *s_out, const char *s_in) {
  if (s_out == NULL) {
    return ERR_NULL_ARGUMENT;
  }
  if (s_in == NULL) {
    s_in = "";
  }

  size_t len = cdstr_strlen(s_in);
  size_t cap = len + 1 > DEFAULT_CAPACITY ? len + 1 : DEFAULT_CAPACITY;

  char *buf = (char *)malloc(cap);

  if (buf == NULL) {
    return ERR_ALLOC_FAILED;
  }

  cdstr_memcpy(buf, s_in, len + 1);

  s_out->ptr = buf;
  s_out->len = len;
  s_out->cap = cap;

  return SUCCESS;
}

Err cdstr_destroy(String *s) {
  free(s->ptr);

  *s = (String){0};

  return SUCCESS;
}

Err cdstr_clear(String *s) {
  if (s == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  s->ptr[0] = '\0';
  s->len = 0;

  return SUCCESS;
}

Err str_append(String *s, const char *slice) {
  if (s == NULL || slice == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (s->ptr == NULL) {
    return (cdstr_init(s, slice) == SUCCESS) ? SUCCESS : ERR_STRING_INIT_FAILED;
  }

  size_t slice_len = cdstr_strlen(slice);

  size_t new_len = s->len + slice_len;

  if (s->cap < new_len + 1) {
    size_t tmp_cap = new_len * 2;

    char *tmp_ptr = realloc(s->ptr, tmp_cap);
    if (tmp_ptr == NULL) {
      return ERR_ALLOC_FAILED;
    }

    s->ptr = tmp_ptr;
    s->cap = tmp_cap;
  }

  cdstr_memcpy(s->ptr + s->len, slice, slice_len + 1);

  s->len = new_len;

  return SUCCESS;
}

Err cdstr_insert(String *s, size_t index, const char *slice) {
  if (s == NULL || slice == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (index > s->len) {
    return ERR_INDEX_OUT_OF_LEN;
  }

  size_t slice_len = cdstr_strlen(slice);
  if (slice_len == 0) {
    return SUCCESS;
  }

  size_t new_len = s->len + slice_len;

  if (new_len + 1 > s->cap) {
    size_t tmp_cap = (new_len + 1) * 2;

    char *tmp_ptr = realloc(s->ptr, tmp_cap);
    if (tmp_ptr == NULL) {
      return ERR_ALLOC_FAILED;
    }

    s->ptr = tmp_ptr;
    s->cap = tmp_cap;
  }

  if (index < s->len) {
    memmove(s->ptr + index + slice_len, s->ptr + index, s->len - index);
  }

  cdstr_memcpy(s->ptr + index, slice, slice_len);

  s->len = new_len;
  s->ptr[s->len] = '\0';

  return SUCCESS;
}

// think about 1. Self-clone, 2. What if src is destroyed?, 3. What if dest is
// in the destroyed/empty state?

Err cdstr_copy(String *dest, String const *src) {
  if (src == NULL || dest == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (dest->cap < src->len + 1) {
    size_t tmp_cap = src->len * 2;
    char *tmp_ptr = realloc(dest->ptr, tmp_cap);

    if (tmp_ptr == NULL) {
      return ERR_ALLOC_FAILED;
    }

    dest->ptr = tmp_ptr;
    dest->cap = tmp_cap;
    cdstr_memcpy(dest->ptr, src->ptr, src->len + 1);
    dest->len = src->len;

    return SUCCESS;
  }

  cdstr_memcpy(dest->ptr, src->ptr, src->len + 1);
  dest->len = src->len;

  return SUCCESS;
}

// overflow check
Err cdstr_reserve(String *s, size_t count) {
  if (s == NULL || s->ptr == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (count == 0) {
    return SUCCESS;
  }

  size_t new_cap = s->cap + count;

  char *tmp_ptr = realloc(s->ptr, new_cap);
  if (tmp_ptr == NULL) {
    return ERR_ALLOC_FAILED;
  }

  s->ptr = tmp_ptr;
  s->cap = new_cap;

  return SUCCESS;
}

Err cdstr_shrink(String *s) {
  if (s == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (s->cap == s->len + 1) {
    return SUCCESS;
  }

  size_t new_cap = s->len + 1;

  char *tmp_ptr = realloc(s->ptr, new_cap);

  if (tmp_ptr == NULL) {
    return ERR_ALLOC_FAILED;
  }

  s->ptr = tmp_ptr;
  s->cap = new_cap;

  return SUCCESS;
}

size_t cdstr_len(const String *s) {
  if (s == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  return s->len;
}

size_t cdstr_cap(const String *s) {
  if (s == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  return s->cap;
}

bool cdstr_empty(const String *s) {
  if (s == NULL) {
    return true;
  }

  return (s->len == 0);
}
