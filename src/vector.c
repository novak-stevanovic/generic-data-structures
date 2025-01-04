#include <stdio.h>
#include <string.h>

#include "vector.h"
#include "assert.h"
#include "misc.h"

struct Vector
{
    size_t count;
    size_t alloced_count;
    size_t chunk_count;
    size_t element_size;
    size_t min_count;
    void* data;
};

/* Calculates position of element with starting address <start_addr> in Vector <vector>.
 * Return value: Index of calculated position. */
size_t _vec_get_element_idx(struct Vector* vector, void* start_addr);

/* Calculates starting address of element with index <idx> in Vector <vector>.
 * Return value: Pointer to calculated starting address. */
void* _vec_get_element_addr(const struct Vector* vector, size_t idx);

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

/* Gets count of chunks for given <count>, <min_count> and <chunk_count> parameters. For example:
 * For a vector with: 624 elements, min_count = 20, chunk_count = 100, the return value will be equal to 7 because 7 chunks of
 * 100 elements are required to store the whole vector.
 * Return value: count of chunks needed to store vector with given parameters. */
int _vec_get_count_of_chunks(ssize_t count, ssize_t min_count, ssize_t chunk_count);

/* Checks if vector <vector> needs to be resized to accomodate the new count of <vector>'s elements. New count is calculated with:
 * old_count + count_diff. If needed, performs a realloc() call for <vector>'s data.
 * Return value:
 * on success: 0
 * on failure: 1 - realloc() failed. */
int _vec_resize_if_needed(struct Vector* vector, int count_diff);

// --------------------------------------------------------------------------------------------

struct Vector* vec_init(size_t min_count, size_t chunk_count, size_t element_size)
{
    assert(chunk_count != 0);
    assert(element_size != 0);

    struct Vector* vector = (struct Vector*)malloc(sizeof(struct Vector));

    if(vector == NULL) return NULL;

    vector->element_size = element_size;
    vector->chunk_count = chunk_count;
    vector->min_count = min_count;
    vector->count = 0;
    vector->alloced_count = min_count;
    vector->data = malloc(element_size * min_count);

    if(vector->data == NULL) return NULL;

    return vector;
}

void vec_destruct(struct Vector* vector)
{
    assert(vector != NULL);

    free(vector->data);

    vector->count = 0;
    vector->alloced_count = 0;
    vector->element_size = 0;
    vector->min_count = 0;
    vector->chunk_count = 0;
    vector->data = NULL;
}

int vec_assign(struct Vector* vector, const void* data, size_t pos)
{
    assert(vector != NULL);
    assert(pos <= vector->count);
    if(pos == vector->count)
        assert(vector->count < vector->alloced_count);

    void* addr = _vec_get_element_addr(vector, pos);
    assert(addr != NULL);

    void* memcpy_addr = memcpy(addr, data, vector->element_size);
    if(memcpy_addr == NULL) return 1;

    return 0;
}

int vec_insert(struct Vector* vector, const void* data, size_t pos)
{
    assert(vector != NULL);
    assert(vector->data != NULL);

    assert(pos <= vector->count);
    assert(data != NULL);

    if (_vec_resize_if_needed(vector, 1) != 0) return 1;

    if(pos < vector->count)
        if(_vec_shift_right(vector, pos) != 0) return 3;
    
    int assign_status = vec_assign(vector, data, pos);
    if(assign_status != 0) return 2;

    vector->count++;

    return 0;

}

int vec_append(struct Vector* vector, const void* data)
{
    return vec_insert(vector, data, vector->count);
}

int vec_remove(struct Vector* vector, size_t pos)
{
    assert(vector != NULL);
    assert(vector->data != NULL);
    assert(pos < vector->count);

   if(pos < (vector->count - 1))
   {
       int shift_status = _vec_shift_left(vector, pos + 1);
       if(shift_status != 0) return 1;
   }

   vector->count--;

   int shrink_status = _vec_resize_if_needed(vector, -1); 
   if((shrink_status != 0) && (shrink_status != 1)) return 2;

   return 0;
}

