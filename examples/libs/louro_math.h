#ifndef LOURO_MATH_H
#define LOURO_MATH_H

#include "../../louro.h"
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline double lr_pi(void) { return 3.14159265358979323846; }

#define LOURO_MATH \
    LOURO_PURE("sqrt", sqrt), \
    LOURO_PURE("sin", sin), \
    LOURO_PURE("cos", cos), \
    LOURO_PURE("tan", tan), \
    LOURO_PURE("asin", asin), \
    LOURO_PURE("acos", acos), \
    LOURO_PURE("atan", atan), \
    LOURO_PURE("atan2", atan2), \
    LOURO_PURE("abs", fabs), \
    LOURO_PURE("fabs", fabs), \
    LOURO_PURE("log", log), \
    LOURO_PURE("log10", log10), \
    LOURO_PURE("exp", exp), \
    LOURO_PURE("ceil", ceil), \
    LOURO_PURE("floor", floor), \
    LOURO_PURE("pi", lr_pi)

#ifdef __cplusplus
}
#endif

#endif // LOURO_MATH_H
