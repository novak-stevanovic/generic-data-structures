#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "gds_array.h"
#include "gds_misc.h"

#define _ARR_INTERNAL_ERR_BASE (ARR_ERR_BASE + 500)

struct GDSArray
{
    size_t count;
    size_t max_count;
    size_t element_size;
    void* data;
    void (*on_element_removal_func)(void*);
};

// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------

#define _ARR_SHIFT_R_ERR_BASE (_ARR_INTERNAL_ERR_BASE + 10)
#define _ARR_SHIFT_R_ERR_ARR_NULL (_ARR_SHIFT_R_ERR_BASE + 1)
#define _ARR_SHIFT_R_ERR_START_IDX_OUT_OF_BOUNDS (_ARR_SHIFT_R_ERR_BASE + 2)
#define _ARR_SHIFT_R_ERR_INSUFF_SPACE (_ARR_SHIFT_R_ERR_BASE + 3) // Happens when array->count == array->max_count. This means there is not enough memory alloced to shift
                                                                  // any elements rightwards.
/* Shift elements right of index(including) start_idx rightward by calling memmove(). This function cannot expand the vector if there is not enough memory allocated. Make sure that
 * vector->count < vector->alloced_count before calling. 
 * Return value:
 * on success: 0
 * on failure - one of the error codes above. */
static int _gds_arr_shift_right(struct GDSArray* array, size_t start_idx);

// --------------------------------------------------------------------------------------------------------------------------------------------

#define _ARR_SHIFT_L_ERR_BASE (_ARR_INTERNAL_ERR_BASE + 20)
#define _ARR_SHIFT_L_ERR_ARR_NULL (_ARR_SHIFT_L_ERR_BASE + 1)
#define _ARR_SHIFT_L_ERR_START_IDX_OUT_OF_BOUNDS (_ARR_SHIFT_L_ERR_BASE + 2)

/* Shift elements right of(including) index start_idx leftward by calling memmove().
 * Return value:
 * on success: 0
 * on failure - one of the error codes above. */
static int _gds_arr_shift_left(struct GDSArray* array, size_t start_idx);

// --------------------------------------------------------------------------------------------------------------------------------------------

/* Invokes array->on_element_removal_func(data) if the field is non-NULL. */
static void _gds_arr_on_element_removal(struct GDSArray* array, void* data);

// --------------------------------------------------------------------------------------------------------------------------------------------

/* Invokes _gds_arr_on_element_removal(struct GDSArray* array, void* data) for each element starting from start_pos(including)
 * , ending at end_pos (excluding). If start_pos == end_pos, no calls will be performed.
 * Return value:
 * on success: 0,
 * on failure: 
 * 1 - argument array is NULL, 
 * 2 - start_pos > end_pos, 
 * 3 - array->count = 0,
 * 4 - start_pos > array->count - 1, 
 * 5 - end_pos > array->count
 * 6 - gds_arr_at() returned NULL. */
static int _gds_arr_on_element_removal_batch(struct GDSArray* array, size_t start_pos, size_t end_pos);

// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------

struct GDSArray* gds_arr_create(size_t max_count, size_t element_size, void (*on_element_removal_func)(void*))
{
    if((max_count == 0) || (element_size == 0)) return NULL;

    struct GDSArray* array = (struct GDSArray*)malloc(sizeof(struct GDSArray));

    array->max_count = max_count;
    array->element_size = element_size;
    array->on_element_removal_func = on_element_removal_func;
    array->count = 0;

    if(array == NULL) return NULL;

    array->data = malloc(max_count * element_size);

    if(array->data == NULL)
    {
        free(array);
        return NULL;
    }

    return array;
}

void gds_arr_destruct(struct GDSArray* array)
{
    if(array == NULL) return;

    gds_arr_empty(array);

    free(array->data);
    array->count = 0;
    array->element_size = 0;
    array->max_count = 0;
}

void* gds_arr_at(const struct GDSArray* array, size_t pos)
{
    if(array == NULL) return NULL;
    if(pos >= array->count) return NULL;

    return array->data + pos * array->element_size;
}

