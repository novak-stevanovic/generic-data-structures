#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "gds.h"
#include "gds_array.h"
#include "gds_misc.h"

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
#define __GDS_ARRAY_DEF_ALLOW__
#include "def/gds_array_def.h"
#endif // GDS_ENABLE_OPAQUE_STRUCTS

// ------------------------------------------------------------------------------------------------------------------------------------------

/* Shift elements right of index(including) start_idx rightward by calling memmove().
 * This function cannot expand the array if there is not enough memory allocated. Make sure that
 * array->count < array->alloced_count before calling. 
 * Return value:
 * on success: 0,
 * on failure: one of gds generic error codes */
static gds_err _gds_array_shift_right(GDSArray* array, size_t start_idx);

// ---------------------------------------------------------------------------------------------------------------------

/* Shift elements right of(including) index start_idx leftward by calling memmove().
 * Return value:
 * on success: 0
 * on failure: one of gds generic error codes */
static gds_err _gds_array_shift_left(GDSArray* array, size_t start_idx);

// ---------------------------------------------------------------------------------------------------------------------

/* Invokes array->on_element_removal_func(data) if the field is non-NULL. */
static void _gds_array_on_element_removal(GDSArray* array, void* data);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_array_init(GDSArray* array, size_t capacity, size_t element_size, void (*on_element_removal_func)(void*))
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(capacity == 0) return GDS_GEN_ERR_INVALID_ARG(2);
    if(element_size == 0) return GDS_GEN_ERR_INVALID_ARG(3);

    array->_capacity = capacity;
    array->_element_size = element_size;
    array->_on_element_removal_func = on_element_removal_func;
    array->_count = 0;

    array->_data = malloc(capacity * element_size);

    if(array->_data == NULL) return GDS_ARR_ERR_MALLOC_FAIL;

    array->_swap_buff = malloc(element_size);

    if(array->_swap_buff == NULL)
    {
        free(array->_data);
        return GDS_ARR_ERR_MALLOC_FAIL;
    }

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

