#include "../louro.h"
#include "libs/louro_std.h"
#include <stdio.h>

void print_indent(int depth) {
    for(int i=0; i<depth; i++) printf("  ");
}

void louro_emit_wat(const LouroExpression *n, int depth) {
    if (!n) return;
    
    int type = TYPE_MASK(n->type);
    
    if (type == LOURO_CONSTANT) {
        print_indent(depth);
        printf("f64.const %f\n", n->value);
        return;
    }
    
    if (type == LOURO_VARIABLE) {
        // In a real WASM compiler, you would map this pointer back to an index or memory offset.
        // For this demo, we'll pretend it's a WASM function parameter (local).
        print_indent(depth);
        printf("local.get $var_%p\n", (void*)n->bound);
        return;
    }
    
    // WebAssembly is a Stack Machine! 
    // We must evaluate the children nodes first and push them to the stack.
    int arity = ARITY(n->type);
    for (int i = 0; i < arity; i++) {
        louro_emit_wat((LouroExpression*)n->parameters[i], depth + 1);
    }
    
    print_indent(depth);
    
    // Now we map the C function pointers to WASM native opcodes!
    if (n->function == lr_add) {
        printf("f64.add\n");
    } else if (n->function == lr_sub) {
        printf("f64.sub\n");
    } else if (n->function == lr_mul) {
        printf("f64.mul\n");
    } else if (n->function == lr_divide) {
        printf("f64.div\n");
    } else if (n->function == lr_negate) {
        printf("f64.neg\n");
    } else if (n->function == lr_cmp_gt) {
        // WASM logical ops return i32 (integer), but Louro expects f64.
        // We do a fast conversion right after comparing!
        printf("f64.gt\n"); 
        print_indent(depth);
        printf("f64.convert_i32_u\n");
    } else {
        // Fallback for custom functions like 'sin' or custom impure logic
        // We emit a call to an imported host function.
        printf("call $host_function_%p\n", n->function);
    }
}

int main() {
    printf("--- Louro to WebAssembly (WAT) Experimental Compiler ---\n\n");
    
    double x = 0;
    LouroVariable vars[] = {
        LOURO_STD,
        LOURO_VAR("x", &x)
    };
    int count = sizeof(vars)/sizeof(vars[0]);
    
    int err;
    const char *expr_str = "10.5 + 2 * x - 5.0";
    
    printf("Compiling Expression: %s\n", expr_str);
    LouroExpression *expr = louro_compile(expr_str, vars, count, &err);
    
    if (expr) {
        printf("\nGenerated WAT (WebAssembly Text):\n");
        printf("(module\n");
        printf("  (func $evaluate (param $var_%p f64) (result f64)\n", (void*)&x);
        
        // Start recursive compilation
        louro_emit_wat(expr, 2);
        
        printf("  )\n");
        printf("  (export \"evaluate\" (func $evaluate))\n");
        printf(")\n");
        
        louro_free(expr);
    } else {
        printf("Parse error at %d\n", err);
    }
    
    return 0;
}