int gds_arr_assign(struct GDSArray* array, size_t pos, const void* data)
{
    if(array == NULL) return ARR_ASSIGN_ERR_ARR_NULL;
    if(data == NULL) return ARR_ASSIGN_ERR_DATA_NULL;
    if(pos >= array->count) return ARR_ASSIGN_ERR_POS_OUT_OF_BOUNDS;

    void* addr = gds_arr_at(array, pos);
    if(addr == NULL) return ARR_ASSIGN_ERR_INVALID_ADDR_FOUND;

    memcpy(addr, data, array->element_size);

    return 0;
}

int gds_arr_append(struct GDSArray* array, const void* data)
{
    int insert_op_status = gds_arr_insert(array, data, array->count);
    if(insert_op_status != 0)
        return insert_op_status - ARR_INSERT_ERR_BASE + ARR_APPEND_ERR_BASE; // convert to insert error code
    else
        return 0;
}

int gds_arr_insert(struct GDSArray* array, const void* data, size_t pos)
{
    if(array == NULL) return ARR_INSERT_ERR_ARR_NULL;
    if(data == NULL) return ARR_INSERT_ERR_DATA_NULL;

    if(pos > array->count) return ARR_INSERT_ERR_POS_OUT_OF_BOUNDS;

    if(pos < array->count)
    {
        int shift_status = _gds_arr_shift_right(array, pos);
        if(shift_status != 0) return ARR_INSERT_ERR_SHIFTING_OP_FAILED;
    }

    array->count++; // it is important to increase the count first, so that the gsd_arr_assign() function will work.

    int assign_op_status = gds_arr_assign(array, pos, data);
    if(assign_op_status != 0) 
    {
        array->count--;
        return ARR_INSERT_ERR_ASSIGN_FAIL;
    }

    return 0;

}

int gds_arr_remove(struct GDSArray* array, size_t pos)
{
    if(array == NULL) return ARR_REMOVE_ERR_ARR_NULL;
    if(pos >= array->count) return ARR_REMOVE_ERR_POS_OUT_OF_BOUNDS;

    void* element_addr = gds_arr_at(array, pos);
    if(element_addr == NULL) return ARR_REMOVE_ERR_AT_FAIL;
    _gds_arr_on_element_removal(array, element_addr);

    if(pos < (array->count - 1))
    {
        int shift_status = _gds_arr_shift_left(array, pos + 1);
        if(shift_status != 0) return ARR_REMOVE_ERR_SHIFTING_OP_FAILED;
    }

    array->count--;

    return 0;
}

int gds_arr_pop(struct GDSArray* array)
{
    if(array == NULL) return ARR_POP_ERR_ARR_NULL;
    if(array->count == 0) return ARR_POP_ERR_ARR_EMPTY;

    int remove_op_status =  gds_arr_remove(array, array->count - 1);
    if(remove_op_status != 0)
        return remove_op_status - ARR_REMOVE_ERR_BASE + ARR_POP_ERR_BASE; // convert to pop error code
    else
        return 0;
}

int gds_arr_set_size_val(struct GDSArray* array, size_t new_count, void* default_val)
{
    if(array == NULL) return ARR_SET_SIZE_VAL_NULL_ARR;
    if(new_count > array->max_count) return ARR_SET_SIZE_VAL_INVALID_NEW_COUNT_ARG;
    
    size_t old_count = array->count;

    if(old_count > new_count)
    {
        int batch_removal_status = _gds_arr_on_element_removal_batch(array, new_count, old_count);
        if(batch_removal_status != 0) return ARR_SET_SIZE_VAL_ON_BATCH_REMOVAL_FAIL;
        array->count = new_count;
    }
    else if(old_count < new_count)
    {
        if(default_val == NULL) return ARR_SET_SIZE_VAL_NULL_DEFAULT_VAL;

        int i, assign_status;
        for(i = old_count; i < new_count; i++)
        {
            assign_status = gds_arr_assign(array, old_count, default_val);
            if(assign_status != 0) return ARR_SET_SIZE_VAL_ASSIGN_FAIL;
        }
    }

    return 0;
}

