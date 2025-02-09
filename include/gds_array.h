#ifndef _GDS_ARRAY_H_
#define _GDS_ARRAY_H_

#include "gds.h"
#include <stdlib.h>

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
struct GDSArray;
#else
#define __GDS_ARRAY_DEF_ALLOW__
#include "def/gds_array_def.h"
#endif

typedef struct GDSArray GDSArray;

// ------------------------------------------------------------------------------------------------------------------------------------------

#define GDS_ARR_ERR_BASE 2000
#define GDS_ARR_ERR_INSUFF_CAPACITY 2001
#define GDS_ARR_ERR_ARR_EMPTY 2002
#define GDS_ARR_ERR_MALLOC_FAIL 2003
#define GDS_ARR_ERR_REALLOC_FAIL 2004

// ------------------------------------------------------------------------------------------------------------------------------------------

/* Dynamically allocates memory for GDSArray and initializes it. Dynamically allocates max_count * element_size for array's data.
 * Return value:
 * on success - address of dynamically allocated GDSArray. 
 * on failure - NULL: max_count or element_size equals 0 OR malloc() failed for GDSArray or array's data. */
GDSArray* gds_arr_create(size_t max_count, size_t element_size, void (*on_element_removal_func)(void*));

// ------------------------------------------------------------------------------------------------------------------------------------------

/* Initializes GDSArray array. Used when opaque structs are disabled. Dynamically allocates max_count * element_size for array's data. 
 * Return value:
 * on success - 0,
 * on failure: one of gds generic error codes. */
#ifndef GDS_ENABLE_OPAQUE_STRUCTS
gds_err gds_arr_init(GDSArray* array, size_t max_count, size_t element_size, void (*on_element_removal_func)(void*));
#endif

// ----------------------------------------------------------------------------------------------------------------------------------------

/* Frees dynamically allocated memory for data. Sets values of array's fields to default values. If array == NULL,
 * the function performs no action. */
void gds_arr_destruct(GDSArray* array);

// ----------------------------------------------------------------------------------------------------------------------------------------

/* Calculates address of element with index specified by pos.
 * Return value:
 * on success: address of element with index specified by pos,
 * on failure: NULL. Function may fail if pos is invalid/out of bounds or if array is NULL. */
void* gds_arr_at(const GDSArray* array, size_t pos);

// ----------------------------------------------------------------------------------------------------------------------------------------

/* Copies memory content pointed to by data into the array at index pos. Argument pos must be of lesser value than the count of the specified array's elements.
 * Function relies on gds_arr_at() to find the required address for the following memcpy() call.
 * Return value:
 * on success - 0,
 * on failure - one of gds generic error codes. */
gds_err gds_arr_assign(GDSArray* array, size_t pos, const void* data);

// -----------------------------------------------------------------------------------------------------------------------------------------

/* Inserts data pointed to by data to index pos in the array. Performs a call to _arr_shift_right() to make space for the new element at pos.
 * Performs a call to gds_arr_assign() to assign value at the pos index.
 * Return value:
 * on success - 0,
 * on failure - one of gds generic error codes, or array generic codes.*/
gds_err gds_arr_insert(GDSArray* array, const void* data, size_t pos);

// -----------------------------------------------------------------------------------------------------------------------------------------

/* Appends data pointed to by data to the end of the array. Performs the call: gds_arr_insert(array, data, array->count);
 * Return value:
 * on success - 0,
 * on failure - one of gds generic error codes, or array generic codes.*/
gds_err gds_arr_append(GDSArray* array, const void* data);

// -----------------------------------------------------------------------------------------------------------------------------------------

/* Removes element with position pos from array.
 * Function relies on internal function _gds_arr_shift_left() which shifts elements right of pos(including) leftwards.
 * Return value:
 * on success - 0,
 * on failure - one of gds generic error codes, or array generic codes.*/
gds_err gds_arr_remove(GDSArray* array, size_t pos);

// -----------------------------------------------------------------------------------------------------------------------------------------

/* Removes last element in array by performing a call: gds_arr_remove(array, array->count - 1);
 * Return value:
 * on success - 0,
 * on failure - one of gds generic error codes, or array generic codes.*/
gds_err gds_arr_pop(GDSArray* array);

// -----------------------------------------------------------------------------------------------------------------------------------------

// /* Sets the count of elements of array to new_count. If the array's count is:
//  * 1. greater than new_size - the array will be shrank to the new size.
//  * 2. lesser than new_size - array will be expanded to the new size. This means that the provided function assign_func() will be called repeatedly(for each
//  * element added) until array->count = new_count. This function accepts 2 params: 1. address of the newly-added element in the array, 2. address of the data argument.
//  * This allows the caller of the function to perform some action on the chunk of memory in the array, for each newly-added element(initializing, assigning value... etc).
//  * Data parameter allows the caller to pass data into the assign_func.
//  * 3. equal to new_size - array will remain unchanged.
//  * If the function is guaranteed to shrink the array, argument default_val may be NULL.
//  * Return value:
//  * on success: 0,
//  * on failure: one of gds generic error codes. */
// gds_err gds_arr_set_size(GDSArray* array, size_t new_count, void (*assign_func)(void*, void*), void* data);

// -----------------------------------------------------------------------------------------------------------------------------------------

/* Empties the array by calling gds_arr_set_size(array, 0, NULL).
 * Return value:
 * on success: 0,
 * on failure: 1 - argument 'array' is null. 2 - invoked function failed. */
gds_err gds_arr_empty(GDSArray* array);

// -----------------------------------------------------------------------------------------------------------------------------------------

/* Performs a realloc() on array's data, so the new data can fit new_max_count elements. If count > new_max_count, the array will shrink.
 * Keep in mind that this will not perform any calls to array->on_element_removal_func(void*). Use a variant of gds_arr_set_size() instead.
 * Return value:
 * on success - 0,
 * on failure - one of gds generic error codes or array generic codes. */
gds_err gds_arr_realloc(GDSArray* array, size_t new_max_count);

// -----------------------------------------------------------------------------------------------------------------------------------------

/* Gets current count of elements in array.
 * Return value:
 * on success: current count of elements in array, 
 * on failure: -1 - array is NULL. */
ssize_t gds_arr_get_count(const GDSArray* array);

// -----------------------------------------------------------------------------------------------------------------------------------------

/* Checks if the array is empty. 
 * Return value:
 * on success: 0 indicating the array is not empty, 1 indicating the array is empty.
 * on failure: -1 - indicating the passed array argument is NULL. */
int gds_arr_is_empty(const GDSArray* array);

// -----------------------------------------------------------------------------------------------------------------------------------------

/* Gets max count of elements in array.
 * Return value:
 * on success: max count of elements in array, 
 * on failure: -1 - array is NULL. */
ssize_t gds_arr_get_max_count(const GDSArray* array);

// -----------------------------------------------------------------------------------------------------------------------------------------

/* Gets address of data in array.
 * Return value:
 * on success: address of array's data field,
 * on failure: NULL - array is NULL.*/
void* gds_arr_get_data(const GDSArray* array);

// -----------------------------------------------------------------------------------------------------------------------------------------

/* Returns element size of array.
 * Return value:
 * on success: value greater than 0, representing the value of element_size field in array,
 * on failure: 0 - argument array is NULL. */
size_t gds_arr_get_element_size(const GDSArray* array);

// -----------------------------------------------------------------------------------------------------------------------------------------

/* Performs sizeof(GDSArray) and returns the value */
size_t gds_arr_get_struct_size();

// ------------------------------------------------------------------------------------------------------------------------------------------

#endif
