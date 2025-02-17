#ifndef _GDSE_VECTOR_H_
#define _GDSE_VECTOR_H_

#include <stdlib.h>
#include <stdbool.h>

#include "gds.h"

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
struct GDSEVector;
#else
#define __GDSE_VECTOR_DEF_ALLOW__
#include "def/gdse_vector_def.h"
#endif

typedef struct GDSEVector GDSEVector;

// ------------------------------------------------------------------------------------------------------------------------------------------

#define GDSE_VEC_ERR_BASE 2000
#define GDSE_VEC_ERR_MALLOC_FAIL 2001
#define GDSE_VEC_ERR_NO_COMPARE_FUNC 2002

// ------------------------------------------------------------------------------------------------------------------------------------------

/* Initializes GDSEVector by first initializing the underlying vector and then the GDSEVector fields. 
 * Return value: 
 * on success: GDS_SUCCESS,
 * on failure: one of the generic error codes indicating an invalid argument or GDSE_VEC_ERR_MALLOC_FAIL.
 * Function may fail if given an invalid argument for the initialization of the underlying vector. Check gds_vector.h for
 * valid init arguments. */
gds_err gdse_vector_init(GDSEVector* vector, size_t element_size,
        size_t initial_capacity, double resize_factor, 
        bool (*compare_func)(void*, void*),
        void (*on_removal_callback_func)(void*));

// ---------------------------------------------------------------------------------------------------------------------

/* Dynamically allocates enough memory for one GDSEVector.
 * Initializes GDSEVector by first initializing the underlying vector and then the GDSEVector fields. 
 * Return value: 
 * on success: GDS_SUCCESS,
 * on failure: one of the generic error codes indicating an invalid argument or GDSE_VEC_ERR_MALLOC_FAIL.
 * Function may fail if given an invalid argument for the initialization of the underlying vector. Check gds_vector.h for
 * valid init arguments. */
GDSEVector* gdse_vector_create(size_t element_size, size_t initial_capacity,
        double resize_factor,
        bool (*compare_func)(void*, void*),
        void (*on_removal_callback_func)(void*));

// ---------------------------------------------------------------------------------------------------------------------

void gdse_vector_destruct(GDSEVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

/* Swaps the data of elements at pos1 and pos2. If 'pos1' == 'pos2', the function performs no action.
 * on success - GDS_SUCCESS,
 * on failure - one of the generic error codes representing an invalid argument.
 * Function may fail if 'vector' is NULL or 'pos1' or 'pos2' are out of bounds('pos' >= vector's count). */
gds_err gdse_array_swap(GDSEVector* vector, size_t pos1, size_t pos2);

// ---------------------------------------------------------------------------------------------------------------------

/* Sets vector's _compare_func field. This function assumes a non-NULL 'vector' argument */
void gdse_vector_set_compare_func(GDSEVector* vector, bool (*compare_func)(void*, void*));

// ---------------------------------------------------------------------------------------------------------------------

/* Sets vector's _on_removal_callback_func field. This function assumes a non-NULL 'vector' argument */
void gdse_vector_set_on_removal_callback_func(GDSEVector* vector, void (*on_removal_callback_func)(void*));

// ---------------------------------------------------------------------------------------------------------------------

/* Performs sizeof(GDSEVector) and returns the value. */
size_t gdse_vector_get_struct_size();

// ------------------------------------------------------------------------------------------------------------------------------------------

#endif // _GDSE_VECTOR_H_
