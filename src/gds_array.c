#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "gds.h"
#include "gds_array.h"
#include "gds_misc.h"

#define _ARR_INTERNAL_ERR_BASE (ARR_ERR_BASE + 500)

#ifndef GDS_DISABLE_OPAQUE_STRUCTS
typedef struct GDSArray
{
    size_t _count;
    size_t _max_count;
    size_t _element_size;
    void* _data;
    void (*_on_element_removal_func)(void*);
} GDSArray;
#endif

// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------

#define _ARR_SHIFT_R_ERR_BASE (_ARR_INTERNAL_ERR_BASE + 10)
#define _ARR_SHIFT_R_ERR_ARR_NULL (_ARR_SHIFT_R_ERR_BASE + 1)
#define _ARR_SHIFT_R_ERR_START_IDX_OUT_OF_BOUNDS (_ARR_SHIFT_R_ERR_BASE + 2)
/* Shift elements right of index(including) start_idx rightward by calling memmove(). This function cannot expand the vector if there is not enough memory allocated. Make sure that
 * vector->count < vector->alloced_count before calling. 
 * Return value:
 * on success: 0
 * on failure - one of the error codes above. */
static int _gds_arr_shift_right(GDSArray* array, size_t start_idx);

// --------------------------------------------------------------------------------------------------------------------------------------------

#define _ARR_SHIFT_L_ERR_BASE (_ARR_INTERNAL_ERR_BASE + 20)
#define _ARR_SHIFT_L_ERR_ARR_NULL (_ARR_SHIFT_L_ERR_BASE + 1)
#define _ARR_SHIFT_L_ERR_START_IDX_OUT_OF_BOUNDS (_ARR_SHIFT_L_ERR_BASE + 2)

/* Shift elements right of(including) index start_idx leftward by calling memmove().
 * Return value:
 * on success: 0
 * on failure - one of the error codes above. */
static int _gds_arr_shift_left(GDSArray* array, size_t start_idx);

// --------------------------------------------------------------------------------------------------------------------------------------------

/* Invokes array->on_element_removal_func(data) if the field is non-NULL. */
static void _gds_arr_on_element_removal(GDSArray* array, void* data);

// --------------------------------------------------------------------------------------------------------------------------------------------

/* Invokes _gds_arr_on_element_removal(GDSArray* array, void* data) for each element starting from start_pos(including)
 * , ending at end_pos (excluding). If start_pos == end_pos, no calls will be performed.
 * Return value:
 * on success: 0,
 * on failure: 
 * 1 - argument array is NULL, 
 * 2 - start_pos > end_pos, 
 * 3 - array->count = 0,
 * 4 - start_pos > array->count - 1, 
 * 5 - end_pos > array->count */
static int _gds_arr_on_element_removal_batch(GDSArray* array, size_t start_pos, size_t end_pos);

// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------

GDSArray* gds_arr_create(size_t max_count, size_t element_size, void (*on_element_removal_func)(void*))
{
    if((max_count == 0) || (element_size == 0)) return NULL;

    GDSArray* array = (GDSArray*)malloc(sizeof(GDSArray));

    array->_max_count = max_count;
    array->_element_size = element_size;
    array->_on_element_removal_func = on_element_removal_func;
    array->_count = 0;

    if(array == NULL) return NULL;

    array->_data = malloc(max_count * element_size);

    if(array->_data == NULL)
    {
        free(array);
        return NULL;
    }

    return array;
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
    if(array == NULL) return ARR_ASSIGN_ERR_ARR_NULL;
    if(data == NULL) return ARR_ASSIGN_ERR_DATA_NULL;
    if(pos >= array->_count) return ARR_ASSIGN_ERR_POS_OUT_OF_BOUNDS;

    void* addr = gds_arr_at(array, pos);
    gds_rcheck(addr != NULL, _ARR_ASSIGN_ERR_INVALID_ADDR_FOUND);

    memcpy(addr, data, array->_element_size);

    return 0;
}

int gds_arr_append(GDSArray* array, const void* data)
{
    int insert_op_status = gds_arr_insert(array, data, array->_count);
    if(insert_op_status != 0)
        return insert_op_status - ARR_INSERT_ERR_BASE + ARR_APPEND_ERR_BASE; // convert to insert error code
    else
        return 0;
}

int gds_arr_insert(GDSArray* array, const void* data, size_t pos)
{
    if(array == NULL) return ARR_INSERT_ERR_ARR_NULL;
    if(data == NULL) return ARR_INSERT_ERR_DATA_NULL;
    if(array->_count == array->_max_count) return ARR_INSERT_ERR_INSUFF_SPACE;

    if(pos > array->_count) return ARR_INSERT_ERR_POS_OUT_OF_BOUNDS;

    if(pos < array->_count)
    {
        int shift_status = _gds_arr_shift_right(array, pos);
        gds_rcheck(shift_status == 0, _ARR_INSERT_FERR_SHIFTING_OP_FAILED);
    }

    array->_count++; // it is important to increase the count first, so that the gsd_arr_assign() function will work.

    int assign_op_status = gds_arr_assign(array, pos, data);
    gds_rcheck(assign_op_status == 0, _ARR_INSERT_FERR_ASSIGN_FAIL);

    return 0;

}

