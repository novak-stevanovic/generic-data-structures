#include "vector.h"
#include "assert.h"
#include <string.h>

/* Resizes Vector <vector> by performing alloc/realloc. Call to this function extends memory by _resize_count * <vector>->_element_size.
 * Return value:
 * on success: address of <vector>->data.
 * on failure: NULL. */
void* _vec_alloc_new_chunk(struct Vector* vector, size_t _resize_count);

/* Calls _alloc_new_chunk() with parameters (<vector>, <vector>->_resize_count).
 * Return value: Same as _alloc_new_chunk(). */
void* _vec_alloc_new_chunk_rsz(struct Vector* vector);

/* Calculates position of element with starting address <start_addr> in Vector <vector>.
 * Return value: Index of calculated position. */
size_t _vec_get_element_idx(struct Vector* vector, void* start_addr);

/* Calculates starting address of element with index <idx> in Vector <vector>.
 * Return value: Pointer to calculated starting address. */
void* _vec_get_element_addr(struct Vector* vector, size_t idx);

/* Shift elements starting from index <start_idx> rightward by calling memmove().
 * Return value:
 * on success: 0
 * on failure: 1 - memmove() failed. */
int _vec_shift_right(struct Vector* vector, size_t start_idx);

/* Shift elements starting from index <start_idx> leftward by calling memmove().
 * Return value:
 * on success: 0
 * on failure: 1 - memmove() failed. */
int _vec_shift_left(struct Vector* vector, size_t start_idx);

/* Calculates if it is possible to shrink Vector <vector> by at least <vector>->resize_count. Performs the shrinking if possible.
 * Return value:
 * on success: 0(shrinking was not performed), 1(shrinking was successfully perfmormed)
 * on failure: 2(shrinking was possible but failed - realloc() failed) */
int _vec_shrink_if_possible(struct Vector* vector);

void __print_int_vector_debug(struct Vector* v);

// --------------------------------------------------------------------------------------------

int vec_init(struct Vector* vector, size_t _initial_count, size_t _resize_count, size_t _element_size)
{
    ASSERT_NON_NULL_ARG(vector, "vector");

    ASSERT(_initial_count != 0, "Invalid '_initial_count' parameter");
    ASSERT(_resize_count != 0, "Invalid '_resize_count' parameter");
    ASSERT(_element_size != 0, "Invalid '_element_size' parameter");

    vector->element_size = _element_size;
    vector->resize_count = _resize_count;
    vector->initial_count = _initial_count;

    vector->data = NULL;
    vector->count = 0;
    vector->alloced_count = 0;

    if (_vec_alloc_new_chunk(vector, _initial_count) == NULL) return 1;

    return 0;
}

void vec_destruct(struct Vector* vector)
{
    ASSERT_NON_NULL_ARG(vector, "vector");
    ASSERT_NON_NULL_VAL(vector->data, "vector->data");

    free(vector->data);

    vector->data = NULL;
    vector->count = 0;
    vector->element_size = 0;
    vector->resize_count = 0;
    vector->alloced_count = 0;
}

int vec_assign(struct Vector* vector, void* data, size_t pos)
{
    ASSERT_NON_NULL_ARG(vector, "vector");
    ASSERT(pos <= vector->count, "Value of <pos> must be lesser or equal to vector->curr_count");
    if(pos == vector->count)
        ASSERT(vector->count < vector->alloced_count, "INVALID POS, ARRAY_COUNT, MAX_COUNT");

    void* addr = _vec_get_element_addr(vector, pos);
    ASSERT_NON_NULL_VAL(addr, "addr");

    void* memcpy_addr = memcpy(addr, data, vector->element_size);
    if(memcpy_addr == NULL) return 1;

    return 0;
}

int vec_insert(struct Vector* vector, void* data, size_t pos)
{
    ASSERT_NON_NULL_ARG(vector, "vector");
    ASSERT_NON_NULL_VAL(vector->data, "vector->data");

    ASSERT(pos <= vector->count, "Invalid insert operation.\n");
    ASSERT_NON_NULL_ARG(data, "data");

    if(vector->count == vector->alloced_count)
        if (_vec_alloc_new_chunk_rsz(vector) == NULL) return 1;

    if(pos < vector->count)
        if(_vec_shift_right(vector, pos) != 0) return 3;
    
    int assign_status = vec_assign(vector, data, pos);
    if(assign_status != 0) return 2;

    vector->count++;

    return 0;

}

int vec_append(struct Vector* vector, void* data)
{
    return vec_insert(vector, data, vector->count);
}

