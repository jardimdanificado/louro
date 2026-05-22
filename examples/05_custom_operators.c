#include "../louro.h"
#include "libs/louro_std.h"
#include <stdio.h>

// Helper func
double my_interpret(const char *expr, const LouroVariable *vars, int count, int *err) {
    LouroExpression *n = louro_compile(expr, vars, count, err);
    if (!n) return NAN;
    double ret = louro_evaluate(n);
    louro_free(n);
    return ret;
}

// 1. Custom Prefix and Postfix Functions
double op_factorial(double a) {
    if (a < 0) return NAN;
    double res = 1;
    for (int i = 2; i <= (int)a; i++) res *= i;
    return res;
}
double op_not(double a) {
    return a == 0.0 ? 1.0 : 0.0;
}

int main() {
    printf("--- Louro Custom Dynamic Operators ---\n");

    // We register ALL standard operators manually since the engine is now "naked".
    // We can reuse the lr_* built-in functions from louro.h
    LouroVariable vars[] = {
        LOURO_STD,
        // New Prefix and Postfix!
        LOURO_OP_PREFIX("!", op_not, 60),
        LOURO_OP_POSTFIX("!", op_factorial, 70)
    };
    int vars_count = sizeof(vars) / sizeof(vars[0]);

    int err;

    // Test 1: standard operators
    double r1 = my_interpret("10 + 5 * (2 - 1) ^ 3", vars, vars_count, &err);
    printf("10 + 5 * (2 - 1) ^ 3 = %.2f (Expected 15.00)\n", r1);

    // Test 2: comparisons
    double r2 = my_interpret("10 * 2 >= 20", vars, vars_count, &err);
    printf("10 * 2 >= 20 is %s\n", r2 > 0.0 ? "True" : "False");

    // Test 3: Unary prefix
    double r3 = my_interpret("-5 + -(-10)", vars, vars_count, &err);
    printf("-5 + -(-10) = %.2f (Expected 5.00)\n", r3);

    // Test 4: Custom Postfix and Prefix operators sharing the same symbol!
    double r4 = my_interpret("!0 + 5!", vars, vars_count, &err);
    // !0 = 1.0
    // 5! = 120.0
    // 1.0 + 120.0 = 121.0
    printf("!0 + 5! = %.2f (Expected 121.00)\n", r4);

    return 0;
}
