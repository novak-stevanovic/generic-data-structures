#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>

/* Fields:
* size_t count - current count of elements
* size_t max_count - array capacity
* size_t element_size - size of each element
* int data_alloc_type - array alloc type(each array may be allocated statically or dynamically)
* void* data - address of array beginning */
struct Array;

#define ALLOC_TYPE_UNALLOCED 0
#define ALLOC_TYPE_STATIC 1
#define ALLOC_TYPE_DYNAMIC 2

/* Dynamically allocates memory for struct Array and initializes it: 
 * struct Array will have max_count = <max_count> elements, each the size of <element_size>. <array>'s array field will point to the provided statically allocated array <static_arr_ptr>.
 * Return value:
 * on success - address of dynamically allocated struct Array 
 * on failure - NULL - failure to malloc() for struct Array */
struct Array* arr_init_static(size_t max_count, size_t element_size, void* static_arr_ptr);

/* Dynamically allocates memory for struct Array and initializes it: 
 * struct Array will have max_count = <max_count> elements, each the size of <element_size>. <array>'s array field will point to a dynamically allocated chunk of memory of adequate size.
 * Return value:
 * on success - address of dynamically allocated struct Array 
 * on failure - NULL - failure to malloc() for struct Array or malloc for <array>->data */
struct Array* arr_init_dynamic(size_t max_count, size_t element_size);

/* Frees dynamically allocated memory for <data> if needed. Frees dynamically allocated memory for <array>. */
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

/* Calculates address of element with index specified by <pos>.
 * Return value:
 * on success: address of element with index specified by <pos> 
 * on faulure: NULL */
void* arr_at(const struct Array* array, size_t pos);

/* Returns current count of elements in <array> */
size_t arr_get_count(const struct Array* array);

/* Returns max count of elements in <array> */
size_t arr_get_max_count(const struct Array* array);

/* Returns address of data in <array> */
void* arr_get_data(const struct Array* array);


#endif
