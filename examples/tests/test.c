#include "../../louro.h"
#include "../libs/louro_std.h"
#include "../libs/louro_math.h"
#include <stdio.h>
#include <math.h>

static const LouroVariable LOURO_STANDARD_OPS[] = {
    LOURO_STD
};
static const int LOURO_STANDARD_OPS_COUNT = sizeof(LOURO_STANDARD_OPS) / sizeof(LOURO_STANDARD_OPS[0]);

static double test_interpret(const char *expr, int *err) {
    LouroExpression *n = louro_compile(expr, LOURO_STANDARD_OPS, LOURO_STANDARD_OPS_COUNT, err);
    if (!n) return NAN;
    double ret = louro_evaluate(n);
    louro_free(n);
    return ret;
}

#define RUN_TEST(name, expr, expected) \
    do { \
        int err; \
        double result = test_interpret((expr), &err); \
        if (err != 0) { \
            printf("[FAIL] %s: '%s' -> Parse error at %d\n", (name), (expr), err); \
        } else if (fabs(result - (expected)) > 1e-7) { \
            printf("[FAIL] %s: '%s' -> Expected %f, got %f\n", (name), (expr), (double)(expected), result); \
        } else { \
            printf("[PASS] %s\n", (name)); \
        } \
    } while (0)

#define RUN_TEST_VARS(name, expr, expected, vars, count) \
    do { \
        int err; \
        LouroExpression *n = louro_compile((expr), (vars), (count), &err); \
        if (!n) { \
            printf("[FAIL] %s: '%s' -> Parse error at %d\n", (name), (expr), err); \
        } else { \
            double result = louro_evaluate(n); \
            louro_free(n); \
            if (fabs(result - (expected)) > 1e-7) { \
                printf("[FAIL] %s: '%s' -> Expected %f, got %f\n", (name), (expr), (double)(expected), result); \
            } else { \
                printf("[PASS] %s\n", (name)); \
            } \
        } \
    } while (0)


void test_basic_math() {
    printf("--- Basic Math Tests ---\n");
    RUN_TEST("Addition", "2 + 2", 4.0);
    RUN_TEST("Subtraction", "5 - 3", 2.0);
    RUN_TEST("Multiplication", "3 * 4", 12.0);
    RUN_TEST("Division", "10 / 2", 5.0);
    RUN_TEST("Precedence", "2 + 3 * 4", 14.0);
    RUN_TEST("Parentheses", "(2 + 3) * 4", 20.0);
    RUN_TEST("Power", "2 ^ 3", 8.0);
    RUN_TEST("Modulo", "10 % 3", 1.0);
}

void test_functions() {
    printf("\n--- Function Tests (Injected via variables) ---\n");
    
    LouroVariable funcs[] = {
        LOURO_STD,
        LOURO_MATH
    };
    int count = sizeof(funcs) / sizeof(funcs[0]);

    RUN_TEST_VARS("sqrt", "sqrt(16)", 4.0, funcs, count);
    RUN_TEST_VARS("abs", "abs(-5)", 5.0, funcs, count);
    RUN_TEST_VARS("sin", "sin(0)", 0.0, funcs, count);
    RUN_TEST_VARS("cos", "cos(0)", 1.0, funcs, count);
    RUN_TEST_VARS("pi constant", "pi", 3.14159265358979323846, funcs, count);
    RUN_TEST_VARS("max 2 args (atan2)", "atan2(0, -1)", 3.14159265358979323846, funcs, count);
}

void test_comparisons() {
    printf("\n--- Comparison Tests ---\n");
    RUN_TEST("Less than (True)", "2 < 3", 1.0);
    RUN_TEST("Less than (False)", "3 < 2", 0.0);
    RUN_TEST("Greater than (True)", "5 > 4", 1.0);
    RUN_TEST("Greater than (False)", "4 > 5", 0.0);
    RUN_TEST("Less or equal (True)", "3 <= 3", 1.0);
    RUN_TEST("Greater or equal (True)", "4 >= 4", 1.0);
    RUN_TEST("Equal (True)", "5 == 5", 1.0);
    RUN_TEST("Equal (False)", "5 == 6", 0.0);
    RUN_TEST("Not equal (True)", "5 != 6", 1.0);
    RUN_TEST("Not equal (False)", "5 != 5", 0.0);
    RUN_TEST("Precedence over addition", "1 + 2 == 3", 1.0);
    RUN_TEST("Precedence over multiplication", "2 * 3 > 5", 1.0);
}

void test_variables() {
    printf("\n--- Variable Tests ---\n");
    double x = 3, y = 4;
    LouroVariable vars[] = {
        LOURO_STD,
        LOURO_VAR("x", &x),
        LOURO_VAR("y", &y)
    };
    int err;

    LouroExpression *expr = louro_compile("x^2 + y^2", vars, sizeof(vars)/sizeof(vars[0]), &err);
    if (!expr) {
        printf("[FAIL] Variable Compilation: Parse error at %d\n", err);
        return;
    }

    double res1 = louro_evaluate(expr);
    if (fabs(res1 - 25.0) < 1e-7) {
        printf("[PASS] Variable evaluation 1 (x=3, y=4 -> 25)\n");
    } else {
        printf("[FAIL] Variable evaluation 1: Expected 25.0, got %f\n", res1);
    }

    x = 5; y = 12;
    double res2 = louro_evaluate(expr);
    if (fabs(res2 - 169.0) < 1e-7) {
        printf("[PASS] Variable evaluation 2 (x=5, y=12 -> 169)\n");
    } else {
        printf("[FAIL] Variable evaluation 2: Expected 169.0, got %f\n", res2);
    }

    louro_free(expr);
}

void test_stress() {
    printf("\n--- Stress & Edge Case Tests ---\n");
    
    // Right associativity: 2^3^2 = 2^(3^2) = 2^9 = 512
    RUN_TEST("Right associativity", "2^3^2", 512.0);
    
    // Deep nesting (tests parser stack limits, though Pratt parser uses C call stack)
    RUN_TEST("Deep nesting", "(((((((((10)))))))))", 10.0);
    
    // Consecutive prefix operators
    RUN_TEST("Consecutive prefixes", "---5", -5.0);
    RUN_TEST("Consecutive mixed prefixes", "-(-5)", 5.0);
    
    // Extreme Precedence Mixing: 1 + (2 * (3^4)) - (5 / 2) = 1 + 162 - 2.5 = 160.5
    RUN_TEST("Heavy precedence", "1+2*3^4-5/2", 160.5); 
    
    // Decimal anomalies (no leading zero)
    RUN_TEST("Decimals without leading zero", ".5 + .25", 0.75);
    
    // Long chain of the same precedence
    RUN_TEST("Long expression", "1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1", 20.0);
    
    // Math mixed with logical boolean returns (Louro booleans return 1.0 or 0.0)
    // (10 > 5) evaluates to 1.0. (2 == 3) evaluates to 0.0. 
    RUN_TEST("Math with booleans", "(10 > 5) * 100 + (2 == 3) * 50", 100.0);
    
    // Function arguments parsed as separate expression streams
    LouroVariable funcs[] = {
        LOURO_STD,
        LOURO_PURE("abs", fabs, 1)
    };
    int count = sizeof(funcs) / sizeof(funcs[0]);
    RUN_TEST_VARS("Function argument expressions", "10 + abs(-5 * 2) * 2", 30.0, funcs, count);
}

int main() {
    test_basic_math();
    test_functions();
    test_comparisons();
    test_variables();
    test_stress();
    
    printf("\nAll tests finished.\n");
    return 0;
}
