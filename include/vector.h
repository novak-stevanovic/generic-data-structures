#ifndef _VECTOR_H
#define _VECTOR_H

#include <stdlib.h>

#define DEFAULT_RESIZE_COUNT 100
#define DEFAULT_SIZE_COUNT 100

/* Fields:
* size_t count - current count of elements,
* size_t alloced_count - current array capacity,
* size_t resize_count - how much the array resizes on realloac,
* size_t element_size - size of 1 element,
* size_t initial_count - initial vector capacity
* void* vector - ptr to data. 
* Modifying any fields except resize_count will result in breaking the code. */
struct Vector
{
    size_t count;
    size_t alloced_count;
    size_t resize_count;
    size_t element_size;
    size_t initial_count;
    void* data;
};

/* Allocates memory for struct Vector <vector>, initializes struct fields. Allocates the memory for the first <_initial_count> elements of the array. 
 * Return value:
 * on success: 0
 * on failure: 1 - allocing first chunk for array failed */
int vec_init(struct Vector* vector, size_t _initial_count, size_t _resize_count, size_t _element_size);

/* Frees dynamically allocated memory. Sets fields to default values. */
void vec_destruct(struct Vector* vector);

/* Calls vec_init with parameter <_resize_count> set to DEFAULT_RESIZE_COUNT. */
#define VEC_INIT_DEFAULT(vector, element_size) vec_init(vector, DEFAULT_SIZE_COUNT, DEFAULT_RESIZE_COUNT, element_size)

/* Assigns data pointed to by <data> to position <pos> inside Vector <vector>. Keep in mind that the maximum value of <pos> must be 
 * <vector>->count. This can only be done if <vector>->count < <vector>->alloced_count. This function, even if it assigns
 * value at 1 element further than the end of the vector, will not expand the vector. Use vec_append() instead. 
 * Return value:
 * on success: 0
 * on failure: 1 - memcpy() failed. */
int vec_assign(struct Vector* vector, void* data, size_t pos);

/* Inserts element pointed at by <data> at index <pos> in the Vector <vector>. May result in resizing of <vector>.
 * Return value:
 * on success: 0
 * on failure:
 * 1 - array had to be resized and the operation failed.
 * 2 - failure to assign value to specified <pos>
 * 3 - failure to shift elements starting from index <pos> rightward(using memmove()) */
int vec_insert(struct Vector* vector, void* data, size_t pos);

/* Appends data pointed at by <data> at the end of the Vector <vector>. Calls vec_insert() by specyfing <pos> as <vector>->count - 1.
 * May result in resizing of <vector>.
 * Return value:
 * on success: 0
 * on failure:
 * 1 - array had to be resized and the operation failed.
 * 2 - failure to perform memcpy() for calculated dest address and <data> src address. */
int vec_append(struct Vector* vector, void* data);

/* Removes element at index <pos> in Vector <vector>. May result in resizing of <vector>.
 * Return value:
 * on success: 0
 * on failure:
 * 1 - failure to shift elements starting from index <pos + 1> leftward(memmove).
 * 2 - failure to resize Vector <vector>. */
int vec_remove(struct Vector* vector, size_t pos);

/* Removes last element Vector <vector>. May result in resizing of <vector>.
 * Return value:
 * on success: 0
 * on failure: 2 - failure to resize Vector <vector>. */
int vec_pop(struct Vector* vector);

/* Calculates address of element with index specified by <pos>.
 * Return value:
 * Address of element with index <pos>. */
void* vec_at(struct Vector* vector, size_t pos);

#endif
