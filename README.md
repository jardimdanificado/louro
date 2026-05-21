# Louro

Louro is a tiny Domain Specific Language creation library.

Louro is a heavily modified fork of *TinyExpr*.

## Features

- **Header-Only:** Just `#include "louro.h"` and you are ready to go. No Makefiles, no linking.
- **Dependency Free:** You inject the functions you need.
- **Boolean Logic:** Natively supports `<, >, <=, >=, ==, !=`. Returns `1.0` for true and `0.0` for false.

## Usage

```c
double x = 0.0;

LouroVariable scope[] = {
    LOURO_VAR("x", &x),
    LOURO_PURE("sin", sin)
};

int err;
LouroExpression *expr = louro_compile("sin(x) * 10", scope, 2, &err);

for (int i = 0; i < 1000000; i++) {
    x = (double)i;
    double result = louro_evaluate(expr);
}

louro_free(expr);
```

## Built-in Macros (C11)

Louro uses `_Generic` under the hood to automatically detect the number of arguments of your C functions, stripping away the need for verbose casting and flags.

- **`LOURO_PURE("name", function)`**: Injects a pure function (e.g., `sin`, `sqrt`). Pure functions with constant arguments will be pre-calculated during compilation to save CPU cycles.
- **`LOURO_IMPURE("name", function)`**: Injects an impure function (e.g., `rand`). Impure functions are never pre-calculated and will always execute at runtime.
- **`LOURO_VAR("name", pointer)`**: Injects a bound variable using a pointer to a `double`.

## License
Louro is provided under the Zlib license. See the top of `louro.h` for more details.
