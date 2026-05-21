#include "../louro.h"
#include <stdio.h>
#include <math.h>

int main() {
    printf("--- Louro Dynamic Variables Example ---\n");

    // We define some variables in C
    double health = 100.0;
    double damage = 15.5;

    // 1. We can inject variables and functions dynamically.
    // Notice how LOURO_PURE is used for pure math functions like 'sqrt',
    // while LOURO_VAR is used to bind pointers to our local variables.
    
    int err;
    LouroVariable scope1[] = {
        LOURO_VAR("health", &health),
        LOURO_VAR("damage", &damage),
        LOURO_PURE("sqrt", sqrt)
    };
    
    // Evaluate a complex expression using the variables and functions
    LouroExpression *expr1 = louro_compile("sqrt(health) + damage * 2", scope1, 3, &err);
    double result = 0;
    if (expr1) {
        result = louro_evaluate(expr1);
        louro_free(expr1);
    }

    printf("Result of 'sqrt(health) + damage * 2': %.2f\n", result);

    // 2. Because variables are bound by pointers, we can change their values in C
    // and re-evaluate without needing to parse the expression again.
    
    LouroVariable scope2[] = {
        LOURO_VAR("health", &health)
    };
    
    LouroExpression *expr = louro_compile("health - 10", scope2, 1, &err);
    if (expr) {
        printf("\nSimulating damage loop:\n");
        for (int i = 0; i < 3; i++) {
            health = louro_evaluate(expr); // No parsing overhead here!
            printf("Turn %d -> Health is now: %.2f\n", i+1, health);
        }
        louro_free(expr);
    }

    return 0;
}
