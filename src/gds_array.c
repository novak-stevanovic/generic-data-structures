#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "gds.h"
#include "gds_light_array.h"
#include "gds_misc.h"
#include "gds_array.h"

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
#define __GDS_ARRAY_DEF_ALLOW__
#include "def/gds_array_def.h"
#endif // GDS_ENABLE_OPAQUE_STRUCTS

// ------------------------------------------------------------------------------------------------------------------------------------------

static void _gds_array_on_removal_func(GDSArray* array, void* data)
{
    if((array) && (array->_on_element_removal_func) && (data)) array->_on_element_removal_func(data);
}

// ------------------------------------------------------------------------------------------------------------------------------------------

gds_err gds_array_init(GDSArray* array, size_t capacity, size_t element_size, void (*on_element_removal_func)(void*),
        bool (*compare_func)(void*, void*))
{
    gds_err init_status = gds_light_array_init(&array->_base, capacity, element_size);

    if(init_status != GDS_SUCCESS) return init_status;

    array->_on_element_removal_func = on_element_removal_func;
    array->_compare_func = compare_func;

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

GDSArray* gds_array_create(size_t capacity, size_t element_size, void (*on_element_removal_func)(void*),
        bool (*compare_func)(void*, void*))
{
    GDSArray* array = (GDSArray*)malloc(sizeof(GDSArray));
    
    if(array == NULL) return NULL;

    if(gds_array_init(array, capacity, element_size, on_element_removal_func, compare_func) != GDS_SUCCESS)
    {
        free(array);
        return NULL;
    }
    else return array;
}

// ---------------------------------------------------------------------------------------------------------------------

void gds_array_destruct(GDSArray* array)
{
    if(array == NULL) return;

    gds_array_empty(array);
    gds_light_array_destruct(&array->_base);
    array->_compare_func = NULL;
    array->_on_element_removal_func = NULL;
}

// ---------------------------------------------------------------------------------------------------------------------

void* gds_array_at(const GDSArray* array, size_t pos)
{
    return gds_light_array_at(&array->_base, pos);
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_array_assign(const GDSArray* array, const void* data, size_t pos)
{
    return gds_light_array_assign(&array->_base, data, pos);
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_array_swap(const GDSArray* array, size_t pos1, size_t pos2)
{
    return gds_light_array_swap(&array->_base, pos1, pos2);
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_array_push_back(GDSArray* array, const void* data)
{
    return gds_light_array_push_back(&array->_base, data);
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_array_insert_at(GDSArray* array, const void* data, size_t pos)
{
    return gds_light_array_insert_at(&array->_base, data, pos);
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_array_remove_at(GDSArray* array, size_t pos)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);

    GDSLightArray* base = &array->_base;

    if(pos >= base->_count) return GDS_GEN_ERR_INVALID_ARG(2);

    void* data = gds_light_array_at(base, pos);
    _gds_array_on_removal_func(array, data);

    return gds_light_array_remove_at(base, pos);
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_array_pop_back(GDSArray* array)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);

    GDSLightArray* base = &array->_base;

    if(array->_base._count == 0) return GDS_ARR_ERR_ARR_EMPTY;

    void* data = gds_light_array_at(&array->_base, (array->_base._count - 1));
    _gds_array_on_removal_func(array, data);

    return gds_light_array_pop_back(&array->_base);
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_array_empty(GDSArray* array)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);

    while(array->_base._count != 0) gds_array_pop_back(array);

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_array_realloc(GDSArray* array, size_t capacity)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(capacity == 0) return GDS_GEN_ERR_INVALID_ARG(2);

    while(array->_base._count > capacity) gds_array_pop_back(array);

    return gds_light_array_realloc(&array->_base, capacity);
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_array_get_count(const GDSArray* array)
{
    return gds_light_array_get_count(&array->_base);
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_array_get_capacity(const GDSArray* array)
{
    return gds_light_array_get_capacity(&array->_base);
}

// ---------------------------------------------------------------------------------------------------------------------

bool gds_array_is_empty(const GDSArray* array)
{
    return gds_light_array_is_empty(&array->_base);
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_array_get_element_size(const GDSArray* array)
{
    return gds_light_array_get_element_size(&array->_base);
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_array_get_struct_size();

// ------------------------------------------------------------------------------------------------------------------------------------------
