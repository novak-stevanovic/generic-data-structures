#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "gds.h"
#include "gds_array.h"
#include "gds_misc.h"

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
#include "def/gds_array_def.h"
typedef struct GDSArray GDSArray;
#endif

// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------

/* Shift elements right of index(including) start_idx rightward by calling memmove(). This function cannot expand the vector if there is not enough memory allocated. Make sure that
 * vector->count < vector->alloced_count before calling. 
 * Return value:
 * on success: 0,
 * on failure: one of gds generic error codes */
static int _gds_arr_shift_right(GDSArray* array, size_t start_idx);

// --------------------------------------------------------------------------------------------------------------------------------------------

/* Shift elements right of(including) index start_idx leftward by calling memmove().
 * Return value:
 * on success: 0
 * on failure: one of gds generic error codes */
static int _gds_arr_shift_left(GDSArray* array, size_t start_idx);

// --------------------------------------------------------------------------------------------------------------------------------------------

/* Invokes array->on_element_removal_func(data) if the field is non-NULL. */
static void _gds_arr_on_element_removal(GDSArray* array, void* data);

// --------------------------------------------------------------------------------------------------------------------------------------------

/* Invokes _gds_arr_on_element_removal(GDSArray* array, void* data) for each element starting from start_pos(including)
 * , ending at end_pos (excluding). If start_pos == end_pos, no calls will be performed.
 * Return value:
 * on success: 0,
 * on failure: one of gds generic codes */
static int _gds_arr_on_element_removal_batch(GDSArray* array, size_t start_pos, size_t end_pos);

// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------

int gds_arr_init(GDSArray* array, size_t max_count, size_t element_size, void (*on_element_removal_func)(void*))
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(max_count == 0) return GDS_GEN_ERR_INVALID_ARG(2);
    if(element_size == 0) return GDS_GEN_ERR_INVALID_ARG(3);

    array->_max_count = max_count;
    array->_element_size = element_size;
    array->_on_element_removal_func = on_element_removal_func;
    array->_count = 0;

    array->_data = malloc(max_count * element_size);

    if(array->_data == NULL) return GDS_ARR_ERR_MALLOC_FAIL;

    return GDS_SUCCESS;
}

GDSArray* gds_arr_create(size_t max_count, size_t element_size, void (*on_element_removal_func)(void*))
{
    if((max_count == 0) || (element_size == 0)) return NULL;

    GDSArray* array = (GDSArray*)malloc(sizeof(GDSArray));
    if(array == NULL) return NULL;

    int init_status = gds_arr_init(array, max_count, element_size, on_element_removal_func);

    if(init_status == 0) return array;
    else
    {
        free(array);
        return NULL;
    }
}

void gds_arr_destruct(GDSArray* array)
{
    if(array == NULL) return;

    gds_arr_empty(array);

    free(array->_data);
    array->_count = 0;
    array->_element_size = 0;
    array->_max_count = 0;
}

void* gds_arr_at(const GDSArray* array, size_t pos)
{
    if(array == NULL) return NULL;
    if(pos >= array->_count) return NULL;

    return array->_data + pos * array->_element_size;
}

int gds_arr_assign(GDSArray* array, size_t pos, const void* data)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(pos >= array->_count) return GDS_GEN_ERR_INVALID_ARG(2);
    if(data == NULL) return GDS_GEN_ERR_INVALID_ARG(3);

    void* addr = gds_arr_at(array, pos);

    memcpy(addr, data, array->_element_size);

    return GDS_SUCCESS;
}

int gds_arr_append(GDSArray* array, const void* data)
{
    return gds_arr_insert(array, data, array->_count);
}

int gds_arr_insert(GDSArray* array, const void* data, size_t pos)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(data == NULL) return GDS_GEN_ERR_INVALID_ARG(2);
    if(pos > array->_count) return GDS_GEN_ERR_INVALID_ARG(3);
    if(array->_count == array->_max_count) return GDS_ARR_ERR_INSUFF_CAPACITY;

    if(pos < array->_count) _gds_arr_shift_right(array, pos);

    array->_count++; // it is important to increase the count first, so that the gsd_arr_assign() function will work.
    int assign_op_status = gds_arr_assign(array, pos, data);

    return GDS_SUCCESS;

}

int gds_arr_remove(GDSArray* array, size_t pos)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(pos >= array->_count) return GDS_GEN_ERR_INVALID_ARG(2);

    void* element_addr = gds_arr_at(array, pos);
    _gds_arr_on_element_removal(array, element_addr);

    if(pos < (array->_count - 1))
    {
        int shift_status = _gds_arr_shift_left(array, pos + 1);
    }

    array->_count--;

    return GDS_SUCCESS;
}

