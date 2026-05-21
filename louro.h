// SPDX-License-Identifier: Zlib
/*
 * TINYEXPR - Tiny recursive descent parser and evaluation engine in C
 *
 * Copyright (c) 2015-2020 Lewis Van Winkle
 *
 * http://CodePlea.com
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgement in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/*
 * NOTICE: This is a *HEAVILY* modified version of the original TinyExpr library.
 */

#ifndef LOURO_H
#define LOURO_H

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>

#ifndef NAN
#define NAN (0.0/0.0)
#endif

#ifndef INFINITY
#define INFINITY (1.0/0.0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LouroExpression {
    int type;
    union {double value; const double *bound; const void *function;};
    void *parameters[1];
} LouroExpression;


enum {
    LOURO_VARIABLE = 0,

    LOURO_FUNCTION0 = 8, LOURO_FUNCTION1, LOURO_FUNCTION2, LOURO_FUNCTION3,
    LOURO_FUNCTION4, LOURO_FUNCTION5, LOURO_FUNCTION6, LOURO_FUNCTION7,

    LOURO_CLOSURE0 = 16, LOURO_CLOSURE1, LOURO_CLOSURE2, LOURO_CLOSURE3,
    LOURO_CLOSURE4, LOURO_CLOSURE5, LOURO_CLOSURE6, LOURO_CLOSURE7,

    LOURO_FLAG_PURE = 32
};

/* Builtin function descriptor (used internally; no variable binding). */
typedef struct LouroVariable {
    const char *name;
    const void *address;
    int type;
    void *context;
} LouroVariable;
#define LOURO_VAR(name, ptr) {name, (const void*)(ptr), LOURO_VARIABLE, 0}

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    /* Helper macro to automatically detect function arity (C11 only) */
    #define LOURO_ARITY(func) _Generic((func), \
        double (*)(void): LOURO_FUNCTION0, \
        double (*)(double): LOURO_FUNCTION1, \
        double (*)(double, double): LOURO_FUNCTION2, \
        double (*)(double, double, double): LOURO_FUNCTION3, \
        double (*)(double, double, double, double): LOURO_FUNCTION4, \
        double (*)(double, double, double, double, double): LOURO_FUNCTION5, \
        double (*)(double, double, double, double, double, double): LOURO_FUNCTION6, \
        double (*)(double, double, double, double, double, double, double): LOURO_FUNCTION7 \
    )

    #define LOURO_PURE(name, func)   {name, (const void*)(func), LOURO_ARITY(func) | LOURO_FLAG_PURE, 0}
    #define LOURO_IMPURE(name, func) {name, (const void*)(func), LOURO_ARITY(func), 0}
#endif


/* Parses the input expression, evaluates it, and frees it. */
/* Returns NaN on error. */
/* Comparison operators: <, >, <=, >=, ==, != return 1.0 (true) or 0.0 (false). */
static inline double louro_interpret(const char *expression, int *error);


/* Parses the input expression. */
/* Returns NULL on error. */
static inline LouroExpression *louro_compile(const char *expression, const LouroVariable *variables, int var_count, int *error);

/* Evaluates the expression. */
static inline double louro_evaluate(const LouroExpression *n);

/* Prints debugging information on the syntax tree. */
static inline void louro_print(const LouroExpression *n);

/* Frees the expression. */
/* This is safe to call on NULL pointers. */
static inline void louro_free(LouroExpression *n);


/* 
 * ============================================================================
 * IMPLEMENTATION
 * ============================================================================
 */

typedef double (*lr_fun2)(double, double);

enum {
    TOK_NULL = LOURO_CLOSURE7+1, TOK_ERROR, TOK_END, TOK_SEP,
    TOK_OPEN, TOK_CLOSE, TOK_NUMBER, TOK_VARIABLE, TOK_INFIX
};


enum {LOURO_CONSTANT = 1};


typedef struct state {
    const char *start;
    const char *next;
    int type;
    union {double value; const double *bound; const void *function;};
    void *context;

    const LouroVariable *lookup;
    int lookup_len;
} state;


