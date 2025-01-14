#ifndef GDS_ARRAY_H
#define GDS_ARRAY_H

#include <stdlib.h>

#define ARR_ERR_BASE 1000

/* Fields:
* size_t count - current count of elements,
* size_t max_count - array capacity,
* size_t element_size - size of each element,
* void* data - address of array beginning. */
struct GDSArray;

// --------------------------------------------------------------------------------------------------------------------------------------------

/* Dynamically allocates memory for struct GDSArray and initializes it. Dynamically allocates max_count* element_size for array's data.
 * Return value:
 * on success - address of dynamically allocated struct GDSArray. 
 * on failure - NULL: max_count or element_size equals 0 OR malloc() failed for struct GDSArray or array's data. */
struct GDSArray* gds_arr_init(size_t max_count, size_t element_size);

// --------------------------------------------------------------------------------------------------------------------------------------------

/* Frees dynamically allocated memory for data. Sets values of array's fields to default values. If array == NULL, the function performs
 * no action. */
void gds_arr_destruct(struct GDSArray* array);

// --------------------------------------------------------------------------------------------------------------------------------------------

/* Calculates address of element with index specified by pos.
 * Return value:
 * on success: address of element with index specified by pos,
 * on failure: NULL. Function may fail if pos is invalid/out of bounds or if array is NULL. */
void* gds_arr_at(const struct GDSArray* array, size_t pos);

// --------------------------------------------------------------------------------------------------------------------------------------------

#define ARR_ASSIGN_ERR_BASE (ARR_ERR_BASE + 10)
#define ARR_ASSIGN_ERR_ARR_NULL (ARR_ASSIGN_ERR_BASE + 1) // argument array is NULL.
#define ARR_ASSIGN_ERR_DATA_NULL (ARR_ASSIGN_ERR_BASE + 2) // argument data is NULL.
#define ARR_ASSIGN_ERR_POS_OUT_OF_BOUNDS (ARR_ASSIGN_ERR_BASE + 3) // argument pos is out of bounds(greater or equal to array->count)
#define ARR_ASSIGN_ERR_INVALID_ADDR_FOUND (ARR_ASSIGN_ERR_BASE + 4) // gds_arr_at() returned NULL.

/* Copies memory content pointed to by data into the array at index pos. Argument pos must be of lesser value than the count of the specified array's elements.
 * Function relies on gds_arr_at() to find the required address for the following memcpy() call.
 * Return value:
 * on success - 0,
 * on failure - one of the error codes above. */
int gds_arr_assign(struct GDSArray* array, size_t pos, const void* data);

// --------------------------------------------------------------------------------------------------------------------------------------------

#define ARR_INSERT_ERR_BASE (ARR_ERR_BASE + 20)
#define ARR_INSERT_ERR_ARR_NULL (ARR_INSERT_ERR_BASE + 1) // argument array is NULL.
#define ARR_INSERT_ERR_DATA_NULL (ARR_INSERT_ERR_BASE + 2) // argument data is NULL.
#define ARR_INSERT_ERR_POS_OUT_OF_BOUNDS (ARR_ASSIGN_ERR_BASE + 3) // argument pos is out of bounds(greater or equal to array->count)
#define ARR_INSERT_ERR_SHIFTING_OP_FAILED (ARR_INSERT_ERR_BASE + 4) // internal function int _gds_arr_shift_right(struct GDSArray* array, size_t start_idx) failed.
#define ARR_INSERT_ERR_ASSIGN_FAIL (ARR_INSERT_ERR_BASE + 5) // gds_arr_assign() failed.

/* Inserts data pointed to by data to index pos in the array. Performs a call to _arr_shift_right() to make space for the new element at pos.
 * Performs a call to gds_arr_assign() to assign value at the pos index.
 * Return value:
 * on success - 0,
 * on failure - one of the error codes above. */
int gds_arr_insert(struct GDSArray* array, const void* data, size_t pos);

// --------------------------------------------------------------------------------------------------------------------------------------------

#define ARR_APPEND_ERR_BASE (ARR_ERR_BASE + 30)
#define ARR_APPEND_ERR_ARR_NULL (ARR_APPEND_ERR_BASE + 1) // argument array is NULL.
#define ARR_APPEND_ERR_DATA_NULL (ARR_APPEND_ERR_BASE + 2) // argument data is NULL.
#define ARR_APPEND_ERR_ASSIGN_FAIL (ARR_APPEND_ERR_BASE + 5) // gds_arr_assign() failed.

