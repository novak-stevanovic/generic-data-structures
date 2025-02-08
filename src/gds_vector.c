#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "gds.h"
#include "gds_vector.h"
#include "gds_misc.h"

#define _VEC_INTERNAL_ERR_BASE (VEC_ERR_BASE + 500)

#ifndef GDS_DISABLE_OPAQUE_STRUCTS
typedef struct GDSVector
{
    size_t _count;
    size_t _alloced_count;
    size_t _count_in_chunk;
    size_t _element_size;
    size_t _min_count;
    void* _data;
    void (*_on_element_removal_func)(void*);
} GDSVector;
#endif

// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------

#define _VEC_SHIFT_R_ERR_BASE (_VEC_INTERNAL_ERR_BASE + 10)
#define _VEC_SHIFT_R_ERR_VEC_NULL (_VEC_SHIFT_R_ERR_BASE + 1)
#define _VEC_SHIFT_R_ERR_START_IDX_OUT_OF_BOUNDS (_VEC_SHIFT_R_ERR_BASE + 2)
#define _VEC_SHIFT_R_ERR_INVALID_ADDR (_VEC_SHIFT_R_ERR_BASE + 4) // gds_vec_at() returned null.

/* Shift elements right of index(including) start_idx rightward by calling memmove(). This function cannot expand the vector if there is not enough memory allocated.
 * Make sure that vector->count < vector->alloced_count before calling.
 * Return value:
 * on success: 0
 * on failure: one of the error codes above. */
static int _gds_vec_shift_right(GDSVector* vector, size_t start_idx);

// --------------------------------------------------------------------------------------------------------------------------------------------

#define _VEC_SHIFT_L_ERR_BASE (_VEC_INTERNAL_ERR_BASE + 20)
#define _VEC_SHIFT_L_ERR_VEC_NULL (_VEC_SHIFT_L_ERR_BASE + 1)
#define _VEC_SHIFT_L_ERR_START_IDX_OUT_OF_BOUNDS (_VEC_SHIFT_L_ERR_BASE + 2)
#define _VEC_SHIFT_L_ERR_INVALID_ADDR (_VEC_SHIFT_L_ERR_BASE + 4) // gds_vec_at() returned null.

/* Shift elements right of index(including) start_idx leftward by calling memmove().
 * Return value:
 * on success: 0
 * on failure: one of the error codes above. */
static int _gds_vec_shift_left(GDSVector* vector, size_t start_idx);

// --------------------------------------------------------------------------------------------------------------------------------------------

/* Gets count of chunks for given count, min_count and count_in_chunk parameters. For example:
 * For a vector with: 624 elements, min_count = 20, count_in_chunk = 100, the return value will be equal to 7 because 7 chunks of
 * 100 elements are required to store the whole vector. For the same vector with 601 elements, 6 chunks would suffice(600 + 20 > 601).
 * This function can be considered static in the object-oriented sense, as it is not tied to a particular vector. This function may be used
 * to determine how many chunks has been allocated for a vector.
 * Return value: count of chunks needed to store vector with given parameters. */
static size_t _gds_vec_get_count_of_chunks(ssize_t count, ssize_t min_count, ssize_t count_in_chunk);

/* Invokes _gds_vec_get_count_of_chunks(vector->count, vector->min_count, vector->count_in_chunk).
 * Return value:
 * on success: positive number representing how many chunks(each consisting of vector->count_in_chunk elements) are needed to store
 * vector's data. Example given above.
 * on failure: -1 - vector is NULL. */
static ssize_t _gds_vec_get_count_of_chunks_for_vec(GDSVector* vector);

// --------------------------------------------------------------------------------------------------------------------------------------------

/* Checks if vector has allocated more/less memory for its data than it needs based on the vector's:
 * current count, alloced_count, count_in_chunk, min_count and new_count parameter, where: new_count is the new count of elements
 * It uses the out_chunks_required parameter as an extra return value.
 * Return value:
 * 1: vector resizing is needed - out_chunks_required will be set to the count of required chunks of memory needed to store vector->data.
 * 0: vector resizing is not needed - out_chunks_required = 0. */
static int _gds_vec_is_resizing_needed(GDSVector* vector, size_t new_count, size_t* out_chunks_required);

// --------------------------------------------------------------------------------------------------------------------------------------------

/* Resizes vector by reallocating vector->data for (chunks_required * count_in_chunk * element_size + min_count * element_size) bytes of memory. 
 * This is in order to store the min_count of elements + the chunks needed to store the extra elements. This function does not check whether reallocating should happen, nor does
 * it check if a malloc() has been called for vector->data(). Make sure that: reallocating even needs to happen via the func _gds_vec_is_resizing_needed() and make sure that
 * vector->data != NULL. Note: This function will not perform any calls to vector->on_element_removal_func(void*) even if shrinking occurs. Use a variant of gds_vec_set_size() instead.
 * Return value:
 * on success: 0
 * on failure: 1 - realloc() failed. */
