CC = gcc
CFLAGS = -Wall -Wextra -I../.. -O2
LDFLAGS = -lm

LOUCO = ../../louco

all: build

out.c: script.txt my_env.h
	$(LOUCO) -e my_env.h -o out.c script.txt

build: out.c
	$(CC) $(CFLAGS) out.c -o out $(LDFLAGS)

clean:
	rm -f out.c out tmp_aot_compiler

.PHONY: all build clean
