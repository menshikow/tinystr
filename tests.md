# Testing C Libraries — A Practical Guide

## 1. Choose a test harness

| Approach | Pros | Cons | Best for |
|---|---|---|---|
| `assert.h` | zero deps, always available | no runner, first fail stops | smoke tests |
| Simple macro harness | zero deps, customisable | you write it | tiny libs like cdstr |
| [utest.h](https://github.com/sheredom/utest.h) | single-header, C/C++, MIT | minimal features | small-to-medium C libs |
| [Unity](https://github.com/ThrowTheSwitch/Unity) | embedded-friendly, xUnit | heavier setup | serious C projects |
| [Check](https://libcheck.github.io/check/) | fork-per-test isolation | POSIX only | system-level C libs |
| [Criterion](https://github.com/Snaipe/Criterion) | modern, parameterised | bigger binary | mid-size projects |

For cdstr (a 200-line lib), a **custom macro harness** or **utest.h** (already in `dependencies/`) is the best fit.

## 2. Structure your tests

One file per function, each testing happy path + edge cases + error paths:

```
tests/
  test_init.c          # cdstr_init
  test_append.c        # cdstr_append / cdstr_append_n
  test_assign.c        # cdstr_assign / cdstr_assign_n
  test_clear.c         # cdstr_clear
  test_copy.c          # cdstr_copy
  test_reserve.c       # cdstr_reserve
  test_shrink.c        # cdstr_shrink
  test_invariants.c    # invariant checks after every op
  test_harness.h       # shared macros, pass/fail counter
  test_main.c          # runs all suites
```

## 3. Test layers

| Layer | What | Why |
|---|---|---|
| **Unit** | one test per public function | each function works in isolation |
| **Invariant** | check `cap >= len+1` and `ptr[len]=='\0'` after every op | container correctness |
| **Round-trip** | `init(a) → append(b) → strcmp(s.ptr, a∥b) == 0` | higher-level correctness |
| **Edge cases** | empty string, NULL, DEFAULT_CAPACITY boundary, growth | robustness |
| **Memory** | AddressSanitizer (`-fsanitize=address,undefined`) + Valgrind | no leaks, no UB |

## 4. Build & run

Use a `Makefile` with targets:

```makefile
CC       = cc
CFLAGS   = -std=c11 -Wall -Wextra -pedantic -g -I include -I dependencies/utest.h
SRC      = src/cdstr.c
TESTS    = tests/test_init.c tests/test_append.c ... tests/test_main.c
TEST_BIN = build/test_runner

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(SRC) $(TESTS)
	mkdir -p build
	$(CC) $(CFLAGS) -o $@ $^

test-asan: CFLAGS += -fsanitize=address,undefined -fno-omit-frame-pointer
test-asan: clean test

test-valgrind: test
	valgrind --leak-check=full --error-exitcode=1 ./$(TEST_BIN)

clean:
	rm -rf build
```

## 5. Write a simple macro harness

```c
// tests/test_harness.h
#ifndef TEST_HARNESS_H
#define TEST_HARNESS_H

#include <stdio.h>

static int tests_passed  = 0;
static int tests_failed  = 0;
static int tests_ran     = 0;

#define RUN_TEST(name) do {                           \
    tests_ran++;                                       \
    int ok = test_##name();                            \
    if (ok) { tests_passed++; }                        \
    else     { tests_failed++;                         \
               fprintf(stderr, "  FAIL %s\n", #name); }\
} while (0)

#define ASSERT(cond, msg) do {                        \
    if (!(cond)) {                                     \
        fprintf(stderr, "    line %d: %s\n",           \
                __LINE__, msg);                        \
        return 0;                                      \
    }                                                   \
} while (0)

static int test_summary(void) {
    printf("\n%d / %d tests passed", tests_passed, tests_ran);
    if (tests_failed) printf(", %d failed", tests_failed);
    putchar('\n');
    return tests_failed == 0 ? 0 : 1;
}

#endif
```

Example test file:

```c
#include "cdstr.h"
#include "test_harness.h"

static int test_init_basic(void) {
    String s;
    Err e = cdstr_init(&s, "hello");
    ASSERT(e == SUCCESS, "init should succeed");
    ASSERT(s.ptr != NULL, "ptr should be allocated");
    ASSERT(s.len == 5, "len should be 5");
    ASSERT(s.cap >= s.len + 1, "cap >= len+1");
    ASSERT(s.ptr[s.len] == '\0', "null-terminated");
    cdstr_destroy(&s);
    return 1;
}
```

## 6. Memory testing workflow

```bash
# AddressSanitizer (fast, detailed)
make test-asan

# Valgrind (thorough, slower)
make test-valgrind

# Both must exit 0 before considering tests clean
```

## 7. What to test for each function

| Function | Test cases |
|---|---|
| `cdstr_init` | normal string, empty `""`, NULL input, very long string |
| `cdstr_destroy` | normal string, already destroyed, NULL `s` |
| `cdstr_append` | append to empty, append to existing, append empty `""`, NULL slice, self-append, capacity growth boundary |
| `cdstr_append_n` | partial slice, len=0, len > strlen(slice) |
| `cdstr_assign` | replace content, replace with `""`, replace with longer (growth) |
| `cdstr_clear` | clear non-empty, clear already empty |
| `cdstr_copy` | src→dest, copy to zeroed dest (NULL ptr), self-copy, copy then destroy src (dest must survive) |
| `cdstr_reserve` | add capacity, reserve 0, reserve on NULL-ptr string |
| `cdstr_shrink` | shrink after append, shrink already-tight string |
| `cdstr_len` / `cdstr_cap` | normal string, empty string, after growth |
| `cdstr_empty` | empty string, non-empty string, destroyed string |

## 8. Common pitfalls in C testing

- **Undefined behaviour in tests** — a passing test with UB is worse than a failing test. Always use `-fsanitize=undefined`.
- **Test order dependencies** — never rely on global state leaking between tests. Use setup/teardown or fresh state per test.
- **NULL checks** — test that every function handles `NULL` inputs gracefully (without crashing).
- **Inverted exit codes** — `main` should return 0 on success, non-zero on failure (shell convention).
- **Memory leaks** — invisible to logic tests. Must run under ASan/Valgrind.

## 9. Resources

- James Grenning, *Test-Driven Development for Embedded C*
- [AddressSanitizer docs](https://github.com/google/sanitizers/wiki/AddressSanitizer)
- [Valgrind Quick Start](https://valgrind.org/docs/manual/quick-start.html)
- [SQLite testing](https://www.sqlite.org/testing.html) — gold standard
