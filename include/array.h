#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>

/* Fields:
* size_t count - current count of elements
* size_t max_count - array capacity
* size_t element_size - size of each element
* int alloc_type - array alloc type(each array may be allocated statically or dynamically)
* void* data - addr of array beginning
 */
struct Array
{
    size_t count;
    size_t max_count;
    size_t element_size;
    int alloc_type;
    void* data;
};

#define ALLOC_TYPE_UNALLOCED 0
#define ALLOC_TYPE_STATIC 1
#define ALLOC_TYPE_DYNAMIC 2

/* Initializes Array <array> with max_count elements, each the size of <element_size>. <array>'s array field will point to the provided statically allocated array <static_arr_ptr>.
 * Return value:
 * on success - 0 */
int arr_init_static(struct Array* array, size_t max_count, size_t element_size, void* static_arr_ptr);

/* Initializes Array <array> with max_count elements, each the size of <element_size>. <array>'s array field will point to a dynamically allocated chunk of memory of adequate size.
 * Return value:
 * on success - 0
 * on failure - 1: malloc() failed */
int arr_init_dynamic(struct Array* array, size_t max_count, size_t element_size);

/* Frees dynamically allocated memory if needed. Sets fields of Array <array> to default values. */
void arr_destruct(struct Array* array);

/* Copies memory content pointed to by <data> into the Array <array> at index <pos>. <pos> <= <array>.count, otherwise the program will terminate. Keep in mind that while
 * it is possible to assign data 1 element beyond the current array count, this will not expand the array.
 * Return value:
 * on success - 0
 * on failure - 1: memcpy() failed. */
int arr_assign(struct Array* array, size_t pos, const void* data);

/* Appends data pointed to by <data> to the end of the Array <array>. 
 * Return value:
 * on success - 0 
 * on failure - 1: arr_assign(array, data, array->count) failed. */
int arr_append(struct Array* array, const void* data);

/* Inserts data pointed to by <data> to index <pos> in the Array <array>. 
 * Return value:
 * on success - 0
 * on failure - 1: shifting elements rightwards starting from <pos> failed. 2: arr_assign(array, data, pos) failed. */
int arr_insert(struct Array* array, const void* data, size_t pos);

/* Removes element with position <pos> from Array <array>.
 * Return value:
 * on success - 0
 * on failure - 1: shifting elements with index > <pos> leftwards failed. */
int arr_remove(struct Array* array, size_t pos);

/* Removes element with position <pos> from Array <array>.
 * Return value:
 * on success - 0 */
int arr_pop(struct Array* array);

/* Calculates address of element with position <pos>. 
 * Return value: address of element with position <pos>. */
void* arr_at(const struct Array* array, size_t pos);

/* Returns current count of elements in Array <array> */
size_t arr_get_curr_count(const struct Array* array);

/* Returns max count of elements in Array <array> */
size_t arr_get_max_count(const struct Array* array);

/* Returns address of Array <array> beginning */
void* arr_get_array(const struct Array* array);

#endif