#define TYPE_MASK(TYPE) ((TYPE)&0x0000001F)

#define IS_PURE(TYPE) (((TYPE) & LOURO_FLAG_PURE) != 0)
#define IS_FUNCTION(TYPE) (((TYPE) & LOURO_FUNCTION0) != 0)
#define IS_CLOSURE(TYPE) (((TYPE) & LOURO_CLOSURE0) != 0)
#define ARITY(TYPE) ( ((TYPE) & (LOURO_FUNCTION0 | LOURO_CLOSURE0)) ? ((TYPE) & 0x00000007) : 0 )
#define NEW_EXPR(type, ...) new_expr((type), (const LouroExpression*[]){__VA_ARGS__})
#define CHECK_NULL(ptr, ...) if ((ptr) == NULL) { __VA_ARGS__; return NULL; }

static inline LouroExpression *new_expr(const int type, const LouroExpression *parameters[]) {
    const int arity = ARITY(type);
    const int psize = sizeof(void*) * arity;
    const int size = (sizeof(LouroExpression) - sizeof(void*)) + psize + (IS_CLOSURE(type) ? sizeof(void*) : 0);
    LouroExpression *ret = (LouroExpression*)malloc(size);
    CHECK_NULL(ret);

    memset(ret, 0, size);
    if (arity && parameters) {
        memcpy(ret->parameters, parameters, psize);
    }
    ret->type = type;
    ret->bound = 0;
    return ret;
}


static inline void louro_free_parameters(LouroExpression *n) {
    if (!n) return;
    switch (TYPE_MASK(n->type)) {
        case LOURO_FUNCTION7: case LOURO_CLOSURE7: louro_free((LouroExpression*)n->parameters[6]);     /* Falls through. */
        case LOURO_FUNCTION6: case LOURO_CLOSURE6: louro_free((LouroExpression*)n->parameters[5]);     /* Falls through. */
        case LOURO_FUNCTION5: case LOURO_CLOSURE5: louro_free((LouroExpression*)n->parameters[4]);     /* Falls through. */
        case LOURO_FUNCTION4: case LOURO_CLOSURE4: louro_free((LouroExpression*)n->parameters[3]);     /* Falls through. */
        case LOURO_FUNCTION3: case LOURO_CLOSURE3: louro_free((LouroExpression*)n->parameters[2]);     /* Falls through. */
        case LOURO_FUNCTION2: case LOURO_CLOSURE2: louro_free((LouroExpression*)n->parameters[1]);     /* Falls through. */
        case LOURO_FUNCTION1: case LOURO_CLOSURE1: louro_free((LouroExpression*)n->parameters[0]);
    }
}


static inline void louro_free(LouroExpression *n) {
    if (!n) return;
    louro_free_parameters(n);
    free(n);
}


/* Built-in functions and their declarations for TinyExpr */



static inline const LouroVariable *find_lookup(const state *s, const char *name, int len) {
    int iters;
    const LouroVariable *var;
    if (!s->lookup) return 0;

    for (var = s->lookup, iters = s->lookup_len; iters; ++var, --iters) {
        if (strncmp(name, var->name, len) == 0 && var->name[len] == '\0') {
            return var;
        }
    }
    return 0;
}

static inline double lr_add(double a, double b) {return a + b;}
static inline double lr_sub(double a, double b) {return a - b;}
static inline double lr_mul(double a, double b) {return a * b;}
static inline double lr_divide(double a, double b) {return a / b;}
static inline double lr_negate(double a) {return -a;}
static inline double lr_comma(double a, double b) {(void)a; return b;}

/* Comparison operators — return 1.0 for true, 0.0 for false */
static inline double lr_cmp_lt(double a, double b)  {return a <  b ? 1.0 : 0.0;}
static inline double lr_cmp_gt(double a, double b)  {return a >  b ? 1.0 : 0.0;}
static inline double lr_cmp_le(double a, double b)  {return a <= b ? 1.0 : 0.0;}
static inline double lr_cmp_ge(double a, double b)  {return a >= b ? 1.0 : 0.0;}
static inline double lr_cmp_eq(double a, double b)  {return a == b ? 1.0 : 0.0;}
static inline double lr_cmp_ne(double a, double b)  {return a != b ? 1.0 : 0.0;}


