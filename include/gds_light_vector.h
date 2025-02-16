#ifndef _GDS_LIGHT_VECTOR_H_
#define _GDS_LIGHT_VECTOR_H_

#include <stdlib.h>
#include <stdbool.h>

#include "gds.h"

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
struct GDSLightVector;
#else
#define __GDS_LIGHT_VECTOR_DEF_ALLOW__
#include "def/gds_light_vector_def.h"
#endif

typedef struct GDSLightVector GDSLightVector;

// ------------------------------------------------------------------------------------------------------------------------------------------

#define GDS_VEC_ERR_BASE 200
#define GDS_VEC_ERR_VEC_EMPTY 201
#define GDS_VEC_ERR_MALLOC_FAIL 202
#define GDS_VEC_ERR_REALLOC_FAIL 203
#define GDS_VEC_ERR_INIT_FAIL 204

// ------------------------------------------------------------------------------------------------------------------------------------------

/* Initializes GDSLightVector vector. Used when opaque structs are disabled. May also be used for initializing
 * an vector after its destruction. Dynamically allocates a chunk of memory to hold 'capacity' amount of elements.
 * Return value:
 * on success - GDS_SUCCESS,
 * on failure - one of the generic error codes representing an invalid argument, GDS_VEC_ERR_MALLOC_FAIL or
 * GDS_VEC_ERR_INIT_FAIL. Function may fail: if 'vector is NULL', if 'element_size' == 0, if 'initial_capacity' == 0.
 * Function may also return GDS_ARR_ERR_MALLOC_FAIL if dynamic allocation for the vector's data fails. Function may also
 * return GDS_VEC_ERR_INIT_FAIL if initializing of internal GDSLightArray.
 * Note: As any other init function, the function may fail if 'data_size' exceeds GDS_INIT_MAX_SIZE. This macro,
 * if defined, is defined in gds.h. */
gds_err gds_light_vector_init(GDSLightVector* vector, size_t element_size, size_t initial_capacity, double resize_factor);

// ---------------------------------------------------------------------------------------------------------------------


/* Dynamically allocates memory for GDSLightVector. Calls gds_light_vector_init() to initialize the newly created vector.
 * Return value:
 * on success - address of dynamically allocated GDSLightVector. 
 * on failure - NULL. The function can fail because: allocating memory for the new vector failed, or because
 * gds_light_vector_init() returned an error code. */
GDSLightVector* gds_light_vector_create(size_t element_size, size_t initial_capacity, double resize_factor);

// ---------------------------------------------------------------------------------------------------------------------

/* Frees dynamically allocated memory for vector. Sets values of vector's fields to default values.
 * If vector == NULL, the function performs no action. This doesn't free memory pointed to by 'vector'.
 * This function invokes the appropriate destructors for the data structures it uses. */
