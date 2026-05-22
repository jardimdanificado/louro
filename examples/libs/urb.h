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

static inline double urb_set(double _list, double index, double value)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    i = (i < 0) ? (list->size + i) : i;
    if (list->size <= 0 || i >= list->size || i < 0)
        return 0.0;
    list->data[index] = value;
    return _list;
}

static inline double urb_pop(double _list)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    if (list->size <= 0)
        return 0.0;
    return list->data[--list->size];
}

static inline double urb_shift(double _list)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    if (list->size <= 0)
        return 0.0;
    double ret = list->data[0];
    memmove(&(list->data[0]), &(list->data[1]), (size_t)(list->size - 1) * 8); 
    list->size--; 
    return ret;
}

static inline double urb_remove(double _list, double i)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    double original_index = i;
    
    // index cycle
    i = (i < 0) ? (list->size + i) : i;

    if (list->size <= 0)
        return 0.0;
    else if(i >= list->size || i < 0)
        return 0.0;
    
    double ret = list->data[(int)i];
    double elements_to_move = list->size - i - 1;
    memmove(&(list->data[(int)i]), &(list->data[(int)i + 1]), elements_to_move * 8); 
    list->size--; 
    return ret;
}

static inline double urb_get(double _list, double i)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    i = (i < 0) ? (list->size + i) : i;
    if (list->size <= 0 || i >= list->size || i < 0)
        return 0.0;
    return list->data[(int)i];
}

static inline double urb_len(double _list)
{
    Urb *list = (Urb*)(uintptr_t)_list;
    if(!list) return 0.0;
    return (double)list->size;
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
    LOURO_IMPURE("len", urb_len, 1)
    
#endif // ifndef URB_H macro