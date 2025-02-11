#ifndef _GDS_VECTOR_H_
#define _GDS_VECTOR_H_

#include <stdlib.h>
#include <stdbool.h>

#include "gds.h"

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
struct GDSVector;
#else
#define __GDS_VECTOR_DEF_ALLOW__
#include "def/gds_vector_def.h"
#endif // GDS_ENABLE_OPAQUE_STRUCTS

typedef struct GDSVector GDSVector;

// ------------------------------------------------------------------------------------------------------------------------------------------

#define GDS_VEC_ERR_BASE 3000
#define GDS_VEC_ERR_VEC_EMPTY 3001
#define GDS_VEC_ERR_MALLOC_FAIL 3002
#define GDS_VEC_ERR_REALLOC_FAIL 3003
#define GDS_VEC_ERR_INIT_FAIL 3004
#define GDS_VEC_ERR_CHUNK_GEN_FUNC_FAIL 3005 // Occurs when vector->_get_next_chunk_size_func returns 0

// ------------------------------------------------------------------------------------------------------------------------------------------

/* Initializes GDSVector vector. Used when opaque structs are disabled. May also be used for initializing
 * an vector after its destruction. Dynamically allocates a chunk of memory to hold 'capacity' amount of elements.
 * The vector's capacity will never go below the 'min_capacity' value.
 * Return value:
 * on success - GDS_SUCCESS,
 * on failure - one of the generic error codes representing an invalid argument, GDS_VEC_ERR_MALLOC_FAIL or
 * GDS_VEC_ERR_INIT_FAIL. Function may fail: if 'vector is NULL', if 'element_size' == 0, if
 * 'min_capacity' == 0, if '_get_next_chunk_size_func' == NULL. Function may also return 
 * GDS_ARR_ERR_MALLOC_FAIL if dynamic allocation for the vector's data fails. Function may also
 * return GDS_VEC_ERR_INIT_FAIL if initializing of internal GDSVector or _GDSVectorChunkList fails. */
gds_err gds_vector_init(GDSVector* vector,
        size_t element_size,
        void (*on_element_removal_func)(void*),
        size_t min_capacity,
        size_t (*_get_next_chunk_size_func)(struct GDSVector* vector, size_t last_chunk_size));

// ---------------------------------------------------------------------------------------------------------------------


/* Dynamically allocates memory for GDSVector. Calls gds_vector_init() to initialize the newly created vector.
 * Return value:
 * on success - address of dynamically allocated GDSVector. 
 * on failure - NULL. The function can fail because: allocating memory for the new vector failed, or because
 * gds_vector_init() returned an error code. */
GDSVector* gds_vector_create(size_t element_size,
        void (*on_element_removal_func)(void*),
        size_t min_capacity,
        size_t (*get_next_chunk_size_func)(struct GDSVector* vector, size_t last_chunk_size));

// ---------------------------------------------------------------------------------------------------------------------

/* Frees dynamically allocated memory for vector. Sets values of vector's fields to default values.
 * If vector == NULL, the function performs no action. This doesn't free memory pointed to by 'vector'.
 * This function invokes the appropriate destructors for the data structures it uses. */
