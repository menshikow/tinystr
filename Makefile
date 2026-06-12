CC = clang
CFLAGS = -std=c23 -Wall -Wextra -pedantic -g

.PHONY: all clean test asan

all: build/test_runner

build/test_runner: tests/test_sstr.c sstr.h
		mkdir -p build
		$(CC) $(CFLAGS) -o $@ tests/test_sstr.c

asan: CFLAGS += -fsanitize=address -fno-omit-frame-pointer
asan: build/test_runner

clean:
		rm -rf build

test: build/test_runner
		./build/test_runner
