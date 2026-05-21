#include "../louro.h"
#include <stdio.h>

int main() {
    printf("--- Louro Basic Example ---\n");

    // 1. Evaluate a simple mathematical expression
    int err;
    double result = louro_interpret("10 + 5 * 2", &err);
    printf("10 + 5 * 2 = %.2f\n", result);

    // 2. Evaluate a boolean logic expression
    // Returns 1.0 for true, 0.0 for false
    double is_true = louro_interpret("20 > 15", &err);
    printf("20 > 15 is %s\n", is_true > 0.0 ? "True" : "False");

    // 3. Handling syntax errors
    double error_result = louro_interpret("10 + * 2", &err);
    if (isnan(error_result)) {
        printf("Syntax error detected successfully!\n");
    }

    return 0;
}
