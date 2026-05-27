#include "../louro.h"
#include <stdio.h>

// 1. Infix Ternary Operator: a ? b : c
static double my_if_else(double condition, double true_val, double false_val) {
    return condition != 0.0 ? true_val : false_val;
}

// 2. Prefix Ternary Operator: if a else b
static double my_if_prefix(double condition, double false_val) {
    return condition != 0.0 ? condition : false_val;
}

// 3. Lazy AND Operator: a && b (Only evaluates b if a is truthy)
static double my_lazy_and(LouroLazy *left, LouroLazy *right) {
    if (!louro_lazy_eval(left)) return 0.0;
    return louro_lazy_eval(right);
}

// 4. Lazy Ternary Operator: a IF b ELSE c (Only evaluates the chosen branch)
static double my_lazy_ternary(LouroLazy *condition, LouroLazy *true_val, LouroLazy *false_val) {
    if (louro_lazy_eval(condition) != 0.0) {
        return louro_lazy_eval(true_val);
    }
    return louro_lazy_eval(false_val);
}

// Global variable to test side-effects
double x = 1.0;

int main() {
    // Register our environment
    LouroVariable exports[] = {
        LOURO_VAR("x", &x),
        LOURO_TERNARY("?", ":", my_if_else, 10),
        LOURO_TERNARY_PREFIX("if", "else", my_if_prefix, 10),
        LOURO_OP_LAZY("&&", my_lazy_and, 15),
        LOURO_TERNARY_LAZY("IF", "ELSE", my_lazy_ternary, 10)
    };
    int count = sizeof(exports) / sizeof(exports[0]);
    int err;

    // Example 1: Infix Ternary
    const char *script1 = "10 ? 200 : 300";
    LouroExpression *expr1 = louro_compile(script1, exports, count, &err);
    printf("1. Infix Ternary: %s => %f\n", script1, louro_evaluate(expr1));
    louro_free(expr1);

    // Example 2: Prefix Ternary
    const char *script2 = "if 0 else 500";
    LouroExpression *expr2 = louro_compile(script2, exports, count, &err);
    printf("2. Prefix Ternary: %s => %f\n", script2, louro_evaluate(expr2));
    louro_free(expr2);

    // Example 3: Lazy AND
    const char *script3 = "x && 100";
    LouroExpression *expr3 = louro_compile(script3, exports, count, &err);
    printf("3. Lazy AND: %s => %f\n", script3, louro_evaluate(expr3));
    louro_free(expr3);

    // Example 4: Lazy Ternary
    const char *script4 = "0 IF 999 ELSE 111";
    LouroExpression *expr4 = louro_compile(script4, exports, count, &err);
    printf("4. Lazy Ternary: %s => %f\n", script4, louro_evaluate(expr4));
    louro_free(expr4);

    return 0;
}
