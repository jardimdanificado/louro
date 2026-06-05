CC = gcc
CFLAGS = -Wall -Wextra -I.. -O2
LDFLAGS = -lm

SRCS = 01_basic.c 02_variables.c 03_lists.c 05_custom_operators.c 06_benchmark.c 07_wasm_wat.c example_lazy_ternary.c
BINS = $(SRCS:.c=)

all: $(BINS)
	$(MAKE) -C louco_tutorial

%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(BINS)
	$(MAKE) -C louco_tutorial clean

.PHONY: all clean
