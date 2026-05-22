#include "../louro.h"
#include "libs/louro_std.h"
#include <stdio.h>
#include <math.h>

double my_interpret(const char *expr, const LouroVariable *vars, int count, int *err) {
    LouroExpression *n = louro_compile(expr, vars, count, err);
    if (!n) return NAN;
    double ret = louro_evaluate(n);
    louro_free(n);
    return ret;
}

int main() {
    printf("--- Louro Basic Example ---\n");

    // Standard operators must be registered since the engine is now "naked"
    LouroVariable vars[] = {
        LOURO_STD
    };
    int vars_count = sizeof(vars) / sizeof(vars[0]);

    // 1. Evaluate a simple mathematical expression
    int err;
    double result = my_interpret("10 + 5 * 2", vars, vars_count, &err);
    printf("10 + 5 * 2 = %.2f\n", result);

    // 2. Evaluate a boolean logic expression
    // Returns 1.0 for true, 0.0 for false
    double is_true = my_interpret("20 > 15", vars, vars_count, &err);
    printf("20 > 15 is %s\n", is_true > 0.0 ? "True" : "False");

    // 3. Handling syntax errors
    double error_result = my_interpret("10 + * 2", vars, vars_count, &err);
    if (isnan(error_result)) {
        printf("Syntax error detected successfully!\n");
    }

    return 0;
}
