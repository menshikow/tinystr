# cdstr — Dynamic C String Library

## Objective

**cdstr** provides a safe, predictable, Redis-SDS-inspired dynamic string type (`String`) for C with explicit ownership and a documented memory invariant.

## Design goals

1. **Plain C** — no external dependencies beyond libc (`malloc`/`realloc`/`free`)
2. **Small API surface** — create, destroy, append, assign, copy, reserve, shrink, clear, query
3. **Explicit error handling** — every fallible operation returns an `Err` enum; no hidden aborts
4. **Documented invariant** — every valid `String` must satisfy `cap >= len + 1` and `ptr[len] == '\0'`
5. **Memory safety** — no leaks, no double-frees, no buffer overruns after correct use

## Non-goals

- Reference counting or copy-on-write (considered for future but not required)
- Small-string optimization (out of scope for v1)
- Thread safety (caller must synchronise)

## Invariant

```
s != NULL

Either:
    ptr == NULL
    len == 0
    cap == 0

Or:
    ptr != NULL
    cap >= len + 1
    ptr[len] == '\0'
```

## Planned API

| Function | Status |
|---|---|
| `cdstr_init` | ✅ implemented |
| `cdstr_destroy` | ✅ implemented |
| `cdstr_append` | ❌ missing (internal `str_append` exists) |
| `cdstr_append_n` | ❌ missing |
| `cdstr_assign` / `cdstr_assign_n` | ❌ missing |
| `cdstr_clear` | ❌ missing |
| `cdstr_copy` | ✅ implemented (needs fix) |
| `cdstr_reserve` | ✅ implemented (needs overflow check) |
| `cdstr_shrink` | ✅ implemented |
| `cdstr_len` / `cdstr_cap` | 🟡 wrong return type (should be `size_t`) |
| `cdstr_empty` | ❌ missing |

## Weak spots (current project issues)

- **No build system** — `Makefile` is empty
- **5 declared functions not implemented** — `cdstr_append`, `cdstr_append_n`, `cdstr_assign`, `cdstr_assign_n`, `cdstr_clear`
- **Public `cdstr_append` missing** — only an internal `str_append` exists
- **`cdstr_len`/`cdstr_cap` return `Err`** instead of `size_t`
- **`cdstr_copy` crashes on zeroed dest** — no NULL-ptr guard
- **`cdstr_reserve` has no overflow check** — `s->cap + count` can wrap
- **Tests are broken** — `cdstrtest.c` has duplicate defs, wrong arg counts, inverted exit codes
- **No CI** — no automated build or test runner
- **README is minimal** — no API docs or usage examples
