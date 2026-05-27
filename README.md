# Louro

Louro is a heavily modified fork of *TinyExpr* designed to be a tiny Domain Specific Language creation library. You must manually register all operators and functions that you want the language to recognize.

## Usage

```c
#include "louro.h"
#include "libs/louro_std.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Example custom functions
double my_rand() { return (double)(rand() % 100); }
double my_factorial(double a) { return a <= 1 ? 1 : a * my_factorial(a - 1); }
double logical_not(double a) { return a == 0.0 ? 1.0 : 0.0; }

int main() {
    double health = 100.0;

    // Register one element of each supported type
    LouroVariable scope[] = {
        LOURO_STD,                                // 1. Built-in standard library
        LOURO_VAR("health", &health),             // 2. Bound variable (Pointer)
        LOURO_PURE("abs", fabs, 1),               // 3. Pure Function (Optimizable)
        LOURO_IMPURE("rand", my_rand, 0),         // 4. Impure Function (Evaluated at runtime)
        LOURO_OP("=>", lr_cmp_ge, 20),            // 5. Infix Operator
        LOURO_OP_RIGHT("**", pow, 50),            // 6. Right-associative Operator
        LOURO_OP_PREFIX("NOT ", logical_not, 60), // 7. Prefix Operator
        LOURO_OP_POSTFIX("!", my_factorial, 70)   // 8. Postfix Operator
    };

    int err;
    int count = sizeof(scope) / sizeof(scope[0]);
    LouroExpression *expr = louro_compile("NOT (health => 50) + rand() * 2**3 + 5!", scope, count, &err);

    if (expr) {
        double result = louro_evaluate(expr);
        printf("Result: %f\n", result);
        louro_free(expr);
    } else {
        printf("Syntax Error at character %d\n", err);
    }

    return 0;
}
```

## Built-in Macros

- **`LOURO_PURE("name", function, arity)`**: Injects a pure function (e.g., `sin`, `sqrt`). Pure functions with constant arguments will be pre-calculated during compilation to save CPU cycles.
- **`LOURO_IMPURE("name", function, arity)`**: Injects an impure function (e.g., `rand`). Impure functions are never pre-calculated and will always execute at runtime.
- **`LOURO_VAR("name", pointer)`**: Injects a bound variable using a pointer to a `double`.
- **`LOURO_OP("symbol", function, precedence)`**: Registers a custom dynamic operator (e.g., `+`, `mod`, `=>`) with a specific precedence (left-associative, infix).
- **`LOURO_OP_RIGHT("symbol", function, precedence)`**: Registers a right-associative custom dynamic operator (e.g., `**`, `^`).
- **`LOURO_OP_PREFIX("symbol", function, precedence)`**: Registers a prefix unary operator (e.g., `!x`, `-x`).
- **`LOURO_OP_POSTFIX("symbol", function, precedence)`**: Registers a postfix unary operator (e.g., `x!`).
- **`LOURO_TERNARY("sym", "sep", function, precedence)`**: Registers a ternary infix operator (e.g., `a ? b : c`).
- **`LOURO_TERNARY_PREFIX("sym", "sep", function, precedence)`**: Registers a ternary prefix operator (e.g., `if a else b`).

### Lazy Evaluation (Short-Circuiting)

Louro supports universal lazy evaluation for any operator or function. This allows you to implement short-circuiting (like C's `&&` or `||`) where arguments are only evaluated if necessary.

To use it, append `_LAZY` to any macro (e.g., `LOURO_OP_LAZY`, `LOURO_TERNARY_LAZY`). Your C function will receive opaque `LouroLazy*` pointers instead of `double` values. You evaluate them manually using `louro_lazy_eval()`:

```c
// Evaluates 'right' only if 'left' is truthy (C '&&' semantics)
static double my_lazy_and(LouroLazy *left, LouroLazy *right) {
    if (!louro_lazy_eval(left)) return 0.0;
    return louro_lazy_eval(right);
}

// Register it
LOURO_OP_LAZY("&&", my_lazy_and, 20);
```

### AOT Code Generator (`louco`)

Louro includes a powerful Ahead-Of-Time (AOT) compiler called `louco` (Louro Code Generator). It reads an Arara script and transpiles it into hyper-optimized, native C code. 

**Usage:**
```bash
./louco.sh -e my_env.h input_script.txt -o out.c
```
The AOT transpiler guarantees **100% semantic compatibility** with the interpreter. It uses deterministic 2-pass inline thunk generation to compile `_LAZY` operators into zero-overhead native C short-circuiting logic!

## License
Louro is provided under the Zlib license. See the top of `louro.h` for more details.
