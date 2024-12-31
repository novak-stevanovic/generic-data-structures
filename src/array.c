#include "array.h"
#include "assert.h"
#include <stddef.h>
#include <string.h>

/* Initializes the fields curr_count, max_count, element_size - They are shared between funcs arr_init_static, arr_init_dynamic. */
void _arr_init_shared(struct Array* array, size_t max_count, size_t element_size);

/* Function returns address of element with index <pos>. Keep in mind that if <pos> == array->count, the function(unlike arr_at()) will work - it will return the address of location
 * where to append the next element 
 * Return value:
 * on success - address of existing element with index <pos> OR address of array[curr_count] element. 
 * on failure - NULL */
void* _arr_at_incl(const struct Array* array, size_t pos);

/* Shift elements starting from index <start_idx> rightward by calling memmove().
 * Return value:
 * on success: 0
 * on failure: 1 - memmove() failed. */
int _arr_shift_right(struct Array* array, size_t start_idx);

/* Shift elements starting from index <start_idx> leftward by calling memmove().
 * Return value:
 * on success: 0
 * on failure: 1 - memmove() failed. */
int _arr_shift_left(struct Array* array, size_t start_idx);

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int arr_init_static(struct Array* array, size_t max_count, size_t element_size, void* static_arr_ptr)
{
    ASSERT_NON_NULL_ARG(array, "array");
    ASSERT_NON_NULL_ARG(static_arr_ptr, "static_arr_ptr");

    _arr_init_shared(array, max_count, element_size);
    array->data = static_arr_ptr;
    array->alloc_type = ALLOC_TYPE_STATIC;

    return 0;
}

int arr_init_dynamic(struct Array* array, size_t max_count, size_t element_size)
{
    ASSERT_NON_NULL_ARG(array, "array");

    _arr_init_shared(array, max_count, element_size);
    array->data = malloc(max_count * element_size);
    if(array->data == NULL)
    {
        array->alloc_type = -1;
        return 1;
    }
    array->alloc_type = ALLOC_TYPE_DYNAMIC;

    return 0;
}

void arr_destruct(struct Array* array)
{
    ASSERT_NON_NULL_ARG(array, "array");

    if(array->alloc_type == ALLOC_TYPE_DYNAMIC) free(array->data);

    array->data = NULL;
    array->alloc_type = ALLOC_TYPE_UNALLOCED;
    array->max_count = 0;
    array->count = 0;
    array->element_size = 0;
}

int arr_assign(struct Array* array, size_t pos, const void* data)
{
    ASSERT_NON_NULL_ARG(array, "array");
    ASSERT(pos <= array->count, "Value of <pos> must be lesser or equal to array->curr_count");
    if(pos == array->count)
        ASSERT(array->count < array->max_count, "INVALID POS, ARRAY_COUNT, MAX_COUNT");

    void* addr = _arr_at_incl(array, pos);
    ASSERT_NON_NULL_VAL(addr, "addr");

    void* memcpy_addr = memcpy(addr, data, array->element_size);
    if(memcpy_addr == NULL) return 1;

    return 0;
}

int arr_append(struct Array* array, const void* data)
{
    return arr_insert(array, data, array->count);
}

int arr_insert(struct Array* array, const void* data, size_t pos)
{
    ASSERT_NON_NULL_ARG(array, "array");
    ASSERT_NON_NULL_ARG(data, "data");

    ASSERT_NON_NULL_VAL(array->data, "array->data");
    ASSERT(pos <= array->count, "Value of <pos> must be lesser or equal to array->curr_count");

    if(pos < array->count)
    {
        int shift_status = _arr_shift_right(array, pos);
        if(shift_status != 0) return 1;
    }

    int assign_status = arr_assign(array, pos, data);
    if(assign_status != 0) return 2;

    array->count++;

    return 0;

}

int arr_remove(struct Array* array, size_t pos)
{
    ASSERT_NON_NULL_ARG(array, "array");

    ASSERT_NON_NULL_VAL(array->data, "array->data");
    ASSERT(pos < array->count, "Value of <pos> must be lesser to array->curr_count");
    ASSERT(array->count > 0, "Value of array->count must be greater than 0");

    if(pos < (array->count - 1))
    {
        int shift_status = _arr_shift_left(array, pos + 1);
        if(shift_status != 0) return 1;
    }

    array->count--;

    return 0;
}

int arr_pop(struct Array* array)
{
    return arr_remove(array, array->count - 1);
}

void* arr_at(const struct Array* array, size_t pos)
{
    ASSERT_NON_NULL_ARG(array, "array");
    ASSERT(pos < array->count, "Argument <pos> must be of lesser value than array->curr_count");

    ASSERT_NON_NULL_VAL(array->data, "array->array");

    return _arr_at_incl(array, pos);
}

size_t arr_get_curr_count(const struct Array* array)
{
    return array->count;
}

size_t arr_get_max_count(const struct Array* array)
{
    return array->max_count;
}

void* arr_get_array(const struct Array* array)
{
    return array->data;
}

// -------------------------------------------------------------------------------------------------------------------------------------

void _arr_init_shared(struct Array* array, size_t max_count, size_t element_size)
{
    array->count = 0;
    array->max_count = max_count;
    array->element_size = element_size;
}

void* _arr_at_incl(const struct Array* array, size_t pos)
{
    ASSERT_NON_NULL_ARG(array, "array");
    ASSERT(pos <= array->count, "Argument <pos> must be of lesser or equal value than array->curr_count");

    ASSERT_NON_NULL_VAL(array->data, "array->data");

    return array->data + pos * array->element_size;
}

int _arr_shift_right(struct Array* array, size_t start_idx)
{
    ASSERT(start_idx < array->count, "Invalid start_idx parameter.\n");
    ASSERT_NON_NULL_ARG(array, "array");
    ASSERT_NON_NULL_VAL(array->data, "array->data");

    size_t array_count = array->count;

    size_t step = array->element_size;
    size_t elements_shifted = array_count - start_idx;

    void* start_pos = _arr_at_incl(array, start_idx);

    int memmove_status = (memmove(start_pos + step, start_pos, step * elements_shifted) == NULL);
    return memmove_status;
}

int _arr_shift_left(struct Array* array, size_t start_idx)
{
    ASSERT_NON_NULL_ARG(array, "array");
    ASSERT_NON_NULL_VAL(array->data, "array->data");

    size_t array_count = array->count;

    ASSERT(start_idx < array_count, "Invalid argument 'start_idx' passed.\n");

    size_t step = array->element_size;
    size_t elements_shifted = array_count - start_idx;

    void* start_pos = _arr_at_incl(array, start_idx) - step;

    return (memmove(start_pos, start_pos + step, step * elements_shifted) == NULL);
}

