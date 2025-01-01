#include "array.h"
#include "assert.h"
#include <stddef.h>
#include <string.h>

struct Array
{
    size_t count;
    size_t max_count;
    size_t element_size;
    int data_alloc_type;
    void* data;
};

/* Initializes the fields curr_count, max_count, element_size - They are shared between funcs arr_init_static, arr_init_dynamic. */
struct Array* _arr_init_shared(size_t max_count, size_t element_size);

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

struct Array* arr_init_static(size_t max_count, size_t element_size, void* static_arr_ptr)
{
    assert(static_arr_ptr != NULL);

    struct Array* array = _arr_init_shared(max_count, element_size);
    if(array == NULL) return NULL;

    array->data = static_arr_ptr;
    array->data_alloc_type = ALLOC_TYPE_STATIC;

    return array;
}

struct Array* arr_init_dynamic(size_t max_count, size_t element_size)
{
    struct Array* array = _arr_init_shared(max_count, element_size);

    if(array == NULL) return NULL;

    array->data = malloc(max_count * element_size);
    if(array->data == NULL)
    {
        free(array);
        return NULL;
    }
    array->data_alloc_type = ALLOC_TYPE_DYNAMIC;

    return array;
}

void arr_destruct(struct Array* array)
{
    assert(array != NULL);

    free(array->data);
    free(array);
}

int arr_assign(struct Array* array, size_t pos, const void* data)
{
    assert(array != NULL);
    assert(pos <= array->count);
    if(pos == array->count)
        assert(array->count < array->max_count);

    void* addr = _arr_at_incl(array, pos);
    assert(addr != NULL);

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
    assert(array != NULL);
    assert(data != NULL);

    assert(array->data != NULL);
    assert(pos <= array->count);

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
    assert(array != NULL);

    assert(array->data != NULL);
    assert(pos < array->count);
    assert(array->count > 0);

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
    assert(array != NULL);
    assert(pos < array->count);

    assert(array->data != NULL);

    return _arr_at_incl(array, pos);
}

size_t arr_get_count(const struct Array* array)
{
    assert(array != NULL);

    return array->count;
}

size_t arr_get_max_count(const struct Array* array)
{
    assert(array != NULL);

    return array->max_count;
}

void* arr_get_data(const struct Array* array)
{
    assert(array != NULL);

    return array->data;
}

size_t arr_get_struct_size()
{
    return sizeof(struct Array);
}

// -------------------------------------------------------------------------------------------------------------------------------------

struct Array* _arr_init_shared(size_t max_count, size_t element_size)
{
    assert(max_count > 0);
    struct Array* array = (struct Array*)malloc(sizeof(struct Array));

    if(array == NULL) return NULL;

    array->count = 0;
    array->max_count = max_count;
    array->element_size = element_size;

    return array;
}

void* _arr_at_incl(const struct Array* array, size_t pos)
{
    assert(array != NULL);
    assert(pos <= array->count);

    assert(array->data != NULL);

    return array->data + pos * array->element_size;
}

int _arr_shift_right(struct Array* array, size_t start_idx)
{
    assert(start_idx < array->count);
    assert(array != NULL);
    assert(array->data != NULL);

    size_t array_count = array->count;

    size_t step = array->element_size;
    size_t elements_shifted = array_count - start_idx;

    void* start_pos = _arr_at_incl(array, start_idx);

    int memmove_status = (memmove(start_pos + step, start_pos, step * elements_shifted) == NULL);
    return memmove_status;
}

int _arr_shift_left(struct Array* array, size_t start_idx)
{
    assert(array != NULL);
    assert(array->data != NULL);

    size_t array_count = array->count;

    assert(start_idx < array_count);

    size_t step = array->element_size;
    size_t elements_shifted = array_count - start_idx;

    void* start_pos = _arr_at_incl(array, start_idx) - step;

    return (memmove(start_pos, start_pos + step, step * elements_shifted) == NULL);
}

