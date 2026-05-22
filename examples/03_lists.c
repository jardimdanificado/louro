#include "../louro.h"
#include "libs/louro_std.h"
#include "libs/urb.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("--- Louro Lists & Typed Arrays Example ---\n\n");

    // 1. Create a C variable to hold the list reference
    double my_list = 0.0; // Will be initialized in the script

    // 2. We register the standard operators and the urb list functions
    LouroVariable vars[] = {
        LOURO_STD,
        LOURO_URB,
        LOURO_VAR("my_list", &my_list)
    };
    int count = sizeof(vars) / sizeof(vars[0]);

    // 3. We can evaluate multiple expressions sequentially. State is preserved!
    my_list = urb_new(10);
    
    int err;
    LouroExpression *e1 = louro_compile("push(my_list, 3.14)", vars, count, &err);
    LouroExpression *e2 = louro_compile("push(my_list, 2.71)", vars, count, &err);
    LouroExpression *e3 = louro_compile("set_i8(my_list, 0, -128)", vars, count, &err);
    LouroExpression *e4 = louro_compile("get_i8(my_list, 0)", vars, count, &err);
    
    if (!e1 || !e2 || !e3 || !e4) {
        printf("Syntax Error: Failed to parse\n");
        return 1;
    }

    printf("Executing script...\n");
    louro_evaluate(e1);
    louro_evaluate(e2);
    louro_evaluate(e3);
    double result = louro_evaluate(e4);
    
    printf("Result of script (get_i8 at byte 0): %f\n", result);

    louro_free(e1);
    louro_free(e2);
    louro_free(e3);
    louro_free(e4);

    return 0;
}
