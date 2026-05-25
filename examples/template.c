// papagaio
#include "./papagaio/src/papagaio.h"

// louro
#include "./louro/louro.h"
#include "louro/examples/libs/louro_std.h"
#include "louro/examples/libs/louro_math.h"
#include "louro/examples/libs/urb.h"

#include <stdio.h>
#include <stdlib.h>

#define CONFIG_VERSION "0.0.1"

static char *read_file(const char *path, size_t *out_len) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(sz + 1);
    if (buf) {
        size_t rb = fread(buf, 1, sz, f);
        buf[rb] = '\0';
        if (out_len) *out_len = rb;
    }
    fclose(f);
    return buf;
}

int main(int argc, char* argv[]) {
    if (argc >= 2 && (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)) {
        printf("interpreter v%s\n", CONFIG_VERSION);
        return 0;
    }
    if (argc < 2 || (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        fprintf(stderr, "Usage: interpreter <file.txt> | -e \"code\" | -\n");
        return 1;
    }

    char* input = NULL;
    size_t len = 0;
    if (strcmp(argv[1], "-e") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: -e requires a string argument\n");
            return 1;
        }
        input = strdup(argv[2]);
        len = strlen(input);
    } else if (strcmp(argv[1], "-") == 0) {
        /* Read from stdin */
        size_t cap = 4096;
        input = malloc(cap);
        while (1) {
            size_t r = fread(input + len, 1, cap - len - 1, stdin);
            len += r;
            if (r == 0) break;
            if (len + 1024 >= cap) {
                cap <<= 1;
                input = realloc(input, cap);
            }
        }
        input[len] = '\0';
    } else {
        input = read_file(argv[1], &len);
    }

    if (!input) {
        fprintf(stderr, "Error reading input\n");
        return 1;
    }

    Papagaio *ctx = papagaio_open();
    double global = urb_new(16);

    LouroVariable static_memory[] = {
        LOURO_STD,
        LOURO_URB,
        LOURO_VAR("global", &global)
    };
    int count = sizeof(static_memory) / sizeof(static_memory[0]);

    
    char* preprocessed = papagaio_process_text(ctx, input, strlen(input));
    if(!preprocessed) {
        printf("Syntax Error: Failed to parse\n");
        return 1;
    }
    papagaio_close(ctx);

    LouroExpression *compiled = louro_compile(preprocessed, static_memory, count, NULL);
    if (!compiled) {
        printf("Syntax Error: Failed to parse\n");
        return 1;
    }
    free(preprocessed);
    
    double result = louro_evaluate(compiled);
    
    printf("%f\n", result);

    louro_free(compiled);

    return 0;
}
