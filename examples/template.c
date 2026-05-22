#include "../louro.h"
#include "libs/louro_std.h"
#include "libs/louro_math.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("--- Louro Starter Template ---\n");

    // 1. Define your custom C variables here
    double x = 10.0;
    double y = 5.0;

    // 2. Register standard operators, math functions, and your variables
    LouroVariable vars[] = {
        LOURO_STD,
        LOURO_MATH,
        LOURO_VAR("x", &x),
        LOURO_VAR("y", &y)
        // Add your custom operators or functions here!
        // LOURO_OP("=>", my_custom_func, 10),
    };
    int count = sizeof(vars) / sizeof(vars[0]);

    // 3. Define the expression to evaluate
    const char *expression = "sqrt(x^2 + y^2)";

    // 4. Compile the expression into an AST
    int err;
    LouroExpression *expr = louro_compile(expression, vars, count, &err);
    
    if (!expr) {
        printf("Syntax Error: Failed to parse at position %d\n", err);
        return 1;
    }

    // 5. Evaluate the AST
    double result = louro_evaluate(expr);
    printf("Result of '%s' is: %f\n", expression, result);

    // 6. Free the AST memory
    louro_free(expr);

    return 0;
}
