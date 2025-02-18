#include "gds.h"
#include "gds_array.h"
#include "gds_misc.h"
#include "gds_vector.h"

#include <assert.h>
#include <stdio.h>

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
#define __GDS_VECTOR_DEF_ALLOW__
#include "def/gds_vector_def.h"
#endif // __GDS_VECTOR_DEF_ALLOW__

// ------------------------------------------------------------------------------------------------------------------------------------------

static gds_err _gds_vector_update_capacity(GDSVector* vector);

// ------------------------------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_init(GDSVector* vector, size_t element_size, size_t initial_capacity, double resize_factor)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(element_size == 0) return GDS_GEN_ERR_INVALID_ARG(2);
    if(initial_capacity == 0) return GDS_GEN_ERR_INVALID_ARG(3);
    if(resize_factor <= GDS_VEC_MIN_RESIZE_FACTOR) return GDS_GEN_ERR_INVALID_ARG(4);

    vector->_resize_factor = resize_factor;

    gds_err init_status = gds_array_init(&vector->_data, initial_capacity, element_size);

    if(init_status == GDS_SUCCESS) return GDS_SUCCESS;
    else if(init_status == GDS_ARR_ERR_MALLOC_FAIL) return GDS_ARR_ERR_MALLOC_FAIL;
    else return GDS_GEN_ERR_INTERNAL_ERR;
}

// ---------------------------------------------------------------------------------------------------------------------

GDSVector* gds_vector_create(size_t element_size, size_t initial_capacity, double resize_factor)
{
    GDSVector* vector = (GDSVector*)malloc(sizeof(GDSVector));
    if(vector == NULL) return NULL;

    gds_err init_status = gds_vector_init(vector, element_size, initial_capacity, resize_factor);
    if(init_status != GDS_SUCCESS) 
    {
        free(vector);
        return NULL;
    }
    else return vector;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_init_default(GDSVector* vector, size_t element_size)
{
    return gds_vector_init(vector, element_size, GDS_VEC_DEFAULT_INITIAL_CAPACITY, GDS_VEC_DEFAULT_RESIZE_FACTOR);
}

// ---------------------------------------------------------------------------------------------------------------------

GDSVector* gds_vector_create_default(size_t element_size)
{
    return gds_vector_create(element_size, GDS_VEC_DEFAULT_INITIAL_CAPACITY, GDS_VEC_DEFAULT_RESIZE_FACTOR);
}

// ---------------------------------------------------------------------------------------------------------------------

void gds_vector_destruct(GDSVector* vector)
{
    if(vector == NULL) return;

    gds_array_destruct(&vector->_data);
    vector->_resize_factor = 0;
}

// ---------------------------------------------------------------------------------------------------------------------

void* gds_vector_at(const GDSVector* vector, size_t pos)
{
    return gds_array_at(&vector->_data, pos);
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_assign(GDSVector* vector, const void* data, size_t pos)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(data == NULL) return GDS_GEN_ERR_INVALID_ARG(2);
    if(pos >= gds_vector_get_count(vector)) return GDS_GEN_ERR_INVALID_ARG(3);

    gds_err assign_status = gds_array_assign(&vector->_data, data, pos);

    if(assign_status == GDS_SUCCESS) return GDS_SUCCESS;
    else return GDS_GEN_ERR_INTERNAL_ERR;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_push_back(GDSVector* vector, const void* data)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(data == NULL) return GDS_GEN_ERR_INVALID_ARG(2);

    gds_err resize_status = _gds_vector_update_capacity(vector);

    if(resize_status == GDS_VEC_ERR_REALLOC_FAIL) return GDS_VEC_ERR_REALLOC_FAIL;
    else if(resize_status != GDS_SUCCESS) return GDS_GEN_ERR_INTERNAL_ERR;

    gds_err push_back_status = gds_array_push_back(&vector->_data, data);
    if(push_back_status != GDS_SUCCESS) return GDS_GEN_ERR_INTERNAL_ERR;

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_swap(GDSVector* vector, size_t pos1, size_t pos2, void* swap_buff)
{
    return gds_array_swap(&vector->_data, pos1, pos2, swap_buff);
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_insert_at(GDSVector* vector, const void* data, size_t pos)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(data == NULL) return GDS_GEN_ERR_INVALID_ARG(2);
    if(pos > gds_vector_get_count(vector)) return GDS_GEN_ERR_INVALID_ARG(3);

    gds_err resize_status = _gds_vector_update_capacity(vector);
    if(resize_status != GDS_SUCCESS) return GDS_VEC_ERR_REALLOC_FAIL;

    gds_err insert_status = gds_array_insert_at(&vector->_data, data, pos);
    if(insert_status != GDS_SUCCESS) return GDS_GEN_ERR_INTERNAL_ERR;

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_pop_back(GDSVector* vector)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(gds_vector_is_empty(vector)) return GDS_VEC_ERR_VEC_EMPTY;

    gds_err resize_status = _gds_vector_update_capacity(vector);
    if(resize_status != GDS_SUCCESS) return GDS_VEC_ERR_REALLOC_FAIL;

    gds_array_pop_back(&vector->_data);

    return GDS_SUCCESS;
    
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_remove_at(GDSVector* vector, size_t pos)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(pos >= gds_vector_get_count(vector)) return GDS_GEN_ERR_INVALID_ARG(2);

    gds_err remove_status = gds_array_remove_at(&vector->_data, pos);
    if(remove_status != GDS_SUCCESS) return GDS_GEN_ERR_INTERNAL_ERR;

    gds_err resize_status = _gds_vector_update_capacity(vector);
    if(resize_status != GDS_SUCCESS) return GDS_VEC_ERR_REALLOC_FAIL;

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_empty(GDSVector* vector)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);

    gds_err empty_status = gds_array_empty(&vector->_data);
    if(empty_status != GDS_SUCCESS) return GDS_GEN_ERR_INTERNAL_ERR;

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_reserve(GDSVector* vector, size_t new_capacity)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);

    size_t vector_capacity = gds_vector_get_capacity(vector);

    if(new_capacity < vector_capacity) return GDS_GEN_ERR_INVALID_ARG(2);
    else if(new_capacity == vector_capacity) return GDS_SUCCESS;

    gds_err realloc_status = gds_array_realloc(&vector->_data, new_capacity);
    if(realloc_status != GDS_SUCCESS) return GDS_VEC_ERR_REALLOC_FAIL;

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_fit(GDSVector* vector)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);

    gds_err realloc_status;
    if(gds_vector_get_count(vector) == 0)
        realloc_status = gds_array_realloc(&vector->_data, gds_array_get_element_size(&vector->_data));
    else
        realloc_status = gds_array_realloc(&vector->_data, vector->_data._count);
    
    if(realloc_status != GDS_SUCCESS) return GDS_VEC_ERR_REALLOC_FAIL;

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_set_resize_factor(GDSVector* vector, double new_resize_factor)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(new_resize_factor <= GDS_VEC_MIN_RESIZE_FACTOR) return GDS_GEN_ERR_INVALID_ARG(2);

    vector->_resize_factor = new_resize_factor;

    return GDS_SUCCESS;
}
// ---------------------------------------------------------------------------------------------------------------------

