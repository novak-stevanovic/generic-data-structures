#ifndef _GDSE_ARRAY_H_
#define _GDSE_ARRAY_H_

#include <stdlib.h>
#include <stdbool.h>

#include "gds.h"

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
struct GDSEArray;
#else
#define __GDSE_ARRAY_DEF_ALLOW__
#include "def/gdse_array_def.h"
#endif

typedef struct GDSEArray GDSEArray;

// ------------------------------------------------------------------------------------------------------------------------------------------

#define GDSE_ARR_ERR_BASE 1000
#define GDSE_ARR_ERR_MALLOC_FAIL 1001
#define GDSE_ARR_ERR_NO_COMPARE_FUNC 1002

// ------------------------------------------------------------------------------------------------------------------------------------------

/* Initializes GDSEArray by first initializing the underlying array and then the GDSEArray fields. 
 * Return value: 
 * on success: GDS_SUCCESS,
 * on failure: one of the generic error codes indicating an invalid argument or GDSE_ARR_ERR_MALLOC_FAIL.
 * Function may fail if given an invalid argument for the initialization of the underlying array. Check gds_array.h for
 * valid init arguments. */
gds_err gdse_array_init(GDSEArray* array, size_t element_size,
        size_t capacity, double resize_factor, 
        bool (*compare_func)(void*, void*),
        void (*on_removal_callback_func)(void*));

// ---------------------------------------------------------------------------------------------------------------------


/* Dynamically allocates enough memory for one GDSEArray. 
 * Initializes GDSEArray by first initializing the underlying array and then the GDSEArray fields. 
 * Return value: 
 * on success: address of newly intialized GDSEArray,
 * on failure: NULL.
 * Function may fail if given an invalid argument for the initialization of the underlying array. Check gds_array.h for
 * valid init arguments. */
GDSEArray* gdse_array_create(size_t element_size, size_t capacity,
        double resize_factor,
        bool (*compare_func)(void*, void*),
        void (*on_removal_callback_func)(void*));

// ---------------------------------------------------------------------------------------------------------------------

/* Swaps the data of elements at pos1 and pos2. If 'pos1' == 'pos2', the function performs no action.
 * on success - GDS_SUCCESS,
 * on failure - one of the generic error codes representing an invalid argument.
 * Function may fail if 'array' is NULL or 'pos1' or 'pos2' are out of bounds('pos' >= array's count). */
gds_err gdse_array_swap(GDSEArray* array, size_t pos1, size_t pos2);

// ---------------------------------------------------------------------------------------------------------------------

/* Sets array's _compare_func field. This function assumes a non-NULL 'array' argument */
void gdse_array_set_compare_func(GDSEArray* array, bool (*compare_func)(void*, void*));

// ---------------------------------------------------------------------------------------------------------------------

/* Sets array's _on_removal_callback_func field. This function assumes a non-NULL 'array' argument */
void gdse_array_set_on_removal_callback_func(GDSEArray* array, void (*on_removal_callback_func)(void*));

// ---------------------------------------------------------------------------------------------------------------------

/* Performs sizeof(GDSEArray) and returns the value. */
size_t gdse_array_get_struct_size();

// ------------------------------------------------------------------------------------------------------------------------------------------

#endif // _GDSE_ARRAY_H_
