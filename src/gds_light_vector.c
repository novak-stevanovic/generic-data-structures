#include "gds.h"
#include "gds_light_array.h"
#include "gds_light_vector.h"

#include <assert.h>
#include <stdio.h>

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
#define __GDS_LIGHT_VECTOR_DEF_ALLOW__
#include "def/gds_light_vector_def.h"
#endif // __GDS_LIGHT_VECTOR_DEF_ALLOW__

// ------------------------------------------------------------------------------------------------------------------------------------------

static gds_err _gds_light_vector_expand_if_needed(GDSLightVector* vector);

static gds_err _gds_light_vector_shrink_if_needed(GDSLightVector* vector);

// ------------------------------------------------------------------------------------------------------------------------------------------

gds_err gds_light_vector_init(GDSLightVector* vector, size_t element_size, size_t initial_capacity, double resize_factor)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(element_size == 0) return GDS_GEN_ERR_INVALID_ARG(2);
    if(initial_capacity == 0) return GDS_GEN_ERR_INVALID_ARG(3);
    if(resize_factor <= 1) return GDS_GEN_ERR_INVALID_ARG(4);

    vector->_resize_factor = resize_factor;
    gds_err init_status = gds_light_array_init(&vector->_data, initial_capacity, element_size);

    if(init_status != GDS_SUCCESS) return init_status;
    else return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

GDSLightVector* gds_light_vector_create(size_t element_size, size_t initial_capacity, double resize_factor)
{
    if(element_size == 0) return NULL;
    if(initial_capacity == 0) return NULL;
    if(resize_factor <= 1) return NULL;

    GDSLightVector* vector = (GDSLightVector*)malloc(sizeof(GDSLightVector));
    if(vector == NULL) return NULL;

    gds_err init_status = gds_light_vector_init(vector, element_size, initial_capacity, resize_factor);
    if(init_status != GDS_SUCCESS) 
    {
        free(vector);
        return NULL;
    }
    else return vector;
}

// ---------------------------------------------------------------------------------------------------------------------

void gds_light_vector_destruct(GDSLightVector* vector)
{
    if(vector == NULL) return;

    gds_light_array_destruct(&vector->_data);
    vector->_resize_factor = 0;
}

// ---------------------------------------------------------------------------------------------------------------------

void* gds_light_vector_at(const GDSLightVector* vector, size_t pos)
{
    return gds_light_array_at(&vector->_data, pos);
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_vector_assign(GDSLightVector* vector, const void* data, size_t pos)
{
    return gds_light_array_assign(&vector->_data, data, pos);
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_vector_swap(GDSLightVector* vector, size_t pos1, size_t pos2)
{
    return gds_light_array_swap(&vector->_data, pos1, pos2);
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_vector_push_back(GDSLightVector* vector, const void* data)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(data == NULL) return GDS_GEN_ERR_INVALID_ARG(2);

    gds_err resize_status = _gds_light_vector_expand_if_needed(vector);
    if(resize_status != GDS_SUCCESS) return GDS_VEC_ERR_REALLOC_FAIL;

    gds_err push_back_status = gds_light_array_push_back(&vector->_data, data);

    assert(push_back_status == GDS_SUCCESS);
    
    return GDS_SUCCESS;
    
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_vector_insert_at(GDSLightVector* vector, const void* data, size_t pos)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(data == NULL) return GDS_GEN_ERR_INVALID_ARG(2);
    if(pos > gds_light_vector_get_count(vector)) return GDS_GEN_ERR_INVALID_ARG(3);

    gds_err resize_status = _gds_light_vector_expand_if_needed(vector);
    if(resize_status != GDS_SUCCESS) return GDS_VEC_ERR_REALLOC_FAIL;

    gds_err insert_status = gds_light_array_insert_at(&vector->_data, data, pos);

    assert(insert_status == GDS_SUCCESS);
    
    return GDS_SUCCESS;
    
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_vector_pop_back(GDSLightVector* vector)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(gds_light_vector_is_empty(vector)) return GDS_VEC_ERR_VEC_EMPTY;

    gds_err resize_status = _gds_light_vector_shrink_if_needed(vector);
    if(resize_status != GDS_SUCCESS) return GDS_VEC_ERR_REALLOC_FAIL;

    gds_err pop_back_status = gds_light_array_pop_back(&vector->_data);

    assert(pop_back_status == GDS_SUCCESS);
    
    return GDS_SUCCESS;
    
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_vector_remove_at(GDSLightVector* vector, size_t pos)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(pos >= gds_light_vector_get_count(vector)) return GDS_GEN_ERR_INVALID_ARG(2);

    gds_err resize_status = _gds_light_vector_shrink_if_needed(vector);
    if(resize_status != GDS_SUCCESS) return GDS_VEC_ERR_REALLOC_FAIL;

    gds_err remove_status = gds_light_array_remove_at(&vector->_data, pos);

    assert(remove_status == GDS_SUCCESS);
    
    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_vector_empty(GDSLightVector* vector)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);

    assert(gds_light_array_empty(&vector->_data) == 0);

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_vector_prealloc(GDSLightVector* vector, size_t new_chunk_size)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(new_chunk_size == 0) return GDS_SUCCESS;

    gds_err realloc_status = gds_light_array_realloc(&vector->_data, vector->_data._capacity + new_chunk_size);
    if(realloc_status != GDS_SUCCESS) return GDS_VEC_ERR_REALLOC_FAIL;
    else return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_vector_fit(GDSLightVector* vector)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);

    gds_err realloc_status = gds_light_array_realloc(&vector->_data, vector->_data._count);
    if(realloc_status != GDS_SUCCESS) return GDS_VEC_ERR_REALLOC_FAIL;
    else return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_light_vector_get_count(const GDSLightVector* vector)
{
    return (vector != NULL) ? gds_light_array_get_count(&vector->_data) : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

/* Gets current capacity of vector. Assumes non-NULL argument. */
size_t gds_light_vector_get_capacity(const GDSLightVector* vector)
{
    return (vector != NULL) ? gds_light_array_get_capacity(&vector->_data) : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

bool gds_light_vector_is_empty(const GDSLightVector* vector)
{
    return (vector != NULL) ? gds_light_array_is_empty(&vector->_data) : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_light_vector_get_element_size(const GDSLightVector* vector)
{
    return (vector != NULL) ? gds_light_array_get_element_size(&vector->_data) : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_light_vector_get_struct_size()
{
    return sizeof(GDSLightVector);
}

// ------------------------------------------------------------------------------------------------------------------------------------------

static gds_err _gds_light_vector_expand_if_needed(GDSLightVector* vector)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);

    size_t vector_count = vector->_data._count;
    size_t vector_capacity = vector->_data._capacity;

    if(vector_count == vector_capacity)
    {
        gds_err realloc_status = gds_light_array_realloc(&vector->_data, ((size_t)vector_count * vector->_resize_factor));
        if(realloc_status != GDS_SUCCESS) return GDS_VEC_ERR_REALLOC_FAIL;
    }

    return GDS_SUCCESS;
}

static gds_err _gds_light_vector_shrink_if_needed(GDSLightVector* vector)
{

    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);

    size_t vector_count = vector->_data._count;
    size_t vector_capacity = vector->_data._capacity;

    if(vector_count  <= ((size_t)(vector_capacity / vector->_resize_factor)))
    {
        gds_err realloc_status = gds_light_array_realloc(&vector->_data, ((size_t)(vector_capacity / vector->_resize_factor)));
        if(realloc_status != GDS_SUCCESS) return GDS_VEC_ERR_REALLOC_FAIL;
    }

    return GDS_SUCCESS;
}
