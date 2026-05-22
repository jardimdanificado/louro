#include "../louro.h"
#include "libs/louro_std.h"
#include <stdio.h>
#include <time.h>
#include <math.h>

#define ITERATIONS 10000000

// Pure C function for comparison
double pure_c_evaluation(double x, double y) {
    return sqrt(pow(x, 2) + pow(y, 2)) + sin(x) * cos(y);
}

int main() {
    printf("--- Louro Performance Benchmark ---\n");
    printf("Evaluating expression: 'sqrt(x^2 + y^2) + sin(x) * cos(y)'\n");
    printf("Iterations: %d\n\n", ITERATIONS);

    double x = 0.5;
    double y = 0.8;

    // 1. Pure C Benchmark
    printf("[1] Running Pure C Benchmark...\n");
    clock_t start_c = clock();
    double result_c = 0.0;
    for (int i = 0; i < ITERATIONS; i++) {
        // Change variables slightly to prevent compiler from optimizing the loop away entirely
        x += 0.0000001; 
        y -= 0.0000001;
        result_c = pure_c_evaluation(x, y);
    }
    clock_t end_c = clock();
    double time_c = ((double)(end_c - start_c)) / CLOCKS_PER_SEC;

    // Reset variables
    x = 0.5;
    y = 0.8;

    // 2. Louro AST Benchmark
    printf("[2] Running Louro AST Benchmark...\n");
    int err;
    LouroVariable vars[] = {
        LOURO_STD,
        LOURO_PURE("sqrt", sqrt, 1),
        LOURO_PURE("sin", sin, 1),
        LOURO_PURE("cos", cos, 1),
        LOURO_VAR("x", &x),
        LOURO_VAR("y", &y)
    };
    int count = sizeof(vars) / sizeof(vars[0]);

    // Compile once outside the loop
    clock_t start_compile = clock();
    LouroExpression *expr = louro_compile("sqrt(x^2 + y^2) + sin(x) * cos(y)", vars, count, &err);
    clock_t end_compile = clock();
    
    if (!expr) {
        printf("Failed to compile Louro expression. Error: %d\n", err);
        return 1;
    }

    clock_t start_louro = clock();
    double result_louro = 0.0;
    for (int i = 0; i < ITERATIONS; i++) {
        // The AST has pointers directly to &x and &y, so we just change the C variables!
        x += 0.0000001;
        y -= 0.0000001;
        result_louro = louro_evaluate(expr);
    }
    clock_t end_louro = clock();
    double time_louro = ((double)(end_louro - start_louro)) / CLOCKS_PER_SEC;

    louro_free(expr);

    // Results
    printf("\n--- Results ---\n");
    printf("Final Math Result C:     %f\n", result_c);
    printf("Final Math Result Louro: %f\n\n", result_louro);

    printf("Compilation Time: %.6f seconds\n", ((double)(end_compile - start_compile)) / CLOCKS_PER_SEC);
    printf("Pure C Time:      %.6f seconds (%.2fM evals/sec)\n", time_c, (ITERATIONS / time_c) / 1000000.0);
    printf("Louro AST Time:   %.6f seconds (%.2fM evals/sec)\n", time_louro, (ITERATIONS / time_louro) / 1000000.0);
    
    double overhead = (time_louro / time_c);
    printf("\nLouro Overhead: ~%.2fx slower than hardcoded, highly-optimized C code.\n", overhead);
    printf("For a fully dynamic interpreted AST with bound pointers, this is blistering fast!\n");

    return 0;
}