static int _gds_vec_resize(GDSVector* vector, size_t chunks_required);

// --------------------------------------------------------------------------------------------------------------------------------------------

/* Invokes vector->on_element_removal_func(data) if the field is non-NULL. */
static void _gds_vec_on_element_removal(GDSVector* vector, void* data);

// --------------------------------------------------------------------------------------------------------------------------------------------

/* Invokes _gds_vec_on_element_removal(GDSVector* vector, void* data) for each element starting from start_pos(including)
 * , ending at end_pos (excluding). If start_pos == end_pos, no calls will be performed.
 * Return value:
 * on success: 0,
 * on failure: 
 * 1 - argument vector is NULL, 
 * 2 - start_pos > end_pos, 
 * 3 - vector->count = 0,
 * 4 - start_pos > vector->count - 1, 
 * 5 - end_pos > vector->count */
static int _gds_vec_on_element_removal_batch(GDSVector* vector, size_t start_pos, size_t end_pos);

// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------

GDSVector* gds_vec_create(size_t min_count, size_t count_in_chunk, size_t element_size, void (*on_element_removal_func)(void*))
{
    if((count_in_chunk == 0) || (element_size == 0) || (min_count == 0)) return NULL;

    GDSVector* vector = (GDSVector*)malloc(sizeof(GDSVector));

    if(vector == NULL) return NULL;

    vector->_data = malloc(element_size * min_count);

    if(vector->_data == NULL)
    {
        free(vector);
        return NULL;
    }

    vector->_element_size = element_size;
    vector->_count_in_chunk = count_in_chunk;
    vector->_min_count = min_count;
    vector->_count = 0;
    vector->_alloced_count = min_count;
    vector->_on_element_removal_func = on_element_removal_func;

    return vector;
}

void gds_vec_destruct(GDSVector* vector)
{
    if(vector == NULL) return;

    gds_vec_empty(vector);

    free(vector->_data);

    vector->_count = 0;
    vector->_alloced_count = 0;
    vector->_element_size = 0;
    vector->_min_count = 0;
    vector->_count_in_chunk = 0;
    vector->_data = NULL;
}

int gds_vec_assign(GDSVector* vector, const void* data, size_t pos)
{
    if(vector == NULL) return VEC_ASSIGN_ERR_NULL_VEC;
    if(data == NULL) return VEC_ASSIGN_ERR_POS_OUT_OF_BOUNDS;
    if(pos >= vector->_count) return VEC_ASSIGN_ERR_POS_OUT_OF_BOUNDS;

    void* addr = gds_vec_at(vector, pos);
    gds_rcheck(addr != NULL, _VEC_ASSIGN_FERR_INVALID_ADDR_FOUND);

    memcpy(addr, data, vector->_element_size);
    return 0;
}

int gds_vec_insert(GDSVector* vector, const void* data, size_t pos)
{
    if(vector == NULL) return VEC_INSERT_ERR_NULL_VEC;
    if(data == NULL) return VEC_INSERT_ERR_NULL_DATA;

    size_t vector_count = vector->_count;
    if(pos > vector_count) return VEC_INSERT_ERR_POS_OUT_OF_BOUNDS;

    size_t chunks_required;
    if(_gds_vec_is_resizing_needed(vector, vector_count + 1, &chunks_required)) 
    {
        int resize_op_status = _gds_vec_resize(vector, chunks_required);
        if(resize_op_status != 0) return VEC_INSERT_FERR_RESIZE_OP_FAILED;
    }

    if(pos < vector->_count)
    {
        int shift_right_op_status = _gds_vec_shift_right(vector, pos);
        gds_rcheck(shift_right_op_status == 0, _VEC_INSERT_FERR_SHIFTING_OP_FAILED);
    }
    
    vector->_count++; // it is important to increase the count first, so that the gds_vec_assign() function will work.

    int assign_op_status = gds_vec_assign(vector, data, pos);
    gds_rcheck(assign_op_status == 0, _VEC_INSERT_FERR_ASSIGN_OP_FAILED);

    return 0;

}

int gds_vec_append(GDSVector* vector, const void* data)
{
    int insert_op_status = gds_vec_insert(vector, data, vector->_count);
    if(insert_op_status != 0)
        return (insert_op_status - VEC_INSERT_ERR_BASE + VEC_APPEND_ERR_BASE); // convert to append error code
    else
        return 0;
}