int gds_arr_remove(GDSArray* array, size_t pos)
{
    if(array == NULL) return ARR_REMOVE_ERR_ARR_NULL;
    if(pos >= array->_count) return ARR_REMOVE_ERR_POS_OUT_OF_BOUNDS;

    void* element_addr = gds_arr_at(array, pos);
    gds_rcheck(element_addr != NULL, _ARR_REMOVE_FERR_AT_FAIL);
    _gds_arr_on_element_removal(array, element_addr);

    if(pos < (array->_count - 1))
    {
        int shift_status = _gds_arr_shift_left(array, pos + 1);
        gds_rcheck(shift_status == 0, _ARR_REMOVE_FERR_AT_FAIL);
    }

    array->_count--;

    return 0;
}

int gds_arr_pop(GDSArray* array)
{
    if(array == NULL) return ARR_POP_ERR_ARR_NULL;
    if(array->_count == 0) return ARR_POP_ERR_ARR_EMPTY;

    int remove_op_status =  gds_arr_remove(array, array->_count - 1);
    if(remove_op_status != 0)
        return remove_op_status - ARR_REMOVE_ERR_BASE + ARR_POP_ERR_BASE; // convert to pop error code
    else
        return 0;
}

int gds_arr_set_size(GDSArray* array, size_t new_count, void (*assign_func)(void*, void*), void* data)
{
    if(array == NULL) return ARR_SET_SIZE_ERR_NULL_ARR;
    if(new_count > array->_max_count) return ARR_SET_SIZE_ERR_INVALID_NEW_COUNT_ARG;
    
    size_t old_count = array->_count;

    if(old_count > new_count)
    {
        int batch_removal_status = _gds_arr_on_element_removal_batch(array, new_count, old_count);
        gds_rcheck(batch_removal_status == 0, _ARR_SET_SIZE_ERR_ON_BATCH_REMOVAL_FAIL);
        array->_count = new_count;
    }
    else if(old_count < new_count)
    {
        if(assign_func == NULL) return ARR_SET_SIZE_ERR_NULL_ASSIGN_FUNC;

        int i;
        void* element_addr;
        array->_count = new_count;
        for(i = old_count; i < new_count; i++)
        {
            element_addr = gds_arr_at(array, i);
            assign_func(element_addr, data);
            gds_rcheck(element_addr != NULL, _ARR_SET_SIZE_FERR_AT_FAIL);
        }
    }

    return 0;
}

int gds_arr_empty(GDSArray* array)
{
    if(array == NULL) return 1;

    int set_size_status = gds_arr_set_size(array, 0, NULL, NULL);
    if(set_size_status != 0) return 2;

    return 0;
}

int gds_arr_realloc(GDSArray* array, size_t new_max_count)
{
    if(array == NULL) return ARR_REALLOC_ERR_ARR_NULL;
    if(new_max_count == 0) return ARR_REALLOC_ERR_NEW_COUNT_EQ_ZERO;

    array->_data = realloc(array->_data, new_max_count * array->_element_size);
    if(array->_data == NULL) 
    {
        array->_count = 0;
        array->_max_count = 0;
        return ARR_REALLOC_ERR_REALLOC_FAIL;
    }

    array->_max_count = new_max_count;
    array->_count = gds_misc_min(array->_count, array->_max_count);

    return 0;
}

ssize_t gds_arr_get_count(const GDSArray* array)
{
    if(array == NULL) return -1;

    return array->_count;
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
    if(array == NULL) return 0;

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
    if(array == NULL) return _ARR_SHIFT_R_ERR_ARR_NULL;
    if(start_idx >= array->_count) return _ARR_SHIFT_R_ERR_START_IDX_OUT_OF_BOUNDS;

    size_t array_count = array->_count;

    size_t step = array->_element_size;
    size_t elements_shifted = array_count - start_idx;

    void* start_pos = gds_arr_at(array, start_idx);

    memmove(start_pos + step, start_pos, step * elements_shifted);
    return 0;
}

static int _gds_arr_shift_left(GDSArray* array, size_t start_idx)
{
    if(array == NULL) return _ARR_SHIFT_L_ERR_ARR_NULL;
    if(start_idx >= array->_count) return _ARR_SHIFT_L_ERR_START_IDX_OUT_OF_BOUNDS;

    size_t array_count = array->_count;

    size_t step = array->_element_size;
    size_t elements_shifted = array_count - start_idx;

    void* start_pos = gds_arr_at(array, start_idx) - step;

    memmove(start_pos, start_pos + step, step * elements_shifted);
    return 0;
}


static void _gds_arr_on_element_removal(GDSArray* array, void* data)
{
    if(array == NULL) return;
    if(array->_on_element_removal_func == NULL) return;

    array->_on_element_removal_func(data);
}

static int _gds_arr_on_element_removal_batch(GDSArray* array, size_t start_pos, size_t end_pos)
{
    if(array == NULL) return 1;
    if(start_pos > end_pos) return 2;
    if(array->_count == 0) return 3;
    if(start_pos > array->_count - 1) return 4;
    if(end_pos > array->_count) return 5;

    if(array->_on_element_removal_func == NULL) return 0;

    int i;
    void* curr_element;
    for(i = start_pos; i < end_pos; i++)
    {
        curr_element = gds_arr_at(array, i);
        _gds_arr_on_element_removal(array, curr_element);
    }

    return 0;
}
