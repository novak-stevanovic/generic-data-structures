#ifndef _GDS_ARRAY_H_
#define _GDS_ARRAY_H_

#include <stdlib.h>
#include <stdbool.h>

#include "gds.h"

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

/* Initializes GDSArray array. Used when opaque structs are disabled. May also be used
 * for initializing an array after its destruction.
 * Dynamically allocates capacity * element_size for array's data. 
 * Return value:
 * on success - 0,
 * on failure: 1.  */
gds_err gds_array_init(GDSArray* array, size_t capacity, size_t element_size, void (*on_element_removal_func)(void*));

// ---------------------------------------------------------------------------------------------------------------------

/* Dynamically allocates memory for GDSArray and initializes it. Dynamically allocates capacity * element_size for array's data.
 * Return value:
 * on success - address of dynamically allocated GDSArray. 
 * on failure - NULL: capacity or element_size equals 0 OR malloc() failed for GDSArray or array's data. */
GDSArray* gds_array_create(size_t capacity, size_t element_size, void (*on_element_removal_func)(void*));

// ---------------------------------------------------------------------------------------------------------------------

/* Frees dynamically allocated memory for data. Sets values of array's fields to default values. If array == NULL,
 * the function performs no action. */
void gds_array_destruct(GDSArray* array);

// ---------------------------------------------------------------------------------------------------------------------

/* Calculates address of element with index specified by pos.
 * Return value:
 * on success: address of element with index specified by pos,
 * on failure: NULL. Function may fail if pos is invalid/out of bounds or if array is NULL. */
void* gds_array_at(const GDSArray* array, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

/* Copies memory content pointed to by data into the array at index pos.
 * Argument pos must be of lesser value than the count of the specified array's elements.
 * Function relies on gds_array_at() to find the required address for the following memcpy() call.
 * Return value:
 * on success - 0,
 * on failure - one of gds generic error codes. */
gds_err gds_array_assign(const GDSArray* array, const void* data, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

/* Swaps the data of elements at pos1 and pos2. 
 * on success - 0,
 * on failure - one of gds generic error codes. */
gds_err gds_array_swap(const GDSArray* array, size_t pos1, size_t pos2);

// ---------------------------------------------------------------------------------------------------------------------

/* Appends data pointed to by data to the end of the array. Performs the call: gds_array_insert_at(array, data, array->count);
 * Return value:
 * on success - 0,
 * on failure - one of gds generic error codes, or array generic codes.*/
gds_err gds_array_push_back(GDSArray* array, const void* data);

// ---------------------------------------------------------------------------------------------------------------------

/* Inserts data pointed to by data to index pos in the array. Performs a call to _gds_array_shift_right()
 * to make space for the new element at pos. Performs a call to gds_array_assign() to assign value at the pos index.
 * Return value:
 * on success - 0,
 * on failure - one of gds generic error codes, or array generic codes.*/
gds_err gds_array_insert_at(GDSArray* array, const void* data, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

/* Removes element with position pos from array.
 * Function relies on internal function _gds_array_shift_left() which shifts elements right of pos(including) leftwards.
 * Return value:
 * on success - 0,
 * on failure - one of gds generic error codes, or array generic codes.*/
gds_err gds_array_remove_at(GDSArray* array, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

/* Removes last element in array by performing a call: gds_array_remove_at(array, array->count - 1);
 * Return value:
 * on success - 0,
 * on failure - one of gds generic error codes, or array generic codes.*/
gds_err gds_array_pop_back(GDSArray* array);

// ---------------------------------------------------------------------------------------------------------------------

/* Empties the array.
 * Return value:
 * on success: 0,
 * on failure: 1 - argument 'array' is null. 2 - invoked function failed. */
gds_err gds_array_empty(GDSArray* array);

// ---------------------------------------------------------------------------------------------------------------------

// TODO special doc
/* Performs a realloc() on array's data, so the new data can fit capacity elements. 
 * If count > capacity, the array will shrink - array->on_element_removal_func() calls will be invoked.
 * New capacity must be greater than 0.
 * Return value:
 * on success - 0,
 * on failure - one of gds generic error codes or array generic codes. */
gds_err gds_array_realloc(GDSArray* array, size_t capacity);

// ---------------------------------------------------------------------------------------------------------------------

/* Gets current count of elements in array. Assumes non-NULL argument. */
size_t gds_array_get_count(const GDSArray* array);

// ---------------------------------------------------------------------------------------------------------------------

/* Checks if the array is empty. 
 * This function doesn't return an error if array == NULL, so the caller must make sure that 'array' argument is non-NULL.
 * In the case that the user sends a NULL pointer, function returns true. */
bool gds_array_is_empty(const GDSArray* array);

// ---------------------------------------------------------------------------------------------------------------------

/* Gets capacity of array. Assumes non-NULL argument. */
size_t gds_array_get_capacity(const GDSArray* array);

// ---------------------------------------------------------------------------------------------------------------------

/* Returns element size of array. ASsumes non-NULL argument. */
size_t gds_array_get_element_size(const GDSArray* array);

// ---------------------------------------------------------------------------------------------------------------------

/* Performs sizeof(GDSArray) and returns the value. */
size_t gds_array_get_struct_size();

// ------------------------------------------------------------------------------------------------------------------------------------------

#endif