static inline void next_token(state *s) {
    s->type = TOK_NULL;

    do {

        if (!*s->next){
            s->type = TOK_END;
            return;
        }

        /* Try reading a number. */
        if ((s->next[0] >= '0' && s->next[0] <= '9') || s->next[0] == '.') {
            s->value = strtod(s->next, (char**)&s->next);
            s->type = TOK_NUMBER;
        } else {
            /* Look for a builtin function call. */
            if (isalpha(s->next[0])) {
                const char *start;
                start = s->next;
                while (isalpha(s->next[0]) || isdigit(s->next[0]) || (s->next[0] == '_')) s->next++;

                const LouroVariable *var = find_lookup(s, start, s->next - start);

                if (!var) {
                    s->type = TOK_ERROR;
                } else {
                    switch(TYPE_MASK(var->type))
                    {
                        case LOURO_VARIABLE:
                            s->type = TOK_VARIABLE;
                            s->bound = (const double*)var->address;
                            break;

                        case LOURO_CLOSURE0: case LOURO_CLOSURE1: case LOURO_CLOSURE2: case LOURO_CLOSURE3:         /* Falls through. */
                        case LOURO_CLOSURE4: case LOURO_CLOSURE5: case LOURO_CLOSURE6: case LOURO_CLOSURE7:         /* Falls through. */
                            s->context = var->context;                                                  /* Falls through. */

                        case LOURO_FUNCTION0: case LOURO_FUNCTION1: case LOURO_FUNCTION2: case LOURO_FUNCTION3:     /* Falls through. */
                        case LOURO_FUNCTION4: case LOURO_FUNCTION5: case LOURO_FUNCTION6: case LOURO_FUNCTION7:     /* Falls through. */
                            s->type = var->type;
                            s->function = var->address;
                            break;
                    }
                }

            } else {
                /* Look for an operator or special character. */
                switch (s->next[0]) {
                    case '+': s->type = TOK_INFIX; s->function = (const void*)lr_add;    s->next++; break;
                    case '-': s->type = TOK_INFIX; s->function = (const void*)lr_sub;    s->next++; break;
                    case '*': s->type = TOK_INFIX; s->function = (const void*)lr_mul;    s->next++; break;
                    case '/': s->type = TOK_INFIX; s->function = (const void*)lr_divide; s->next++; break;
                    case '^': s->type = TOK_INFIX; s->function = (const void*)pow;    s->next++; break;
                    case '%': s->type = TOK_INFIX; s->function = (const void*)fmod;   s->next++; break;
                    case '(': s->type = TOK_OPEN;  s->next++; break;
                    case ')': s->type = TOK_CLOSE; s->next++; break;
                    case ',': s->type = TOK_SEP;   s->next++; break;
                    case '<':
                        if (s->next[1] == '=') {
                            s->type = TOK_INFIX; s->function = (const void*)lr_cmp_le; s->next += 2;
                        } else {
                            s->type = TOK_INFIX; s->function = (const void*)lr_cmp_lt; s->next++;
                        }
                        break;
                    case '>':
                        if (s->next[1] == '=') {
                            s->type = TOK_INFIX; s->function = (const void*)lr_cmp_ge; s->next += 2;
                        } else {
                            s->type = TOK_INFIX; s->function = (const void*)lr_cmp_gt; s->next++;
                        }
                        break;
                    case '=':
                        if (s->next[1] == '=') {
                            s->type = TOK_INFIX; s->function = (const void*)lr_cmp_eq; s->next += 2;
                        } else {
                            s->type = TOK_ERROR; s->next++;
                        }
                        break;
                    case '!':
                        if (s->next[1] == '=') {
                            s->type = TOK_INFIX; s->function = (const void*)lr_cmp_ne; s->next += 2;
                        } else {
                            s->type = TOK_ERROR; s->next++;
                        }
                        break;
                    case ' ': case '\t': case '\n': case '\r': s->next++; break;
                    default: s->type = TOK_ERROR; s->next++; break;
                }
            }
        }
    } while (s->type == TOK_NULL);
}


