#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "gends_array.h"
#include "gends_misc.h"

#define _ARR_INTERNAL_ERR_BASE (ARR_ERR_BASE + 500)

struct Array
{
    size_t count;
    size_t max_count;
    size_t element_size;
    void* data;
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
static int _arr_shift_right(struct Array* array, size_t start_idx);

// --------------------------------------------------------------------------------------------------------------------------------------------

#define _ARR_SHIFT_L_ERR_BASE (_ARR_INTERNAL_ERR_BASE + 20)
#define _ARR_SHIFT_L_ERR_ARR_NULL (_ARR_SHIFT_L_ERR_BASE + 1)
#define _ARR_SHIFT_L_ERR_START_IDX_OUT_OF_BOUNDS (_ARR_SHIFT_L_ERR_BASE + 2)

/* Shift elements right of(including) index start_idx leftward by calling memmove().
 * Return value:
 * on success: 0
 * on failure - one of the error codes above. */
static int _arr_shift_left(struct Array* array, size_t start_idx);

// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------

struct Array* arr_init(size_t max_count, size_t element_size)
{
    if((max_count == 0) || (element_size == 0)) return NULL;

    struct Array* array = (struct Array*)malloc(sizeof(struct Array));

    array->max_count = max_count;
    array->element_size = element_size;
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

void arr_destruct(struct Array* array)
{
    if(array == NULL) return;

    free(array->data);
    array->count = 0;
    array->element_size = 0;
    array->max_count = 0;
}

void* arr_at(const struct Array* array, size_t pos)
{
    if(array == NULL) return NULL;
    if(pos >= array->count) return NULL;

    return array->data + pos * array->element_size;
}

int arr_assign(struct Array* array, size_t pos, const void* data)
{
    if(array == NULL) return ARR_ASSIGN_ERR_ARR_NULL;
    if(data == NULL) return ARR_ASSIGN_ERR_DATA_NULL;
    if(pos >= array->count) return ARR_ASSIGN_ERR_POS_OUT_OF_BOUNDS;

    void* addr = arr_at(array, pos);
    if(addr == NULL) return ARR_ASSIGN_ERR_INVALID_ADDR_FOUND;

    memcpy(addr, data, array->element_size);

    return 0;
}

int arr_append(struct Array* array, const void* data)
{
    int insert_op_status = arr_insert(array, data, array->count);
    if(insert_op_status != 0)
        return insert_op_status - ARR_INSERT_ERR_BASE + ARR_APPEND_ERR_BASE; // convert to insert error code
    else
        return 0;
}

int arr_insert(struct Array* array, const void* data, size_t pos)
{
    if(array == NULL) return ARR_INSERT_ERR_ARR_NULL;
    if(data == NULL) return ARR_INSERT_ERR_DATA_NULL;

    if(pos > array->count) return ARR_INSERT_ERR_POS_OUT_OF_BOUNDS;

    if(pos < array->count)
    {
        int shift_status = _arr_shift_right(array, pos);
        if(shift_status != 0) return ARR_INSERT_ERR_SHIFTING_OP_FAILED;
    }

    array->count++; // it is important to increase the count first, so that the arr_assign() function will work.

    int assign_op_status = arr_assign(array, pos, data);
    if(assign_op_status != 0) 
    {
        array->count--;
        return ARR_INSERT_ERR_ASSIGN_FAIL;
    }

    return 0;

}

int arr_remove(struct Array* array, size_t pos)
{
    if(array == NULL) return ARR_REMOVE_ERR_ARR_NULL;
    if(pos >= array->count) return ARR_REMOVE_ERR_POS_OUT_OF_BOUNDS;

    if(pos < (array->count - 1))
    {
        int shift_status = _arr_shift_left(array, pos + 1);
        if(shift_status != 0) return ARR_REMOVE_ERR_SHIFTING_OP_FAILED;
    }

    array->count--;

    return 0;
}

int arr_pop(struct Array* array)
{
    if(array == NULL) return ARR_POP_ERR_ARR_NULL;
    if(array->count == 0) return ARR_POP_ERR_ARR_EMPTY;

    int remove_op_status =  arr_remove(array, array->count - 1);
    if(remove_op_status != 0)
        return remove_op_status - ARR_REMOVE_ERR_BASE + ARR_POP_ERR_BASE; // convert to pop error code
    else
        return 0;
}

int arr_realloc(struct Array* array, size_t new_max_count)
{
    if(array == NULL) return ARR_REALLOC_ERR_ARR_NULL;
    if(new_max_count == 0) return ARR_REALLOC_ERR_NEW_COUNT_EQ_ZERO;

    array->data = realloc(array->data, new_max_count * array->element_size);
    if(array->data == NULL) return ARR_REALLOC_ERR_REALLOC_FAIL;

    array->max_count = new_max_count;
    array->count = gends_misc_min(array->count, array->max_count);

    return 0;
}

ssize_t arr_get_count(const struct Array* array)
{
    if(array == NULL) return -1;

    return array->count;
}

ssize_t arr_get_max_count(const struct Array* array)
{
    if(array == NULL) return -1;

    return array->max_count;
}

void* arr_get_data(const struct Array* array)
{
    if(array == NULL) return NULL;

    return array->data;
}

size_t arr_get_struct_size()
{
    return sizeof(struct Array);
}

// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------

static int _arr_shift_right(struct Array* array, size_t start_idx)
{
    if(array == NULL) return _ARR_SHIFT_R_ERR_ARR_NULL;
    if(start_idx >= array->count) return _ARR_SHIFT_R_ERR_START_IDX_OUT_OF_BOUNDS;

    size_t array_count = array->count;

    size_t step = array->element_size;
    size_t elements_shifted = array_count - start_idx;

    void* start_pos = arr_at(array, start_idx);

    memmove(start_pos + step, start_pos, step * elements_shifted);
    return 0;
}

static int _arr_shift_left(struct Array* array, size_t start_idx)
{
    if(array == NULL) return _ARR_SHIFT_L_ERR_ARR_NULL;
    if(start_idx >= array->count) return _ARR_SHIFT_L_ERR_START_IDX_OUT_OF_BOUNDS;

    size_t array_count = array->count;

    size_t step = array->element_size;
    size_t elements_shifted = array_count - start_idx;

    void* start_pos = arr_at(array, start_idx) - step;

    memmove(start_pos, start_pos + step, step * elements_shifted);
    return 0;
}