int vec_remove(struct Vector* vector, size_t pos)
{
    ASSERT_NON_NULL_ARG(vector, "vector");
    ASSERT_NON_NULL_VAL(vector->data, "vector->data");
    ASSERT(pos < vector->count, "Invalid value of argument 'pos'.\n");

   if(pos < (vector->count - 1))
   {
       int shift_status = _vec_shift_left(vector, pos + 1);
       if(shift_status != 0) return 1;
   }

   vector->count--;

   int shrink_status = _vec_shrink_if_possible(vector); 
   if((shrink_status != 0) && (shrink_status != 1)) return 2;

   return 0;
}

int vec_pop(struct Vector* vector)
{
    return vec_remove(vector, vector->count - 1);
}

void* vec_at(struct Vector* vector, size_t pos)
{
    ASSERT_NON_NULL_ARG(vector, "vector");
    ASSERT_NON_NULL_VAL(vector->data, "vector->data");
    ASSERT(pos < vector->count, "Invalid value of argument 'idx'\n.");

    return _vec_get_element_addr(vector, pos);
}

// --------------------------------------------------------------------------------------------

void* _vec_alloc_new_chunk(struct Vector* vector, size_t _resize_count)
{
    ASSERT_NON_NULL_ARG(vector, "vector");

    size_t new_alloced_count = vector->alloced_count + _resize_count;
    size_t element_size = vector->element_size;
    size_t op_size = new_alloced_count * element_size;

    if(vector->data == NULL)
        vector->data = malloc(op_size);
    else
        vector->data = realloc(vector->data, op_size);

    if(vector->data == NULL) return NULL;

    vector->alloced_count = new_alloced_count;
    return vector->data;
}

void* _vec_alloc_new_chunk_rsz(struct Vector* vector)
{
    return _vec_alloc_new_chunk(vector, vector->resize_count);
}

size_t _vec_get_element_idx(struct Vector* vector, void* start_addr)
{
    ASSERT_NON_NULL_ARG(vector, "vector");
    ASSERT_NON_NULL_VAL(vector->data, "vector->data");
    void* vector_data = vector->data;
    void* vector_tail = vec_at(vector, vector->count - 1);
    ASSERT(((start_addr >= vector_data) && (start_addr <= vector_tail)), "Invalid value of 'start_addr' argument.\n");

    return ((size_t)(start_addr - vector_data)) / vector->element_size;
}

void* _vec_get_element_addr(struct Vector* vector, size_t idx)
{
    ASSERT_NON_NULL_ARG(vector, "vector");
    ASSERT_NON_NULL_VAL(vector->data, "vector->data");
    ASSERT(idx <= vector->count, "Invalid value of argument 'idx'\n.");

    return vector->data + (idx * vector->element_size);
}

int _vec_shift_right(struct Vector* vector, size_t start_idx)
{
    ASSERT_NON_NULL_ARG(vector, "vector");
    ASSERT_NON_NULL_VAL(vector->data, "vector->data");
    ASSERT(start_idx < vector->count, "Invalid start_idx parameter.\n");

    size_t vector_count = vector->count;

    ASSERT(vector_count < vector->alloced_count, "_shift_right called when count == _alloced_count.\n");

    size_t step = vector->element_size;
    size_t elements_shifted = vector_count - start_idx;

    void* start_pos = _vec_get_element_addr(vector, start_idx);

    int memmove_status = (memmove(start_pos + step, start_pos, step * elements_shifted) == NULL);
    return memmove_status;
}

int _vec_shift_left(struct Vector* vector, size_t start_idx)
{
    ASSERT_NON_NULL_ARG(vector, "vector");
    ASSERT_NON_NULL_VAL(vector->data, "vector->data");

    size_t vector_count = vector->count;

    ASSERT(start_idx < vector_count, "Invalid argument 'start_idx' passed.\n");

    size_t step = vector->element_size;
    size_t elements_shifted = vector_count - start_idx;

    void* start_pos = _vec_get_element_addr(vector, start_idx) - step;

    return (memmove(start_pos, start_pos + step, step * elements_shifted) == NULL);
}

int _vec_shrink_if_possible(struct Vector* vector)
{
    ASSERT_NON_NULL_ARG(vector, "vector");

    size_t count = vector->count;
    size_t alloced_count = vector->alloced_count;
    size_t resize_count = vector->resize_count;
    size_t element_size = vector->element_size;
    size_t initial_count = vector->initial_count;

    size_t chunks_required = (count - initial_count) / resize_count + ((count - initial_count) % resize_count > 0);
    size_t chunks_alloced = (alloced_count - initial_count) / resize_count + ((alloced_count - initial_count) % resize_count > 0);

    if(chunks_required < chunks_alloced)
    {
        vector->data = realloc(vector->data, initial_count * element_size + chunks_required * resize_count * element_size);
        vector->alloced_count = initial_count + chunks_required * resize_count;
        if(vector->data == NULL) return 2;
        else return 1;
    }
    return 0;
}