static inline LouroExpression *list(state *s);
static inline LouroExpression *expr(state *s);
static inline LouroExpression *power(state *s);

static inline LouroExpression *base(state *s) {
    /* <base>      =    <constant> | <variable> | <function-0> {"(" ")"} | <function-1> <power> | <function-X> "(" <expr> {"," <expr>} ")" | "(" <list> ")" */
    LouroExpression *ret;
    int arity;

    switch (TYPE_MASK(s->type)) {
        case TOK_NUMBER:
            ret = new_expr(LOURO_CONSTANT, 0);
            CHECK_NULL(ret);

            ret->value = s->value;
            next_token(s);
            break;

        case TOK_VARIABLE:
            ret = new_expr(LOURO_VARIABLE, 0);
            CHECK_NULL(ret);

            ret->bound = s->bound;
            next_token(s);
            break;

        case LOURO_FUNCTION0:
        case LOURO_CLOSURE0:
            ret = new_expr(s->type, 0);
            CHECK_NULL(ret);

            ret->function = s->function;
            if (IS_CLOSURE(s->type)) ret->parameters[0] = s->context;
            next_token(s);
            if (s->type == TOK_OPEN) {
                next_token(s);
                if (s->type != TOK_CLOSE) {
                    s->type = TOK_ERROR;
                } else {
                    next_token(s);
                }
            }
            break;

        case LOURO_FUNCTION1:
        case LOURO_CLOSURE1:
            ret = new_expr(s->type, 0);
            CHECK_NULL(ret);

            ret->function = s->function;
            if (IS_CLOSURE(s->type)) ret->parameters[1] = s->context;
            next_token(s);
            ret->parameters[0] = power(s);
            CHECK_NULL(ret->parameters[0], louro_free(ret));
            break;

        case LOURO_FUNCTION2: case LOURO_FUNCTION3: case LOURO_FUNCTION4:
        case LOURO_FUNCTION5: case LOURO_FUNCTION6: case LOURO_FUNCTION7:
        case LOURO_CLOSURE2: case LOURO_CLOSURE3: case LOURO_CLOSURE4:
        case LOURO_CLOSURE5: case LOURO_CLOSURE6: case LOURO_CLOSURE7:
            arity = ARITY(s->type);

            ret = new_expr(s->type, 0);
            CHECK_NULL(ret);

            ret->function = s->function;
            if (IS_CLOSURE(s->type)) ret->parameters[arity] = s->context;
            next_token(s);

            if (s->type != TOK_OPEN) {
                s->type = TOK_ERROR;
            } else {
                int i;
                for(i = 0; i < arity; i++) {
                    next_token(s);
                    ret->parameters[i] = expr(s);
                    CHECK_NULL(ret->parameters[i], louro_free(ret));

                    if(s->type != TOK_SEP) {
                        break;
                    }
                }
                if(s->type != TOK_CLOSE || i != arity - 1) {
                    s->type = TOK_ERROR;
                } else {
                    next_token(s);
                }
            }

            break;

        case TOK_OPEN:
            next_token(s);
            ret = list(s);
            CHECK_NULL(ret);

            if (s->type != TOK_CLOSE) {
                s->type = TOK_ERROR;
            } else {
                next_token(s);
            }
            break;

        default:
            ret = new_expr(0, 0);
            CHECK_NULL(ret);

            s->type = TOK_ERROR;
            ret->value = NAN;
            break;
    }

    return ret;
}


static inline LouroExpression *power(state *s) {
    /* <power>     =    {("-" | "+")} <base> */
    int sign = 1;
    while (s->type == TOK_INFIX && (s->function == lr_add || s->function == lr_sub)) {
        if (s->function == lr_sub) sign = -sign;
        next_token(s);
    }

    LouroExpression *ret;

    if (sign == 1) {
        ret = base(s);
    } else {
        LouroExpression *b = base(s);
        CHECK_NULL(b);

        ret = NEW_EXPR(LOURO_FUNCTION1 | LOURO_FLAG_PURE, b);
        CHECK_NULL(ret, louro_free(b));

        ret->function = (const void*)lr_negate;
    }

    return ret;
}