int gds_vec_remove(GDSVector* vector, size_t pos)
{
    if(vector == NULL) return VEC_REMOVE_ERR_NULL_VEC;
    if(pos >= vector->_count) return VEC_REMOVE_ERR_POS_OUT_OF_BOUNDS;

    void* element_addr = gds_vec_at(vector, pos);
    gds_rcheck(element_addr != NULL, _VEC_REMOVE_FERR_AT_FAILED);

    _gds_vec_on_element_removal(vector, element_addr);

    if(pos < (vector->_count - 1))
    {
        int shift_op_status = _gds_vec_shift_left(vector, pos + 1);
        gds_rcheck(shift_op_status == 0, _VEC_REMOVE_FERR_SHIFTING_OP_FAILED);
    }

    vector->_count--;

    size_t chunks_required;
    if(_gds_vec_is_resizing_needed(vector, vector->_count - 1, &chunks_required))
    {
        int resize_op_status = _gds_vec_resize(vector, chunks_required);
        if(resize_op_status != 0) return VEC_REMOVE_FERR_RESIZE_OP_FAILED;
    }

    return 0;
}

int gds_vec_pop(GDSVector* vector)
{
    if(vector == NULL) return VEC_POP_ERR_NULL_VEC;
    if(vector->_count == 0) return VEC_POP_ERR_VEC_EMPTY;

    int remove_op_status = gds_vec_remove(vector, vector->_count - 1);
    if(remove_op_status != 0)
        return remove_op_status - VEC_REMOVE_ERR_BASE + VEC_POP_ERR_BASE; // convert to pop error code
    else
        return 0;
}

int gds_vec_set_size(GDSVector* vector, size_t new_size, void (*assign_func)(void*, void*), void* data)
{
    if(vector == NULL) return VEC_SET_SIZE_ERR_NULL_VEC;

    size_t old_size = vector->_count;
    if(old_size == new_size) return 0;

    if((old_size < new_size) && (assign_func == NULL)) return VEC_SET_SIZE_ERR_NULL_ASSIGN_FUNC;

    // checks are over

    if(old_size > new_size) // if removing, remove redundant elements - call on_removal_func for each
    {
        int on_batch_removal_status = _gds_vec_on_element_removal_batch(vector, new_size, old_size);
        gds_rcheck(on_batch_removal_status == 0, 3);
    }

    size_t chunks_required;
    int resize_needed = _gds_vec_is_resizing_needed(vector, new_size, &chunks_required);

    if(resize_needed) // resize vec if needed
    {
        int resize_status = _gds_vec_resize(vector, chunks_required);
        if(resize_status != 0) return 2;
    }

    vector->_count = new_size;

    if(old_size < new_size) // if appending, append elements until new_count reached.
    {
        int i;
        void* element_addr;
        for(i = old_size; i < new_size; i++)
        {
            element_addr = gds_vec_at(vector, i);
            assign_func(element_addr, data);
            gds_rcheck(element_addr != NULL, _VEC_SET_SIZE_FERR_AT_FAIL);
        }
    }

    return 0;
}

int gds_vec_empty(GDSVector* vector)
{
    if(vector == NULL) return 1;

    int set_size_status = gds_vec_set_size(vector, 0, NULL, NULL);
    if(set_size_status != 0) return 2;

    vector->_count = 0;
    return 0;

}

void* gds_vec_at(const GDSVector* vector, size_t pos)
{
    if(vector == NULL) return NULL;
    if(pos >= vector->_count) return NULL;

    return vector->_data + (pos * vector->_element_size);
}

ssize_t gds_vec_get_count(const GDSVector* vector)
{
    if(vector == NULL) return -1;
    return vector->_count;
}

ssize_t gds_vec_set_min_count(GDSVector* vector, size_t new_min_count)
{
    if(vector == NULL) return -1;

    if(vector->_min_count == new_min_count) return 0;

    vector->_min_count = new_min_count;

    size_t chunks_required = _gds_vec_get_count_of_chunks_for_vec(vector);
    int resize_status = _gds_vec_resize(vector, chunks_required);
    if(resize_status != 0) return -2;

    return 0;
}

ssize_t gds_vec_get_min_count(const GDSVector* vector)
{
    if(vector == NULL) return -1;

    return vector->_min_count;
}

ssize_t gds_vec_get_resize_count(const GDSVector* vector)
{
    if(vector == NULL) return -1;

    return vector->_count_in_chunk;
}

