// urb 0.9.5

#ifndef URB_H
#define URB_H 1

// standard library
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// version
#define URB_VERSION "0.9.5"

typedef struct Urb
{
    // the real size
    uint32_t capacity;
    // the current size
    uint32_t size;
    // the data, it is a pointer to a double array
    double *data;
} Urb;

static inline double urb_new(double size)
{
    if (size < 0)
        return 0.0;
    Urb *list = (Urb*)malloc(sizeof(Urb));
    list->data = (size == 0) ? NULL : (double*)malloc((size_t)size * 8);
    list->size = 0;
    list->capacity = size;

    return (double)(uintptr_t)list;
}

static inline double urb_free(double _list)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    free(list->data);
    free(list);
    return 0.0;
}

static inline double urb_double(double _list)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    list->capacity = list->capacity == 0 ? 1 : list->capacity * 2;
    double *new_data = (double*)realloc(list->data, (size_t)list->capacity * 8);
    list->data = new_data;
    return 0.0;
}

static inline double urb_half(double _list)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    list->capacity /= 2;
    double *new_data = (double*)realloc(list->data, (size_t)list->capacity * 8);
    list->data = new_data;

    if (list->size > list->capacity)
        list->size = list->capacity;
    return 0.0;
}

static inline double urb_push(double _list, double value)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    if (list->size == list->capacity)
        urb_double(_list);
    list->data[list->size] = value;
    list->size++;
    return _list;
}

static inline double urb_unshift(double _list, double value)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    if (list->size == list->capacity)
        urb_double(_list);
    memmove(&(list->data[1]), &(list->data[0]), (size_t)list->size * 8);
    list->data[0] = value;

    list->size++;
    return _list;
}

static inline double urb_insert(double _list, double index, double value)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double original_index = index;
    if (list->size == list->capacity)
        urb_double(_list);
    
        // index cycle
    index = (index < 0) ? (list->size + index) : index;

    if(index > list->size || index < 0)
        return _list;
    
    memmove(&(list->data[(int)index + 1]), &(list->data[(int)index]), (size_t)(list->size - (int)index) * 8);
    list->data[(int)index] = value;
    list->size++;
    return _list;
}

static inline double urb_set(double _list, double i, double value)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    i = (i < 0) ? (list->size + i) : i;
    if (list->size <= 0 || i >= list->size || i < 0)
        return 0.0;
    list->data[(int)i] = value;
    return _list;
}

static inline double urb_get_i8(double _list, double i)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double max_elems = list->size * (sizeof(double) / sizeof(int8_t));
    i = (i < 0) ? (max_elems + i) : i;
    if (list->size <= 0 || i >= max_elems || i < 0)
        return 0.0;
    return (double)(((int8_t*)list->data)[(int)i]);
}

static inline double urb_set_i8(double _list, double i, double value)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double max_elems = list->size * (sizeof(double) / sizeof(int8_t));
    i = (i < 0) ? (max_elems + i) : i;
    if (list->size <= 0 || i >= max_elems || i < 0)
        return 0.0;
    ((int8_t*)list->data)[(int)i] = (int8_t)value;
    return _list;
}

static inline double urb_get_u8(double _list, double i)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double max_elems = list->size * (sizeof(double) / sizeof(uint8_t));
    i = (i < 0) ? (max_elems + i) : i;
    if (list->size <= 0 || i >= max_elems || i < 0)
        return 0.0;
    return (double)(((uint8_t*)list->data)[(int)i]);
}

static inline double urb_set_u8(double _list, double i, double value)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double max_elems = list->size * (sizeof(double) / sizeof(uint8_t));
    i = (i < 0) ? (max_elems + i) : i;
    if (list->size <= 0 || i >= max_elems || i < 0)
        return 0.0;
    ((uint8_t*)list->data)[(int)i] = (uint8_t)value;
    return _list;
}