int gds_arr_set_size_gen(struct GDSArray* array, size_t new_count, void* (*el_gen_func)(void* data), void* data)
{
    if(array == NULL) return ARR_SET_SIZE_GEN_NULL_ARR;
    if(new_count > array->max_count) return ARR_SET_SIZE_GEN_INVALID_NEW_COUNT_ARG;
    
    size_t old_count = array->count;

    if(old_count > new_count)
    {
        int batch_removal_status = _gds_arr_on_element_removal_batch(array, new_count, old_count);
        if(batch_removal_status != 0) return ARR_SET_SIZE_GEN_ON_BATCH_REMOVAL_FAIL;
        array->count = new_count;
    }
    else if(old_count < new_count)
    {
        if(el_gen_func == NULL) return ARR_SET_SIZE_GEN_NULL_GEN_FUNC;

        int i, assign_status;
        for(i = old_count; i < new_count; i++)
        {
            assign_status = gds_arr_assign(array, old_count, el_gen_func(data));
            if(assign_status != 0) return ARR_SET_SIZE_GEN_ASSIGN_FAIL;
        }
    }

    return 0;
}

int gds_arr_empty(struct GDSArray* array)
{
    if(array == NULL) return 1;

    int set_size_status = gds_arr_set_size_val(array, 0, NULL);
    if(set_size_status != 0) return 2;

    return 0;
}

int gds_arr_realloc(struct GDSArray* array, size_t new_max_count)
{
    if(array == NULL) return ARR_REALLOC_ERR_ARR_NULL;
    if(new_max_count == 0) return ARR_REALLOC_ERR_NEW_COUNT_EQ_ZERO;

    array->data = realloc(array->data, new_max_count * array->element_size);
    if(array->data == NULL) return ARR_REALLOC_ERR_REALLOC_FAIL;

    array->max_count = new_max_count;
    array->count = gds_misc_min(array->count, array->max_count);

    return 0;
}

ssize_t gds_arr_get_count(const struct GDSArray* array)
{
    if(array == NULL) return -1;

    return array->count;
}

ssize_t gds_arr_get_max_count(const struct GDSArray* array)
{
    if(array == NULL) return -1;

    return array->max_count;
}

void* gds_arr_get_data(const struct GDSArray* array)
{
    if(array == NULL) return NULL;

    return array->data;
}

size_t gds_arr_get_element_size(const struct GDSArray* array)
{
    if(array == NULL) return 0;

    return array->element_size;
}

size_t gds_arr_get_struct_size()
{
    return sizeof(struct GDSArray);
}

// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------

static int _gds_arr_shift_right(struct GDSArray* array, size_t start_idx)
{
    if(array == NULL) return _ARR_SHIFT_R_ERR_ARR_NULL;
    if(start_idx >= array->count) return _ARR_SHIFT_R_ERR_START_IDX_OUT_OF_BOUNDS;

    size_t array_count = array->count;

    size_t step = array->element_size;
    size_t elements_shifted = array_count - start_idx;

    void* start_pos = gds_arr_at(array, start_idx);

    memmove(start_pos + step, start_pos, step * elements_shifted);
    return 0;
}

static int _gds_arr_shift_left(struct GDSArray* array, size_t start_idx)
{
    if(array == NULL) return _ARR_SHIFT_L_ERR_ARR_NULL;
    if(start_idx >= array->count) return _ARR_SHIFT_L_ERR_START_IDX_OUT_OF_BOUNDS;

    size_t array_count = array->count;

    size_t step = array->element_size;
    size_t elements_shifted = array_count - start_idx;

    void* start_pos = gds_arr_at(array, start_idx) - step;

    memmove(start_pos, start_pos + step, step * elements_shifted);
    return 0;
}


static void _gds_arr_on_element_removal(struct GDSArray* array, void* data)
{
    if(array == NULL) return;
    if(array->on_element_removal_func == NULL) return;

    array->on_element_removal_func(data);
}

static int _gds_arr_on_element_removal_batch(struct GDSArray* array, size_t start_pos, size_t end_pos)
{
    if(array == NULL) return 1;
    if(start_pos > end_pos) return 2;
    if(array->count == 0) return 3;
    if(start_pos > array->count - 1) return 4;
    if(end_pos > array->count) return 5;

    if(array->on_element_removal_func == NULL) return 0;

    int i;
    void* curr_element;
    for(i = start_pos; i < end_pos; i++)
    {
        curr_element = gds_arr_at(array, i);
        if(curr_element == NULL) return 6;
        _gds_arr_on_element_removal(array, curr_element);
    }

    return 0;
}
