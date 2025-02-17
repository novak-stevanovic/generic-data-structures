#include "gds.h"
#include "gdse_vector.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "gds_misc.h"
#include "gds_vector.h"

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
#define __GDSE_VECTOR_DEF_ALLOW__
#include "def/gdse_vector_def.h"
#endif // __GDSE_VECTOR_DEF_ALLOW__

// ------------------------------------------------------------------------------------------------------------------------------------------

gds_err gdse_vector_init(GDSEVector* vector, size_t element_size,
        size_t initial_capacity, double resize_factor, 
        bool (*compare_func)(void*, void*),
        void (*on_removal_callback_func)(void*))
{
    gds_err init_status = gds_vector_init(&vector->_vector, element_size, initial_capacity, resize_factor);

    if(init_status == GDS_VEC_ERR_MALLOC_FAIL) return GDSE_VEC_ERR_MALLOC_FAIL;
    else if(init_status != GDS_SUCCESS) return GDS_GEN_ERR_INTERNAL_ERR;

    vector->_compare_func = compare_func;
    vector->_on_removal_callback_func = on_removal_callback_func;

#ifdef GDS_TEMP_BUFF_USE_SWAP_BUFF

    vector->__GDS_SWAP_BUFF_NAME = malloc(element_size);
    if(vector->__GDS_SWAP_BUFF_NAME == NULL)
    {
        gds_vector_destruct((GDSVector*)vector);
        return GDS_VEC_ERR_MALLOC_FAIL;
    }

#endif // GDS_TEMP_BUFF_USE_SWAP_BUFF
    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

GDSEVector* gdse_vector_create(size_t element_size, size_t initial_capacity,
        double resize_factor,
        bool (*compare_func)(void*, void*),
        void (*on_removal_callback_func)(void*))
{
    GDSEVector* vector = (GDSEVector*)malloc(sizeof(GDSEVector));

    if(vector == NULL) return NULL;

    if(gdse_vector_init(vector, element_size, initial_capacity, resize_factor, compare_func, on_removal_callback_func) == GDS_SUCCESS)
        return vector;
    else
    {
        free(vector);
        return NULL;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void gdse_vector_destruct(GDSEVector* vector)
{
    gds_vector_destruct((GDSVector*)vector);
    vector->_on_removal_callback_func = NULL;
    vector->_compare_func = NULL;

#ifdef GDS_TEMP_BUFF_USE_SWAP_BUFF

    free(vector->__GDS_SWAP_BUFF_NAME);
    vector->__GDS_SWAP_BUFF_NAME = NULL;

#endif // GDS_TEMP_BUFF_USE_SWAP_BUFF
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gdse_vector_swap(GDSEVector* vector, size_t pos1, size_t pos2)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);

    GDSVector* _vector = (GDSVector*)vector;

    if(pos1 >= gds_vector_get_count(_vector)) return GDS_GEN_ERR_INVALID_ARG(2);
    if(pos2 >= gds_vector_get_count(_vector)) return GDS_GEN_ERR_INVALID_ARG(3);

    if(pos1 == pos2) return GDS_SUCCESS;

    void* pos1_data = gds_vector_at(_vector, pos1);
    void* pos2_data = gds_vector_at(_vector, pos2);

    size_t data_size = gds_vector_get_element_size(_vector);

    __GDS_GET_SWAP_BUFF(vector,data_size);

    gds_misc_swap(pos1_data, pos2_data, swap_buff, data_size);

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

void gdse_vector_set_compare_func(GDSEVector* vector, bool (*compare_func)(void*, void*))
{
    if(vector == NULL) return;

    vector->_compare_func = compare_func;
}

// ---------------------------------------------------------------------------------------------------------------------

void gdse_vector_set_on_removal_callback_func(GDSEVector* vector, void (*on_removal_callback_func)(void*))
{
    if(vector == NULL) return;

    vector->_on_removal_callback_func = on_removal_callback_func;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gdse_vector_get_struct_size()
{
    return sizeof(GDSEVector);
}

// ------------------------------------------------------------------------------------------------------------------------------------------