#ifdef LR_POW_FROM_RIGHT
static inline LouroExpression *factor(state *s) {
    /* <factor>    =    <power> {"^" <power>} */
    LouroExpression *ret = power(s);
    CHECK_NULL(ret);

    int neg = 0;

    if (ret->type == (LOURO_FUNCTION1 | LOURO_FLAG_PURE) && ret->function == lr_negate) {
        LouroExpression *se = ret->parameters[0];
        free(ret);
        ret = se;
        neg = 1;
    }

    LouroExpression *insertion = 0;

    while (s->type == TOK_INFIX && (s->function == pow)) {
        lr_fun2 t = (lr_fun2)s->function;
        next_token(s);

        if (insertion) {
            /* Make exponentiation go right-to-left. */
            LouroExpression *p = power(s);
            CHECK_NULL(p, louro_free(ret));

            LouroExpression *insert = NEW_EXPR(LOURO_FUNCTION2 | LOURO_FLAG_PURE, (LouroExpression*)insertion->parameters[1], p);
            CHECK_NULL(insert, louro_free(p), louro_free(ret));

            insert->function = (const void*)t;
            insertion->parameters[1] = insert;
            insertion = insert;
        } else {
            LouroExpression *p = power(s);
            CHECK_NULL(p, louro_free(ret));

            LouroExpression *prev = ret;
            ret = NEW_EXPR(LOURO_FUNCTION2 | LOURO_FLAG_PURE, ret, p);
            CHECK_NULL(ret, louro_free(p), louro_free(prev));

            ret->function = (const void*)t;
            insertion = ret;
        }
    }

    if (neg) {
        LouroExpression *prev = ret;
        ret = NEW_EXPR(LOURO_FUNCTION1 | LOURO_FLAG_PURE, ret);
        CHECK_NULL(ret, louro_free(prev));

        ret->function = (const void*)lr_negate;
    }

    return ret;
}
#else
static inline LouroExpression *factor(state *s) {
    /* <factor>    =    <power> {"^" <power>} */
    LouroExpression *ret = power(s);
    CHECK_NULL(ret);

    while (s->type == TOK_INFIX && (s->function == pow)) {
        lr_fun2 t = (lr_fun2)s->function;
        next_token(s);
        LouroExpression *p = power(s);
        CHECK_NULL(p, louro_free(ret));

        LouroExpression *prev = ret;
        ret = NEW_EXPR(LOURO_FUNCTION2 | LOURO_FLAG_PURE, ret, p);
        CHECK_NULL(ret, louro_free(p), louro_free(prev));

        ret->function = (const void*)t;
    }

    return ret;
}
#endif



static inline LouroExpression *term(state *s) {
    /* <term>      =    <factor> {("*" | "/" | "%") <factor>} */
    LouroExpression *ret = factor(s);
    CHECK_NULL(ret);

    while (s->type == TOK_INFIX && (s->function == lr_mul || s->function == lr_divide || s->function == fmod)) {
        lr_fun2 t = (lr_fun2)s->function;
        next_token(s);
        LouroExpression *f = factor(s);
        CHECK_NULL(f, louro_free(ret));

        LouroExpression *prev = ret;
        ret = NEW_EXPR(LOURO_FUNCTION2 | LOURO_FLAG_PURE, ret, f);
        CHECK_NULL(ret, louro_free(f), louro_free(prev));

        ret->function = (const void*)t;
    }

    return ret;
}