void gds_vector_destruct(GDSVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

/* Calculates address of element with index specified by 'pos'.
 * Return value:
 * on success: address of element with index specified by 'pos',
 * on failure: NULL. Function may fail if 'pos' is invalid/out of bounds or if vector is NULL. */
void* gds_vector_at(const GDSVector* vector, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

/* Copies memory content pointed to by data into the vector at 'pos'.
 * Function relies on gds_vector_at() to find the required address for the following memcpy() call.
 * Return value:
 * on success - GDS_SUCCESS,
 * on failure - one of the generic error codes representing an invalid argument.
 * Function may fail if 'vector' or 'data' are NULL or 'pos' is out of bounds('pos' >= vector's count). */
gds_err gds_vector_assign(const GDSVector* vector, const void* data, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

/* Swaps the data of elements at pos1 and pos2. If 'pos1' == 'pos2', the function performs no action.
 * on success - GDS_SUCCESS,
 * on failure - one of the generic error codes representing an invalid argument.
 * Function may fail if 'vector' is NULL or 'pos1' or 'pos2' are out of bounds('pos' >= vector's count). */
gds_err gds_vector_swap(const GDSVector* vector, size_t pos1, size_t pos2);

// ---------------------------------------------------------------------------------------------------------------------

/* Appends data pointed to by data to the end of the vector.
 * Performs the call: gds_vector_insert_at(vector, data, vector's count).
 * If the vector is at its capacity, the vector will attempt
 * to perform realloc(). The size argument used for realloc() is determined by the vector's _get_next_chunk_size_func
 * (it will be equal to the current vector capacity + the value returned by the called function).
 * Return value:
 * on success - GDS_SUCCESS,
 * on failure - one of the generic error codes representing an invalid argument or GDS_VEC_ERR_REALLOC_FAIL.
 * Function may fail if 'vector' or 'data' are NULL or if the realloc() call fails. If that happens, the vector's
 * capacity and count will remain unchanged - the new element won't be appended. */
gds_err gds_vector_push_back(GDSVector* vector, const void* data);

// ---------------------------------------------------------------------------------------------------------------------

/* Inserts data pointed to by data to index pos in the vector. This is done by shifting all elements with
 * index greater or equal than 'pos' rightward(through a memmove() call), and inserting the element at the empty spot.
 * If pos == vector's count, no shifting is performed.
 * If the vector is at its capacity, the vector will attempt
 * to perform realloc(). The size argument used for realloc() is determined by the vector's _get_next_chunk_size_func
 * (it will be equal to the current vector capacity + the value returned by the called function).
 * Return value:
 * on success - GDS_SUCCESS,
 * on failure - one of the generic error codes representing an invalid argument or GDS_VEC_ERR_REALLOC_FAIL.
 * Function may fail if 'vector' or 'data' are NULL or if the realloc() call fails or if 'pos' is out of bounds.
 * If a realloc() fail happens, the vector's capacity and count will remain unchanged - the new element won't be inserted. */
gds_err gds_vector_insert_at(GDSVector* vector, const void* data, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

/* Removes last element in vector by performing a call: gds_vector_remove_at(vector, vector's count - 1).
 * If, when removing an element, the removal of the element leaves the last allocated 'chunk' of memory for the vector's
 * data empty, an attempt to shrink the vector's data(via realloc()) will be made. The vector will be shrank
 * by the amount equal to the size of the last allocated chunk(this amount is determined by the last call
 * of vector->_get_next_chunk_size_func).
 * This function will invoke vector->_on_element_removal_func for the removed element.
 * The vector will always retain capacity for the amount of elements passed to it in its init/create function.
 * Return value:
 * on success - GDS_SUCCESS,
 * on failure - one of the generic error codes representing an invalid argument('vector' is NULL), 
 * GDS_VEC_ERR_VEC_EMPTY(indicating the vector is already empty) or GDS_VEC_ERR_REALLOC_FAIL if the shrinking failed.
 * If shrinking fails, the vector's capacity will remain unchanged. It is possible to decrease the capacity by the
 * appropriate amount by calling gds_vector_fit() afterwards. */
gds_err gds_vector_pop_back(GDSVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

/* Removes element with position 'pos' from vector. This is done by shifting all elements with index greater than 'pos'
 * leftwards through a memmove() call.
 * If, when removing an element, the removal of the element leaves the last allocated 'chunk' of memory for the vector's
 * data empty, an attempt to shrink the vector's data(via realloc()) will be made. The vector will be shrank
 * by the amount equal to the size of the last allocated chunk(this amount is determined by the last call
 * of vector->_get_next_chunk_size_func).
 * This function will invoke vector->_on_element_removal_func for the removed element.
 * The vector will always retain capacity for the amount of elements passed to it in its init/create function.
 * Return value:
 * on success - GDS_SUCCESS,
 * on failure - one of the generic error codes representing an invalid argument or GDS_VEC_ERR_REALLOC_FAIL.
 * If shrinking fails, the vector's capacity will remain unchanged. It is possible to decrease the capacity by the
 * appropriate amount by calling gds_vector_fit() afterwards.
 * Function may fail if 'vector' is NULL or 'pos' is out of bounds('pos' >= vector's count).
 * If the vector is empty, the function treats 'pos' as an invalid argument and returns the appropriate code. */
gds_err gds_vector_remove_at(GDSVector* vector, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

// TODO - check for realloc() fails.
/* Empties the vector. If the vector is already empty, the function performs no work and returns GDS_SUCESS.
 * This function will invoke vector->_on_element_removal_func for each removed element.
 * The vector will always retain capacity for the amount of elements passed to it in its init/create function.
 * Return value:
 * on success: GDS_SUCCESS,
 * on failure: one of the generic error codes representing an invalid argument('vector' is NULL). */
gds_err gds_vector_empty(GDSVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

/* Function will preallocate a new chunk of memory for the vector to store its elements. This will result in a realloc()
 * call that will expand the vector and increase its capacity. Argument 'new_chunk_size' may not be 0.
 * Return value:
 * on success: GDS_SUCCESS,
 * on failure: one of the generic error codes representing an invalid argument or GDS_VEC_ERR_REALLOC_FAIL.
 * Function may fail if 'vector' is NULL or 'new_chunk_size' == 0 or realloc() fails to accomodate the new capacity.
 * If the realloc fails, the vector will retain its old capacity. */
gds_err gds_vector_prealloc(GDSVector* vector, size_t new_chunk_size);

// ---------------------------------------------------------------------------------------------------------------------

/* Function will shrink the vector's capacity so it can exactly fit its count. This will result in a realloc() call.
 * Return value:
 * on success: GDS_SUCCESS,
 * on failure: one of the generic error codes representing an invalid argument or GDS_VEC_ERR_REALLOC_FAIL.
 * If the realloc fails, the vector will retain its old capacity. */
gds_err gds_vector_fit(GDSVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

/* Gets current count of elements in vector. Assumes non-NULL argument. */
size_t gds_vector_get_count(const GDSVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

/* Gets current capacity of vector. Assumes non-NULL argument. */
size_t gds_vector_get_capacity(const GDSVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

/* Checks if the vector is empty. Assumes non-NULL argument. */
bool gds_vector_is_empty(const GDSVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

/* Returns element size of vector. Assumes non-NULL argument. */
size_t gds_vector_get_element_size(const GDSVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

/* Performs sizeof(GDSArray) and returns the value. */
size_t gds_vector_get_struct_size();

// ------------------------------------------------------------------------------------------------------------------------------------------

#endif // _GDS_VECTOR_H_