GDSArray* gds_array_create(size_t capacity, size_t element_size, void (*on_element_removal_func)(void*))
{
    if((capacity == 0) || (element_size == 0)) return NULL;

    GDSArray* array = (GDSArray*)malloc(sizeof(GDSArray));
    if(array == NULL) return NULL;

    gds_err init_status = gds_array_init(array, capacity, element_size, on_element_removal_func);

    if(init_status == GDS_SUCCESS) return array;
    else
    {
        free(array);
        return NULL;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void gds_array_destruct(GDSArray* array)
{
    if(array == NULL) return;

    gds_array_empty(array);
    free(array->_data);

    array->_count = 0;
    array->_capacity = 0;
    array->_element_size = 0;
    array->_on_element_removal_func = NULL;
    free(array->_swap_buff);
    array->_swap_buff = NULL;
}

// ---------------------------------------------------------------------------------------------------------------------

void* gds_array_at(const GDSArray* array, size_t pos)
{
    if(array == NULL) return NULL;
    if(pos >= array->_count) return NULL;

    return array->_data + pos * array->_element_size;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_array_assign(const GDSArray* array, const void* data, size_t pos)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(data == NULL) return GDS_GEN_ERR_INVALID_ARG(2);
    if(pos >= array->_count) return GDS_GEN_ERR_INVALID_ARG(3);

    void* addr = gds_array_at(array, pos);

    memcpy(addr, data, array->_element_size);

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_array_swap(const GDSArray* array, size_t pos1, size_t pos2)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(pos1 >= array->_count) return GDS_GEN_ERR_INVALID_ARG(2);
    if(pos2 >= array->_count) return GDS_GEN_ERR_INVALID_ARG(3);

    if(pos1 == pos2) return GDS_SUCCESS;

    void* pos1_data = gds_array_at(array, pos1);
    void* pos2_data = gds_array_at(array, pos2);

    size_t data_size = array->_element_size;

    memcpy(array->_swap_buff, pos1_data, data_size);
    memcpy(pos1_data, pos2_data, data_size);
    memcpy(pos2_data, array->_swap_buff, data_size);

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_array_push_back(GDSArray* array, const void* data)
{
    return gds_array_insert_at(array, data, array->_count);
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_array_insert_at(GDSArray* array, const void* data, size_t pos)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(data == NULL) return GDS_GEN_ERR_INVALID_ARG(2);
    if(pos > array->_count) return GDS_GEN_ERR_INVALID_ARG(3);
    if(array->_count == array->_capacity) return GDS_ARR_ERR_INSUFF_CAPACITY;

    if(pos < array->_count) _gds_array_shift_right(array, pos);

    array->_count++; // it is important to increase the count first, so that the gsd_array_assign() function will work.
    gds_err assign_op_status = gds_array_assign(array, data, pos);

    return GDS_SUCCESS;

}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_array_pop_back(GDSArray* array)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(array->_count == 0) return GDS_ARR_ERR_ARR_EMPTY;

     gds_array_remove_at(array, array->_count - 1);

     return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_array_remove_at(GDSArray* array, size_t pos)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(pos >= array->_count) return GDS_GEN_ERR_INVALID_ARG(2);

    void* element_addr = gds_array_at(array, pos);
    _gds_array_on_element_removal(array, element_addr);

    if(pos < (array->_count - 1)) _gds_array_shift_left(array, pos + 1);

    array->_count--;

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_array_empty(GDSArray* array)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);

    while(array->_count != 0) gds_array_pop_back(array);

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_array_realloc(GDSArray* array, size_t new_capacity)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(new_capacity == 0) return GDS_GEN_ERR_INVALID_ARG(2);

    if(new_capacity == array->_capacity) return GDS_SUCCESS;

    while(new_capacity < array->_count) gds_array_pop_back(array); // shrink the array.

    void* realloc_status = realloc(array->_data, new_capacity * array->_element_size);

    if(realloc_status == NULL) return GDS_ARR_ERR_REALLOC_FAIL;
    else array->_data = realloc_status;

    array->_capacity = new_capacity;
    array->_count = gds_misc_min(array->_count, array->_capacity);

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_array_get_count(const GDSArray* array)
{
    return (array != NULL) ? array->_element_size : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

bool gds_array_is_empty(const GDSArray* array)
{
    return (array != NULL) ? array->_element_size : true;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_array_get_capacity(const GDSArray* array)
{
    return (array != NULL) ? array->_element_size : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_array_get_element_size(const GDSArray* array)
{
    return (array != NULL) ? array->_element_size : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_array_get_struct_size()
{
    return sizeof(GDSArray);
}

// ------------------------------------------------------------------------------------------------------------------------------------------

static gds_err _gds_array_shift_right(GDSArray* array, size_t start_idx)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(start_idx >= array->_count) return GDS_GEN_ERR_INVALID_ARG(2);

    size_t array_count = array->_count;

    size_t step = array->_element_size;
    size_t elements_shifted = array_count - start_idx;

    void* start_pos = gds_array_at(array, start_idx);

    memmove(start_pos + step, start_pos, step * elements_shifted);
    return GDS_SUCCESS;
}

static gds_err _gds_array_shift_left(GDSArray* array, size_t start_idx)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(start_idx >= array->_count) return GDS_GEN_ERR_INVALID_ARG(2);

    size_t array_count = array->_count;

    size_t step = array->_element_size;
    size_t elements_shifted = array_count - start_idx;

    void* start_pos = gds_array_at(array, start_idx) - step;

    memmove(start_pos, start_pos + step, step * elements_shifted);
    return GDS_SUCCESS;
}


static void _gds_array_on_element_removal(GDSArray* array, void* data)
{
    if(array == NULL) return;
    if(array->_on_element_removal_func == NULL) return;

    array->_on_element_removal_func(data);
}