static inline LouroExpression *expr(state *s) {
    /* <expr>      =    <term> {("+" | "-") <term>} */
    LouroExpression *ret = term(s);
    CHECK_NULL(ret);

    while (s->type == TOK_INFIX && (s->function == lr_add || s->function == lr_sub)) {
        lr_fun2 t = (lr_fun2)s->function;
        next_token(s);
        LouroExpression *te = term(s);
        CHECK_NULL(te, louro_free(ret));

        LouroExpression *prev = ret;
        ret = NEW_EXPR(LOURO_FUNCTION2 | LOURO_FLAG_PURE, ret, te);
        CHECK_NULL(ret, louro_free(te), louro_free(prev));

        ret->function = (const void*)t;
    }

    return ret;
}


static inline LouroExpression *comparison(state *s) {
    /* <comparison> = <expr> {("<" | ">" | "<=" | ">=" | "==" | "!=") <expr>} */
    LouroExpression *ret = expr(s);
    CHECK_NULL(ret);

    while (s->type == TOK_INFIX && (
               s->function == lr_cmp_lt || s->function == lr_cmp_gt ||
               s->function == lr_cmp_le || s->function == lr_cmp_ge ||
               s->function == lr_cmp_eq || s->function == lr_cmp_ne)) {
        lr_fun2 t = (lr_fun2)s->function;
        next_token(s);
        LouroExpression *r = expr(s);
        CHECK_NULL(r, louro_free(ret));

        LouroExpression *prev = ret;
        ret = NEW_EXPR(LOURO_FUNCTION2 | LOURO_FLAG_PURE, ret, r);
        CHECK_NULL(ret, louro_free(r), louro_free(prev));

        ret->function = (const void*)t;
    }

    return ret;
}


static inline LouroExpression *list(state *s) {
    /* <list>      =    <comparison> {"," <comparison>} */
    LouroExpression *ret = comparison(s);
    CHECK_NULL(ret);

    while (s->type == TOK_SEP) {
        next_token(s);
        LouroExpression *e = comparison(s);
        CHECK_NULL(e, louro_free(ret));

        LouroExpression *prev = ret;
        ret = NEW_EXPR(LOURO_FUNCTION2 | LOURO_FLAG_PURE, ret, e);
        CHECK_NULL(ret, louro_free(e), louro_free(prev));

        ret->function = (const void*)lr_comma;
    }

    return ret;
}


#define LR_FUN(...) ((double(*)(__VA_ARGS__))n->function)
#define M(e) louro_evaluate((LouroExpression*)n->parameters[e])


static inline double louro_evaluate(const LouroExpression *n) {
    if (!n) return NAN;

    switch(TYPE_MASK(n->type)) {
        case LOURO_CONSTANT: return n->value;
        case LOURO_VARIABLE: return *n->bound;

        case LOURO_FUNCTION0: case LOURO_FUNCTION1: case LOURO_FUNCTION2: case LOURO_FUNCTION3:
        case LOURO_FUNCTION4: case LOURO_FUNCTION5: case LOURO_FUNCTION6: case LOURO_FUNCTION7:
            switch(ARITY(n->type)) {
                case 0: return LR_FUN(void)();
                case 1: return LR_FUN(double)(M(0));
                case 2: return LR_FUN(double, double)(M(0), M(1));
                case 3: return LR_FUN(double, double, double)(M(0), M(1), M(2));
                case 4: return LR_FUN(double, double, double, double)(M(0), M(1), M(2), M(3));
                case 5: return LR_FUN(double, double, double, double, double)(M(0), M(1), M(2), M(3), M(4));
                case 6: return LR_FUN(double, double, double, double, double, double)(M(0), M(1), M(2), M(3), M(4), M(5));
                case 7: return LR_FUN(double, double, double, double, double, double, double)(M(0), M(1), M(2), M(3), M(4), M(5), M(6));
                default: return NAN;
            }

        case LOURO_CLOSURE0: case LOURO_CLOSURE1: case LOURO_CLOSURE2: case LOURO_CLOSURE3:
        case LOURO_CLOSURE4: case LOURO_CLOSURE5: case LOURO_CLOSURE6: case LOURO_CLOSURE7:
            switch(ARITY(n->type)) {
                case 0: return LR_FUN(void*)(n->parameters[0]);
                case 1: return LR_FUN(void*, double)(n->parameters[1], M(0));
                case 2: return LR_FUN(void*, double, double)(n->parameters[2], M(0), M(1));
                case 3: return LR_FUN(void*, double, double, double)(n->parameters[3], M(0), M(1), M(2));
                case 4: return LR_FUN(void*, double, double, double, double)(n->parameters[4], M(0), M(1), M(2), M(3));
                case 5: return LR_FUN(void*, double, double, double, double, double)(n->parameters[5], M(0), M(1), M(2), M(3), M(4));
                case 6: return LR_FUN(void*, double, double, double, double, double, double)(n->parameters[6], M(0), M(1), M(2), M(3), M(4), M(5));
                case 7: return LR_FUN(void*, double, double, double, double, double, double, double)(n->parameters[7], M(0), M(1), M(2), M(3), M(4), M(5), M(6));
                default: return NAN;
            }

        default: return NAN;
    }

}