/* Appends data pointed to by data to the end of the array. Performs the call: gds_arr_insert(array, data, array->count);
 * Return value:
 * on success - 0,
 * on failure - one of the error codes above. */
int gds_arr_append(struct GDSArray* array, const void* data);

// --------------------------------------------------------------------------------------------------------------------------------------------

#define ARR_REMOVE_ERR_BASE (ARR_ERR_BASE + 40)
#define ARR_REMOVE_ERR_ARR_NULL (ARR_REMOVE_ERR_BASE + 1) // argument array is NULL.
#define ARR_REMOVE_ERR_SHIFTING_OP_FAILED (ARR_REMOVE_ERR_BASE + 2) // internal function int _gds_arr_shift_left(struct GDSArray* array, size_t start_idx) failed.
#define ARR_REMOVE_ERR_POS_OUT_OF_BOUNDS (ARR_REMOVE_ERR_BASE + 3) // argument pos is out of bounds(greater or equal to array->count)
/* Removes element with position pos from array.
 * Function relies on internal function _gds_arr_shift_left() which shifts elements right of pos(including) leftwards.
 * Return value:
 * on success - 0,
 * on failure - one of the error codes above. */
int gds_arr_remove(struct GDSArray* array, size_t pos);

// --------------------------------------------------------------------------------------------------------------------------------------------

#define ARR_POP_ERR_BASE (ARR_ERR_BASE + 50)
#define ARR_POP_ERR_ARR_NULL (ARR_POP_ERR_BASE + 1) // argument array is NULL.
#define ARR_POP_ERR_ARR_EMPTY (ARR_POP_ERR_BASE + 3) // argument pos is out of bounds(greater or equal to array->count)
/* Removes last element in array by performing a call: gds_arr_remove(array, array->count - 1);
 * Return value:
 * on success - 0,
 * on failure - one of the error codes above. */
int gds_arr_pop(struct GDSArray* array);

// --------------------------------------------------------------------------------------------------------------------------------------------

/* Empties the array.
 * Return value:
 * on success: 0,
 * on failure: 1 - argument 'array' is null. */
int gds_arr_empty(struct GDSArray* array);

// --------------------------------------------------------------------------------------------------------------------------------------------

#define ARR_REALLOC_ERR_BASE (ARR_ERR_BASE + 60)
#define ARR_REALLOC_ERR_ARR_NULL (ARR_REALLOC_ERR_BASE + 1)
#define ARR_REALLOC_ERR_NEW_COUNT_EQ_ZERO (ARR_REALLOC_ERR_BASE + 2)
#define ARR_REALLOC_ERR_REALLOC_FAIL (ARR_REALLOC_ERR_BASE + 3)

/* Performs a realloc() on array's data, so the new data can fit new_max_count elements. If count > new_max_count, the array will shrink.
 * Return value:
 * on success - 0,
 * on failure - one of the error codes above. */
int gds_arr_realloc(struct GDSArray* array, size_t new_max_count);

// --------------------------------------------------------------------------------------------------------------------------------------------

/* Gets current count of elements in array.
 * Return value:
 * on success: current count of elements in array, 
 * on failure: -1 - array is NULL. */
ssize_t gds_arr_get_count(const struct GDSArray* array);

// --------------------------------------------------------------------------------------------------------------------------------------------

/* Gets max count of elements in array.
 * Return value:
 * on success: max count of elements in array, 
 * on failure: -1 - array is NULL. */
ssize_t gds_arr_get_max_count(const struct GDSArray* array);

// --------------------------------------------------------------------------------------------------------------------------------------------

/* Gets address of data in array.
 * Return value:
 * on success: address of array's data field,
 * on failure: NULL - array is NULL.*/
void* gds_arr_get_data(const struct GDSArray* array);

// --------------------------------------------------------------------------------------------------------------------------------------------

/* Performs sizeof(struct GDSArray) and returns the value */
size_t gds_arr_get_struct_size();

// --------------------------------------------------------------------------------------------------------------------------------------------

#endif