static inline double urb_get_i16(double _list, double i)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double max_elems = list->size * (sizeof(double) / sizeof(int16_t));
    i = (i < 0) ? (max_elems + i) : i;
    if (list->size <= 0 || i >= max_elems || i < 0)
        return 0.0;
    return (double)(((int16_t*)list->data)[(int)i]);
}

static inline double urb_set_i16(double _list, double i, double value)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double max_elems = list->size * (sizeof(double) / sizeof(int16_t));
    i = (i < 0) ? (max_elems + i) : i;
    if (list->size <= 0 || i >= max_elems || i < 0)
        return 0.0;
    ((int16_t*)list->data)[(int)i] = (int16_t)value;
    return _list;
}

static inline double urb_get_u16(double _list, double i)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double max_elems = list->size * (sizeof(double) / sizeof(uint16_t));
    i = (i < 0) ? (max_elems + i) : i;
    if (list->size <= 0 || i >= max_elems || i < 0)
        return 0.0;
    return (double)(((uint16_t*)list->data)[(int)i]);
}

static inline double urb_set_u16(double _list, double i, double value)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double max_elems = list->size * (sizeof(double) / sizeof(uint16_t));
    i = (i < 0) ? (max_elems + i) : i;
    if (list->size <= 0 || i >= max_elems || i < 0)
        return 0.0;
    ((uint16_t*)list->data)[(int)i] = (uint16_t)value;
    return _list;
}

static inline double urb_get_i32(double _list, double i)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double max_elems = list->size * (sizeof(double) / sizeof(int32_t));
    i = (i < 0) ? (max_elems + i) : i;
    if (list->size <= 0 || i >= max_elems || i < 0)
        return 0.0;
    return (double)(((int32_t*)list->data)[(int)i]);
}

static inline double urb_set_i32(double _list, double i, double value)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double max_elems = list->size * (sizeof(double) / sizeof(int32_t));
    i = (i < 0) ? (max_elems + i) : i;
    if (list->size <= 0 || i >= max_elems || i < 0)
        return 0.0;
    ((int32_t*)list->data)[(int)i] = (int32_t)value;
    return _list;
}

static inline double urb_get_u32(double _list, double i)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double max_elems = list->size * (sizeof(double) / sizeof(uint32_t));
    i = (i < 0) ? (max_elems + i) : i;
    if (list->size <= 0 || i >= max_elems || i < 0)
        return 0.0;
    return (double)(((uint32_t*)list->data)[(int)i]);
}

static inline double urb_set_u32(double _list, double i, double value)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double max_elems = list->size * (sizeof(double) / sizeof(uint32_t));
    i = (i < 0) ? (max_elems + i) : i;
    if (list->size <= 0 || i >= max_elems || i < 0)
        return 0.0;
    ((uint32_t*)list->data)[(int)i] = (uint32_t)value;
    return _list;
}

static inline double urb_get_i64(double _list, double i)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double max_elems = list->size * (sizeof(double) / sizeof(int64_t));
    i = (i < 0) ? (max_elems + i) : i;
    if (list->size <= 0 || i >= max_elems || i < 0)
        return 0.0;
    return (double)(((int64_t*)list->data)[(int)i]);
}

static inline double urb_set_i64(double _list, double i, double value)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double max_elems = list->size * (sizeof(double) / sizeof(int64_t));
    i = (i < 0) ? (max_elems + i) : i;
    if (list->size <= 0 || i >= max_elems || i < 0)
        return 0.0;
    ((int64_t*)list->data)[(int)i] = (int64_t)value;
    return _list;
}

static inline double urb_get_u64(double _list, double i)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double max_elems = list->size * (sizeof(double) / sizeof(uint64_t));
    i = (i < 0) ? (max_elems + i) : i;
    if (list->size <= 0 || i >= max_elems || i < 0)
        return 0.0;
    return (double)(((uint64_t*)list->data)[(int)i]);
}