int vec_pop(struct Vector* vector)
{
    return vec_remove(vector, vector->count - 1);
}

void* vec_at(const struct Vector* vector, size_t pos)
{
    assert(vector != NULL);
    assert(vector->data != NULL);
    assert(vector->count > pos);

    return _vec_get_element_addr(vector, pos);
}

size_t vec_get_count(const struct Vector* vector)
{
    assert(vector != NULL);

    return vector->count;
}

size_t vec_get_resize_count(const struct Vector* vector)
{
    assert(vector != NULL);

    return vector->chunk_count;
}

void vec_set_resize_count(struct Vector* vector, size_t chunk_count)
{
    assert(vector != NULL);

    vector->chunk_count = chunk_count;
}

void* vec_get_data(const struct Vector* vector)
{
    assert(vector != NULL);

    return vector->data;
}

size_t vec_get_struct_size()
{
    return sizeof(struct Vector);
}

// --------------------------------------------------------------------------------------------

void* _vec_alloc_new_chunk(struct Vector* vector, size_t _resize_count)
{
    assert(vector != NULL);

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
    return _vec_alloc_new_chunk(vector, vector->chunk_count);
}

size_t _vec_get_element_idx(struct Vector* vector, void* start_addr)
{
    assert(vector != NULL);
    assert(vector->data != NULL);
    void* vector_data = vector->data;
    assert(vector->count > 0);
    void* vector_tail = vec_at(vector, vector->count - 1);
    assert(((start_addr >= vector_data) && (start_addr <= vector_tail)));

    return ((size_t)(start_addr - vector_data)) / vector->element_size;
}

void* _vec_get_element_addr(const struct Vector* vector, size_t idx)
{
    assert(vector != NULL);
    assert(vector->data != NULL);
    assert(idx <= vector->count);

    return vector->data + (idx * vector->element_size);
}

int _vec_shift_right(struct Vector* vector, size_t start_idx)
{
    assert(vector != NULL);
    assert(vector->data != NULL);
    assert(start_idx < vector->count);

    size_t vector_count = vector->count;

    assert(vector_count < vector->alloced_count);

    size_t step = vector->element_size;
    size_t elements_shifted = vector_count - start_idx;

    void* start_pos = _vec_get_element_addr(vector, start_idx);

    int memmove_status = (memmove(start_pos + step, start_pos, step * elements_shifted) == NULL);
    return memmove_status;
}

int _vec_shift_left(struct Vector* vector, size_t start_idx)
{
    assert(vector != NULL);
    assert(vector->data != NULL);

    size_t vector_count = vector->count;

    assert(start_idx < vector_count);

    size_t step = vector->element_size;
    size_t elements_shifted = vector_count - start_idx;

    void* start_pos = _vec_get_element_addr(vector, start_idx) - step;

    return (memmove(start_pos, start_pos + step, step * elements_shifted) == NULL);
}

int _vec_get_count_of_chunks(ssize_t count, ssize_t min_count, ssize_t chunk_count)
{
    return misc_max(0, (count - min_count) / chunk_count + ((count - min_count) % chunk_count > 0));
}

int _vec_resize_if_needed(struct Vector* vector, int count_diff)
{
    assert(vector != NULL);

    ssize_t count = vector->count;
    ssize_t alloced_count = vector->alloced_count;
    ssize_t chunk_count = vector->chunk_count;
    ssize_t element_size = vector->element_size;
    ssize_t min_count = vector->min_count;

    ssize_t chunks_required = _vec_get_count_of_chunks(count + count_diff, min_count, chunk_count);
    ssize_t chunks_alloced = _vec_get_count_of_chunks(alloced_count, min_count, chunk_count);

    if(chunks_required != chunks_alloced)
    {
        vector->data = realloc(vector->data, min_count * element_size + chunks_required * chunk_count * element_size);
        if(vector->data == NULL) return 1;
        vector->alloced_count = min_count + chunks_required * chunk_count;
    }
    return 0;
}
