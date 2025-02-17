#include "gds.h"
#include "gds_array.h"
#include "gds_misc.h"
#include "gdse_array.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
#define __GDSE_ARRAY_DEF_ALLOW__
#include "def/gdse_array_def.h"
#endif // __GDSE_ARRAY_DEF_ALLOW__

// ------------------------------------------------------------------------------------------------------------------------------------------

gds_err gdse_array_init(GDSEArray* array, size_t element_size,
        size_t capacity, double resize_factor, 
        bool (*compare_func)(void*, void*),
        void (*on_removal_callback_func)(void*))
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(element_size == 0) return GDS_GEN_ERR_INVALID_ARG(2);
    if(capacity == 0) return GDS_GEN_ERR_INVALID_ARG(3);

    gds_err init_status = gds_array_init((GDSArray*)array, capacity, element_size);
    if(init_status == GDS_ARR_ERR_MALLOC_FAIL) return GDSE_ARR_ERR_MALLOC_FAIL;
    else if(init_status != GDS_SUCCESS) return GDS_GEN_ERR_INTERNAL_ERR;

    array->_compare_func = compare_func;
    array->_on_removal_callback_func = on_removal_callback_func;

#ifdef GDS_TEMP_BUFF_USE_SWAP_BUFF

    array->__GDS_SWAP_BUFF_NAME = malloc(element_size);
    if(array->__GDS_SWAP_BUFF_NAME == NULL)
    {
        gds_array_destruct((GDSArray*)array);
        return GDS_ARR_ERR_MALLOC_FAIL;
    }

#endif // GDS_TEMP_BUFF_USE_SWAP_BUFF
    
    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

GDSEArray* gdse_array_create(size_t element_size, size_t capacity,
        double resize_factor,
        bool (*compare_func)(void*, void*),
        void (*on_removal_callback_func)(void*))
{
    if(element_size == 0) return NULL;
    if(capacity == 0) return NULL;

    GDSEArray* array = (GDSEArray*)malloc(sizeof(GDSEArray));

    gds_err init_status = gdse_array_init(array, element_size, capacity, resize_factor, compare_func, on_removal_callback_func);
    if(init_status != GDS_SUCCESS) 
    {
        free(array);
        return NULL;
    }

    array->_compare_func = compare_func;
    array->_on_removal_callback_func = on_removal_callback_func;

    return array;
}

// ---------------------------------------------------------------------------------------------------------------------

void gdse_array_destruct(GDSEArray* array)
{
    gds_array_destruct((GDSArray*)array);
    array->_on_removal_callback_func = NULL;
    array->_compare_func = NULL;

#ifdef GDS_TEMP_BUFF_USE_SWAP_BUFF

    free(array->__GDS_SWAP_BUFF_NAME);
    array->__GDS_SWAP_BUFF_NAME = NULL;

#endif // GDS_TEMP_BUFF_USE_SWAP_BUFF
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gdse_array_swap(GDSEArray* array, size_t pos1, size_t pos2)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);

    GDSArray* _array = (GDSArray*)array;

    if(pos1 >= _array->_count) return GDS_GEN_ERR_INVALID_ARG(2);
    if(pos2 >= _array->_count) return GDS_GEN_ERR_INVALID_ARG(3);

    if(pos1 == pos2) return GDS_SUCCESS;

    void* pos1_data = gds_array_at(_array, pos1);
    void* pos2_data = gds_array_at(_array, pos2);

    size_t data_size = _array->_element_size;

    __GDS_GET_SWAP_BUFF(array,data_size);

    gds_misc_swap(pos1_data, pos2_data, swap_buff, data_size);

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

void gdse_array_set_compare_func(GDSEArray* array, bool (*compare_func)(void*, void*))
{
    if(array == NULL) return;

    array->_compare_func = compare_func;
}

// ---------------------------------------------------------------------------------------------------------------------

void gdse_array_set_on_removal_callback_func(GDSEArray* array, void (*on_removal_callback_func)(void*))
{
    if(array == NULL) return;

    array->_on_removal_callback_func = on_removal_callback_func;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gdse_array_get_struct_size()
{
    return sizeof(GDSEArray);
}

// ------------------------------------------------------------------------------------------------------------------------------------------