#undef LR_FUN
#undef M

static inline void optimize(LouroExpression *n) {
    /* Evaluates as much as possible. */
    if (n->type == LOURO_CONSTANT) return;
    if (n->type == LOURO_VARIABLE) return;

    /* Only optimize out functions flagged as pure. */
    if (IS_PURE(n->type)) {
        const int arity = ARITY(n->type);
        int known = 1;
        int i;
        for (i = 0; i < arity; ++i) {
            optimize((LouroExpression*)n->parameters[i]);
            if (((LouroExpression*)(n->parameters[i]))->type != LOURO_CONSTANT) {
                known = 0;
            }
        }
        if (known) {
            const double value = louro_evaluate(n);
            louro_free_parameters(n);
            n->type = LOURO_CONSTANT;
            n->value = value;
        }
    }
}


static inline LouroExpression *louro_compile(const char *expression, const LouroVariable *variables, int var_count, int *error) {
    state s;
    s.start = s.next = expression;
    s.context = 0;
    s.lookup = variables;
    s.lookup_len = var_count;

    next_token(&s);
    LouroExpression *root = list(&s);
    if (root == NULL) {
        if (error) *error = -1;
        return NULL;
    }

    if (s.type != TOK_END) {
        louro_free(root);
        if (error) {
            *error = (s.next - s.start);
            if (*error == 0) *error = 1;
        }
        return 0;
    } else {
        optimize(root);
        if (error) *error = 0;
        return root;
    }
}


static inline double louro_interpret(const char *expression, int *error) {
    LouroExpression *n = louro_compile(expression, 0, 0, error);

    double ret;
    if (n) {
        ret = louro_evaluate(n);
        louro_free(n);
    } else {
        ret = NAN;
    }
    return ret;
}



static inline void pn (const LouroExpression *n, int depth) {
    int i, arity;
    printf("%*s", depth, "");

    switch(TYPE_MASK(n->type)) {
    case LOURO_CONSTANT: printf("%f\n", n->value); break;
    case LOURO_VARIABLE: printf("bound %p\n", (void*)n->bound); break;

    case LOURO_FUNCTION0: case LOURO_FUNCTION1: case LOURO_FUNCTION2: case LOURO_FUNCTION3:
    case LOURO_FUNCTION4: case LOURO_FUNCTION5: case LOURO_FUNCTION6: case LOURO_FUNCTION7:
    case LOURO_CLOSURE0: case LOURO_CLOSURE1: case LOURO_CLOSURE2: case LOURO_CLOSURE3:
    case LOURO_CLOSURE4: case LOURO_CLOSURE5: case LOURO_CLOSURE6: case LOURO_CLOSURE7:
         arity = ARITY(n->type);
         printf("f%d", arity);
         for(i = 0; i < arity; i++) {
             printf(" %p", n->parameters[i]);
         }
         printf("\n");
         for(i = 0; i < arity; i++) {
             pn((LouroExpression*)n->parameters[i], depth + 1);
         }
         break;
    }
}


static inline void louro_print(const LouroExpression *n) {
    pn(n, 0);
}

#ifdef __cplusplus
}
#endif

#endif /*LOURO_H*/