ssize_t gds_vector_find(GDSVector* vector, const void* data, bool (*compare_func)(const void*, const void*))
{
    return gds_array_find(&vector->_data, data, compare_func);
}

// ---------------------------------------------------------------------------------------------------------------------

double gds_vector_get_resize_factor(const GDSVector* vector)
{
    return (vector != NULL) ? vector->_resize_factor : -1;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_vector_get_count(const GDSVector* vector)
{
    return (vector != NULL) ? gds_array_get_count(&vector->_data) : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_vector_get_capacity(const GDSVector* vector)
{
    return (vector != NULL) ? gds_array_get_capacity(&vector->_data) : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

bool gds_vector_is_empty(const GDSVector* vector)
{
    return (vector != NULL) ? gds_array_is_empty(&vector->_data) : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_vector_get_element_size(const GDSVector* vector)
{
    return (vector != NULL) ? gds_array_get_element_size(&vector->_data) : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_vector_get_struct_size()
{
    return sizeof(GDSVector);
}

// ------------------------------------------------------------------------------------------------------------------------------------------

static gds_err _gds_vector_update_capacity(GDSVector* vector)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);

    size_t vector_count = vector->_data._count;
    size_t vector_capacity = vector->_data._capacity;
    double resize_factor = vector->_resize_factor;

    if(vector_count == vector_capacity)
    {
        gds_err realloc_status = gds_array_realloc(&vector->_data, ((size_t)vector_count * resize_factor));
        if(realloc_status != GDS_SUCCESS) return GDS_VEC_ERR_REALLOC_FAIL;
    }

    return GDS_SUCCESS;

}