int gds_vec_set_resize_count(GDSVector* vector, size_t count_in_chunk)
{
    if(vector == NULL) return -1;

    vector->_count_in_chunk = count_in_chunk;

    size_t chunks_required = _gds_vec_get_count_of_chunks_for_vec(vector);
    int resize_status = _gds_vec_resize(vector, chunks_required);
    if(resize_status != 0) return -2;

    return 0;
}

void* gds_vec_get_data(const GDSVector* vector)
{
    if(vector == NULL) return NULL;

    return vector->_data;
}

size_t gds_vec_get_element_size(const GDSVector* vector)
{
    if(vector == NULL) return 0;

    return vector->_element_size;
}

size_t gds_vec_get_struct_size()
{
    return sizeof(GDSVector);
}

// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------

static int _gds_vec_shift_right(GDSVector* vector, size_t start_idx)
{
    if(vector == NULL) return _VEC_SHIFT_R_ERR_VEC_NULL;
    if(start_idx >= vector->_count) return _VEC_SHIFT_R_ERR_START_IDX_OUT_OF_BOUNDS;

    size_t vector_count = vector->_count;

    size_t step = vector->_element_size;
    size_t elements_shifted = vector_count - start_idx;

    void* start_pos = gds_vec_at(vector, start_idx);
    if(start_pos == NULL) return _VEC_SHIFT_R_ERR_INVALID_ADDR;

    memmove(start_pos + step, start_pos, step * elements_shifted);

    return 0;
}

static int _gds_vec_shift_left(GDSVector* vector, size_t start_idx)
{
    if(vector == NULL) return _VEC_SHIFT_L_ERR_VEC_NULL;

    size_t vector_count = vector->_count;

    if(start_idx >= vector->_count) return _VEC_SHIFT_L_ERR_START_IDX_OUT_OF_BOUNDS;

    size_t step = vector->_element_size;
    size_t elements_shifted = vector_count - start_idx;

    void* start_pos = gds_vec_at(vector, start_idx) - step;
    if(start_pos == NULL) return _VEC_SHIFT_L_ERR_INVALID_ADDR;

    memmove(start_pos, start_pos + step, step * elements_shifted);
    return 0;
}

static size_t _gds_vec_get_count_of_chunks(ssize_t count, ssize_t min_count, ssize_t count_in_chunk)
{
    return gds_misc_max(0, (count - min_count) / count_in_chunk + ((count - min_count) % count_in_chunk > 0));
}

static ssize_t _gds_vec_get_count_of_chunks_for_vec(GDSVector* vector)
{
    if(vector == NULL) return -1;

    return _gds_vec_get_count_of_chunks(vector->_count, vector->_min_count, vector->_count_in_chunk);
}

static int _gds_vec_is_resizing_needed(GDSVector* vector, size_t new_count, size_t* out_chunks_required)
{
    ssize_t alloced_count = vector->_alloced_count;
    ssize_t count_in_chunk = vector->_count_in_chunk;
    ssize_t min_count = vector->_min_count;

    ssize_t chunks_alloced = _gds_vec_get_count_of_chunks(alloced_count, min_count, count_in_chunk);
    ssize_t chunks_required = _gds_vec_get_count_of_chunks(new_count, min_count, count_in_chunk);

    if(chunks_required != chunks_alloced)
    {
        *out_chunks_required = chunks_required;
        return 1;
    }
    else
    {
        *out_chunks_required = 0;
        return 0;
    }
}

static int _gds_vec_resize(GDSVector* vector, size_t chunks_required)
{
    ssize_t count_in_chunk = vector->_count_in_chunk;
    ssize_t element_size = vector->_element_size;
    ssize_t min_count = vector->_min_count;

    vector->_data = realloc(vector->_data, min_count * element_size + chunks_required * count_in_chunk * element_size);
    if(vector->_data == NULL) return 1;
    vector->_alloced_count = min_count + chunks_required * count_in_chunk;

    return 0;
}

static void _gds_vec_on_element_removal(GDSVector* vector, void* data)
{
    if(vector == NULL) return;
    if(vector->_on_element_removal_func == NULL) return;

    vector->_on_element_removal_func(data);
}

static int _gds_vec_on_element_removal_batch(GDSVector* vector, size_t start_pos, size_t end_pos)
{
    if(vector == NULL) return 1;
    if(start_pos > end_pos) return 2;
    if(vector->_count == 0) return 3;
    if(start_pos > vector->_count - 1) return 4;
    if(end_pos > vector->_count) return 5;

    if(vector->_on_element_removal_func == NULL) return 0;

    int i;
    void* curr_element;
    for(i = start_pos; i < end_pos; i++)
    {
        curr_element = gds_vec_at(vector, i);
        _gds_vec_on_element_removal(vector, curr_element);
    }

    return 0;
}