void gds_light_vector_destruct(GDSLightVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

/* Calculates address of element with index specified by 'pos'.
 * Return value:
 * on success: address of element with index specified by 'pos',
 * on failure: NULL. Function may fail if 'pos' is invalid/out of bounds or if vector is NULL. */
void* gds_light_vector_at(const GDSLightVector* vector, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

/* Copies memory content pointed to by data into the vector at 'pos'.
 * Function relies on gds_light_vector_at() to find the required address for the following memcpy() call.
 * Return value:
 * on success - GDS_SUCCESS,
 * on failure - one of the generic error codes representing an invalid argument.
 * Function may fail if 'vector' or 'data' are NULL or 'pos' is out of bounds('pos' >= vector's count). */
gds_err gds_light_vector_assign(GDSLightVector* vector, const void* data, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

/* Swaps the data of elements at pos1 and pos2. If 'pos1' == 'pos2', the function performs no action.
 * on success - GDS_SUCCESS,
 * on failure - one of the generic error codes representing an invalid argument.
 * Function may fail if 'vector' is NULL or 'pos1' or 'pos2' are out of bounds('pos' >= vector's count). */
gds_err gds_light_vector_swap(GDSLightVector* vector, size_t pos1, size_t pos2);

// ---------------------------------------------------------------------------------------------------------------------

/* Appends data pointed to by data to the end of the vector. Performs the call:
 * gds_light_vector_insert_at(vector, data, vector's count). If the vector is at its capacity,
 * This action may invoke realloc() to expand the vector - if at max capacity.
 * Return value:
 * on success - GDS_SUCCESS,
 * on failure - one of the generic error codes representing an invalid argument or GDS_VEC_ERR_REALLOC_FAIL.
 * Function may fail if 'vector' or 'data' are NULL or if the realloc() call fails. If that happens, the vector's
 * capacity and count will remain unchanged - the new element won't be appended. */
gds_err gds_light_vector_push_back(GDSLightVector* vector, const void* data);

// ---------------------------------------------------------------------------------------------------------------------

/* Inserts data pointed to by data to index pos in the vector. This is done by shifting all elements with
 * index greater or equal than 'pos' rightward(through a memmove() call), and inserting the element at the empty spot.
 * If pos == vector's count, no shifting is performed.
 * This action may invoke realloc() to expand the vector - if at max capacity.
 * Return value:
 * on success - GDS_SUCCESS,
 * on failure - one of the generic error codes representing an invalid argument or GDS_VEC_ERR_REALLOC_FAIL.
 * Function may fail if 'vector' or 'data' are NULL or if the realloc() call fails or if 'pos' is out of bounds.
 * If a realloc() fail happens, the vector's capacity and count will remain unchanged - the new element won't be inserted. */
gds_err gds_light_vector_insert_at(GDSLightVector* vector, const void* data, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

/* Removes last element in vector by performing a call: gds_light_vector_remove_at(vector, vector's count - 1).
 * This action may invoke realloc() to shrink the vector.
 * Return value:
 * on success - GDS_SUCCESS,
 * on failure - one of the generic error codes representing an invalid argument('vector' is NULL), 
 * GDS_VEC_ERR_VEC_EMPTY(indicating the vector is already empty) or GDS_VEC_ERR_REALLOC_FAIL if the shrinking failed.
 * If shrinking fails, the vector's capacity will remain unchanged. It is possible to decrease the capacity by the
 * appropriate amount by calling gds_light_vector_fit() afterwards. */
gds_err gds_light_vector_pop_back(GDSLightVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

/* Removes element with position 'pos' from vector. This is done by shifting all elements with index greater than 'pos'
 * leftwards through a memmove() call.
 * This action may invoke realloc() to shrink the vector.
 * Return value:
 * on success - GDS_SUCCESS,
 * on failure - one of the generic error codes representing an invalid argument or GDS_VEC_ERR_REALLOC_FAIL.
 * If shrinking fails, the vector's capacity will remain unchanged. It is possible to decrease the capacity by the
 * appropriate amount by calling gds_light_vector_fit() afterwards.
 * Function may fail if 'vector' is NULL or 'pos' is out of bounds('pos' >= vector's count).
 * If the vector is empty, the function treats 'pos' as an invalid argument and returns the appropriate code. */
gds_err gds_light_vector_remove_at(GDSLightVector* vector, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

// TODO - check for realloc() fails.
/* Empties the vector. If the vector is already empty, the function performs no work and returns GDS_SUCCESS.
 * Return value:
 * on success: GDS_SUCCESS,
 * on failure: one of the generic error codes representing an invalid argument('vector' is NULL). */
gds_err gds_light_vector_empty(GDSLightVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

/* Reserves enough memory to fit 'new_capacity' elements. If 'new_capacity' < vector's current capacity - the
 * function returns an invalid argument error code. If 'new_capacity' == vector's current capacity -
 * the function performs nothing. This will perform a realloc() call so the vector can be expanded.
 * This function will effectively disable dynamic shrinking until gds_light_vector_fit() has been performed. 
 * Return value:
 * on success: GDS_SUCCESS,
 * on failure: one of the generic error codes representing an invalid argument or GDS_VEC_ERR_REALLOC_FAIL.*/
gds_err gds_light_vector_reserve(GDSLightVector* vector, size_t new_capacity);

// ---------------------------------------------------------------------------------------------------------------------

/* Function will shrink the vector's capacity so it can exactly fit its count. If the vector's capacity == vector's
 * count, the function performs no work. This will result in a realloc() call. A call to this function
 * will re-enable dynamic vector shrinking.
 * Return value:
 * on success: GDS_SUCCESS,
 * on failure: one of the generic error codes representing an invalid argument or GDS_VEC_ERR_REALLOC_FAIL.
 * If the realloc fails, the vector will retain its old capacity. */
gds_err gds_light_vector_fit(GDSLightVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

/* Gets current count of elements in vector. Assumes non-NULL argument. */
size_t gds_light_vector_get_count(const GDSLightVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

/* Gets current capacity of vector. Assumes non-NULL argument. */
size_t gds_light_vector_get_capacity(const GDSLightVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

/* Checks if the vector is empty. Assumes non-NULL argument. */
bool gds_light_vector_is_empty(const GDSLightVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

/* Returns element size of vector. Assumes non-NULL argument. */
size_t gds_light_vector_get_element_size(const GDSLightVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

/* Performs sizeof(GDSLightVector) and returns the value. */
size_t gds_light_vector_get_struct_size();

// ------------------------------------------------------------------------------------------------------------------------------------------

#endif // _GDS_LIGHT_VECTOR_H_