static inline double urb_set_u64(double _list, double i, double value)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double max_elems = list->size * (sizeof(double) / sizeof(uint64_t));
    i = (i < 0) ? (max_elems + i) : i;
    if (list->size <= 0 || i >= max_elems || i < 0)
        return 0.0;
    ((uint64_t*)list->data)[(int)i] = (uint64_t)value;
    return _list;
}

static inline double urb_get_f32(double _list, double i)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double max_elems = list->size * (sizeof(double) / sizeof(float));
    i = (i < 0) ? (max_elems + i) : i;
    if (list->size <= 0 || i >= max_elems || i < 0)
        return 0.0;
    return (double)(((float*)list->data)[(int)i]);
}

static inline double urb_set_f32(double _list, double i, double value)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double max_elems = list->size * (sizeof(double) / sizeof(float));
    i = (i < 0) ? (max_elems + i) : i;
    if (list->size <= 0 || i >= max_elems || i < 0)
        return 0.0;
    ((float*)list->data)[(int)i] = (float)value;
    return _list;
}

static inline double urb_get_f64(double _list, double i)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double max_elems = list->size * (sizeof(double) / sizeof(double));
    i = (i < 0) ? (max_elems + i) : i;
    if (list->size <= 0 || i >= max_elems || i < 0)
        return 0.0;
    return (double)(((double*)list->data)[(int)i]);
}

static inline double urb_set_f64(double _list, double i, double value)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double max_elems = list->size * (sizeof(double) / sizeof(double));
    i = (i < 0) ? (max_elems + i) : i;
    if (list->size <= 0 || i >= max_elems || i < 0)
        return 0.0;
    ((double*)list->data)[(int)i] = (double)value;
    return _list;
}

#define LOURO_URB \
    LOURO_IMPURE("new", urb_new, 1), \
    LOURO_IMPURE("free", urb_free, 1), \
    LOURO_IMPURE("double", urb_double, 1), \
    LOURO_IMPURE("half", urb_half, 1), \
    LOURO_IMPURE("push", urb_push, 2), \
    LOURO_IMPURE("unshift", urb_unshift, 2), \
    LOURO_IMPURE("insert", urb_insert, 3), \
    LOURO_IMPURE("set", urb_set, 3), \
    LOURO_IMPURE("pop", urb_pop, 1), \
    LOURO_IMPURE("shift", urb_shift, 1), \
    LOURO_IMPURE("remove", urb_remove, 2), \
    LOURO_IMPURE("get", urb_get, 2), \
    LOURO_IMPURE("len", urb_len, 1), \
    LOURO_IMPURE("get_i8", urb_get_i8, 2), \
    LOURO_IMPURE("set_i8", urb_set_i8, 3), \
    LOURO_IMPURE("get_u8", urb_get_u8, 2), \
    LOURO_IMPURE("set_u8", urb_set_u8, 3), \
    LOURO_IMPURE("get_i16", urb_get_i16, 2), \
    LOURO_IMPURE("set_i16", urb_set_i16, 3), \
    LOURO_IMPURE("get_u16", urb_get_u16, 2), \
    LOURO_IMPURE("set_u16", urb_set_u16, 3), \
    LOURO_IMPURE("get_i32", urb_get_i32, 2), \
    LOURO_IMPURE("set_i32", urb_set_i32, 3), \
    LOURO_IMPURE("get_u32", urb_get_u32, 2), \
    LOURO_IMPURE("set_u32", urb_set_u32, 3), \
    LOURO_IMPURE("get_i64", urb_get_i64, 2), \
    LOURO_IMPURE("set_i64", urb_set_i64, 3), \
    LOURO_IMPURE("get_u64", urb_get_u64, 2), \
    LOURO_IMPURE("set_u64", urb_set_u64, 3), \
    LOURO_IMPURE("get_f32", urb_get_f32, 2), \
    LOURO_IMPURE("set_f32", urb_set_f32, 3), \
    LOURO_IMPURE("get_f64", urb_get_f64, 2), \
    LOURO_IMPURE("set_f64", urb_set_f64, 3)

#endif // ifndef URB_H macro