int gds_arr_pop(GDSArray* array)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(array->_count == 0) return GDS_ARR_ERR_ARR_EMPTY;

     gds_arr_remove(array, array->_count - 1);

     return GDS_SUCCESS;
}

int gds_arr_set_size(GDSArray* array, size_t new_count, void (*assign_func)(void*, void*), void* data)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(new_count > array->_max_count) return GDS_GEN_ERR_INVALID_ARG(2);

    size_t old_count = array->_count;

    if(old_count > new_count)
    {
        int batch_removal_status = _gds_arr_on_element_removal_batch(array, new_count, old_count);
        array->_count = new_count;
    }
    else if(old_count < new_count)
    {
        if(assign_func == NULL) return GDS_GEN_ERR_INVALID_ARG(3);

        int i;
        void* element_addr;
        array->_count = new_count;
        for(i = old_count; i < new_count; i++)
        {
            element_addr = gds_arr_at(array, i);
            assign_func(element_addr, data);
        }
    }

    return GDS_SUCCESS;
}

int gds_arr_empty(GDSArray* array)
{
    if(array == NULL) return 1;

    int set_size_status = gds_arr_set_size(array, 0, NULL, NULL);
    if(set_size_status != 0) return 2;

    return GDS_SUCCESS;
}

int gds_arr_realloc(GDSArray* array, size_t new_max_count)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(new_max_count == 0) return GDS_GEN_ERR_INVALID_ARG(2);

    array->_data = realloc(array->_data, new_max_count * array->_element_size);
    if(array->_data == NULL) 
    {
        array->_count = 0;
        array->_max_count = 0;
        return GDS_ARR_ERR_REALLOC_FAIL;
    }

    array->_max_count = new_max_count;
    array->_count = gds_misc_min(array->_count, array->_max_count);

    return GDS_SUCCESS;
}

ssize_t gds_arr_get_count(const GDSArray* array)
{
    if(array == NULL) return -1;

    return array->_count;
}

int gds_arr_is_empty(const GDSArray* array)
{
    if(array == NULL) return -1;

    return (array->_count == 0);
}

ssize_t gds_arr_get_max_count(const GDSArray* array)
{
    if(array == NULL) return -1;

    return array->_max_count;
}

void* gds_arr_get_data(const GDSArray* array)
{
    if(array == NULL) return NULL;

    return array->_data;
}

size_t gds_arr_get_element_size(const GDSArray* array)
{
    if(array == NULL) return GDS_SUCCESS;

    return array->_element_size;
}

size_t gds_arr_get_struct_size()
{
    return sizeof(GDSArray);
}

// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------

static int _gds_arr_shift_right(GDSArray* array, size_t start_idx)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(start_idx >= array->_count) return GDS_GEN_ERR_INVALID_ARG(2);

    size_t array_count = array->_count;

    size_t step = array->_element_size;
    size_t elements_shifted = array_count - start_idx;

    void* start_pos = gds_arr_at(array, start_idx);

    memmove(start_pos + step, start_pos, step * elements_shifted);
    return GDS_SUCCESS;
}

static int _gds_arr_shift_left(GDSArray* array, size_t start_idx)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(start_idx >= array->_count) return GDS_GEN_ERR_INVALID_ARG(2);

    size_t array_count = array->_count;

    size_t step = array->_element_size;
    size_t elements_shifted = array_count - start_idx;

    void* start_pos = gds_arr_at(array, start_idx) - step;

    memmove(start_pos, start_pos + step, step * elements_shifted);
    return GDS_SUCCESS;
}


static void _gds_arr_on_element_removal(GDSArray* array, void* data)
{
    if(array == NULL) return;
    if(array->_on_element_removal_func == NULL) return;

    array->_on_element_removal_func(data);
}

static int _gds_arr_on_element_removal_batch(GDSArray* array, size_t start_pos, size_t end_pos)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(start_pos > end_pos) return GDS_GEN_ERR_INCONSISTENT_ARGS;
    if((ssize_t)start_pos > (ssize_t)array->_count - 1) return GDS_GEN_ERR_INVALID_ARG(2);
    if(end_pos > array->_count) return GDS_GEN_ERR_INVALID_ARG(3);

    if(array->_on_element_removal_func == NULL) return GDS_SUCCESS;

    int i;
    void* curr_element;
    for(i = start_pos; i < end_pos; i++)
    {
        curr_element = gds_arr_at(array, i);
        _gds_arr_on_element_removal(array, curr_element);
    }

    return GDS_SUCCESS;
}
