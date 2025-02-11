#include "gds.h"
#include "gds_vector.h"

#include <assert.h>
#include <stdio.h>
#include "gds_array.h"

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
#define __GDS_VECTOR_DEF_ALLOW__
#include "def/gds_vector_def.h"
#endif

// ------------------------------------------------------------------------------------------------------------------------------------------

/* Function retrieves the minimum capacity of the vector based on the value it was passed to it during the vector's initialization.
 * The function expects a non-NULL argument */
size_t _gds_vector_get_min_size(const GDSVector* vector);

/* Function calculates if the vector should shrink. This is decided by the used up space of the last allocated chunk for vector's data.
 * When that chunk's empty, the vector should shrink. One exception to this rule is when the count of the vector reaches 0. In that case,
 * the chunk responsible for storing the first <minimum capacity> elements remains even if it is fully obsolete. 
 * This function assumes a non-NULL argument. */
static bool _gds_vector_should_vector_shrink(const GDSVector* vector);


/* This function performs a realloc so that it may store vector's capacity + size_diff elements. 
 * However, this function must be called in a very controlled manner. 
 * It may be used for both expanding and shrinking the vector.
 *      - If its used for expanding: This function will attempt to realloc() the vector's data so that it can fit
 *      the new capacity. If it succeeds, a new chunk will be appended to the vector's chunk list so that it 
 *      can know when to free memory.
 *
 *      - If it's used for shrinking: This function must be called so that the value of 
 *      abs(size_diff) < max_shrink_value. max_shrink_value is a value that represents how much the vector 
 *      may be shrank so that it takes into account its minimum capacity. It is calculated as 
 *      (vector's capacity - _gds_vector_get_min_size(). After computing this, the function will perform
 *      a realloc() call which may theoretically fail. If it succeeds, a call to 
 *      _gds_vector_chunk_list_shrink_by() will be made to fix the tracking of the vector's chunks. 
 * Return value:
 * on success: GDS_SUCCESS,
 * on failure: one of the generic codes representing invalid arguments or GDS_VEC_ERR_REALLOC_FAIL. 
 * Invalid arguments error codes are returned when: 'vector' is NULL, 'size_diff' == 0, or when the vector
 * must shrink but (abs(size_diff) > max_shrink_value). */
static gds_err _gds_vector_allocate(GDSVector* vector, ssize_t size_diff);

// ------------------------------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_init(GDSVector* vector,
        size_t element_size,
        void (*on_element_removal_func)(void*),
        size_t min_capacity,
        size_t (*get_next_chunk_size_func)(struct GDSVector* vector, size_t last_chunk_size))
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(element_size == 0) return GDS_GEN_ERR_INVALID_ARG(2);
    if(min_capacity == 0) return GDS_GEN_ERR_INVALID_ARG(4);
    if(get_next_chunk_size_func == NULL) return GDS_GEN_ERR_INVALID_ARG(5);

    gds_err data_init_status = gds_array_init(&vector->_data, min_capacity, element_size, on_element_removal_func);
    if(data_init_status != GDS_SUCCESS) return GDS_FAILURE;

    gds_err chunks_init_status = _gds_vector_chunk_list_init(&vector->_chunks, min_capacity);
    if(chunks_init_status != GDS_SUCCESS)
    {
        gds_array_destruct(&vector->_data);
        return GDS_FAILURE;
    }

    vector->_get_next_chunk_size_func = get_next_chunk_size_func;

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

GDSVector* gds_vector_create(size_t element_size,
        void (*on_element_removal_func)(void*),
        size_t min_capacity,
        size_t (*get_next_chunk_size_func)(struct GDSVector* vector, size_t last_chunk_size))
{
    if(element_size == 0) return NULL;
    if(min_capacity == 0) return NULL;
    if(get_next_chunk_size_func == NULL) return NULL;

    GDSVector* vector = (GDSVector*)malloc(sizeof(GDSVector));

    gds_err init_status = gds_vector_init(vector, element_size, on_element_removal_func, min_capacity, get_next_chunk_size_func);

    if(init_status != GDS_SUCCESS) return NULL;

    return vector;
}

// ---------------------------------------------------------------------------------------------------------------------

void gds_vector_destruct(GDSVector* vector)
{
    if(vector == NULL) return;

    _gds_vector_chunk_list_destruct(&vector->_chunks);
    gds_array_destruct(&vector->_data);

    vector->_get_next_chunk_size_func = NULL;
}

// ---------------------------------------------------------------------------------------------------------------------

void* gds_vector_at(const GDSVector* vector, size_t pos)
{
    if(vector == NULL) return NULL;

    const GDSArray* vector_data = &vector->_data;

    if(pos >= gds_array_get_count(vector_data)) return NULL;

    return gds_array_at(vector_data, pos);
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_assign(const GDSVector* vector, const void* data, size_t pos)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(data == NULL) return GDS_GEN_ERR_INVALID_ARG(2);

    const GDSArray* vector_data = &vector->_data;

    if(pos >= gds_array_get_count(vector_data)) return GDS_GEN_ERR_INVALID_ARG(3);

    gds_array_assign(vector_data, data, pos);

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_swap(const GDSVector* vector, size_t pos1, size_t pos2)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);

    const GDSArray* vector_data = &vector->_data;
    size_t vector_data_count = gds_array_get_count(vector_data);

    if(pos1 >= vector_data_count) return GDS_GEN_ERR_INVALID_ARG(2);
    if(pos2 >= vector_data_count) return GDS_GEN_ERR_INVALID_ARG(3);

    gds_array_swap(vector_data, pos1, pos2);

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_push_back(GDSVector* vector, const void* data)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(data == NULL) return GDS_GEN_ERR_INVALID_ARG(2);

    return gds_vector_insert_at(vector, data, vector->_data._count);
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_insert_at(GDSVector* vector, const void* data, size_t pos)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(data == NULL) return GDS_GEN_ERR_INVALID_ARG(2);

    GDSArray* vector_data = &vector->_data;

    if(pos > vector_data->_count) return GDS_GEN_ERR_INVALID_ARG(3);

    if(vector_data->_capacity == vector_data->_count)
    {
        size_t new_chunk_size = vector->_get_next_chunk_size_func(vector, _gds_vector_chunk_list_get_last_chunk_size(&vector->_chunks));
        if(new_chunk_size == 0) return GDS_VEC_ERR_CHUNK_GEN_FUNC_FAIL;

        gds_err prealloc_status = gds_vector_prealloc(vector, new_chunk_size);
        if(prealloc_status != GDS_SUCCESS) return GDS_VEC_ERR_REALLOC_FAIL;
    }

    gds_array_insert_at(vector_data, data, pos);

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_pop_back(GDSVector* vector)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(gds_vector_is_empty(vector)) return GDS_VEC_ERR_VEC_EMPTY;

    return gds_vector_remove_at(vector, vector->_data._count - 1);
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_remove_at(GDSVector* vector, size_t pos)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);

    GDSArray* vector_data = &vector->_data;

    if(pos >= vector_data->_count) return GDS_GEN_ERR_INVALID_ARG(2);

    gds_array_remove_at(vector_data, pos);

    if(_gds_vector_should_vector_shrink(vector))
    {
        size_t shrinkage_amount = _gds_vector_chunk_list_get_last_chunk_size(&vector->_chunks);
        ssize_t expansion_amount = -((ssize_t)shrinkage_amount);

        gds_err alloc_status = _gds_vector_allocate(vector, expansion_amount);

        if(alloc_status == GDS_VEC_ERR_REALLOC_FAIL) return GDS_VEC_ERR_REALLOC_FAIL;
        else if(alloc_status == GDS_GEN_ERR_INVALID_ARG(3)) assert(1 != 1);
    }

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_empty(GDSVector* vector)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);

    while(!(gds_vector_is_empty(vector))) gds_vector_pop_back(vector);

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_prealloc(GDSVector* vector, size_t new_chunk_size)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(new_chunk_size == 0) return GDS_GEN_ERR_INVALID_ARG(2);

    gds_err alloc_status = _gds_vector_allocate(vector, new_chunk_size);

    if(alloc_status == GDS_VEC_ERR_REALLOC_FAIL) return GDS_VEC_ERR_REALLOC_FAIL;
    else return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_fit(GDSVector* vector)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);

    GDSArray* vector_data = &vector->_data;

    size_t min_size = _gds_vector_get_min_size(vector);
    size_t required_size = (vector_data->_count > min_size) ? vector_data->_count : min_size;
    size_t shrink_amount = (vector_data->_capacity - required_size);

    ssize_t expansion_amount = -((ssize_t)shrink_amount);

    gds_err alloc_status = _gds_vector_allocate(vector, expansion_amount);

    if(alloc_status == GDS_VEC_ERR_REALLOC_FAIL) return GDS_VEC_ERR_REALLOC_FAIL;
    else if(alloc_status == GDS_GEN_ERR_INVALID_ARG(3)) assert(1 != 1);

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_vector_get_count(const GDSVector* vector)
{
    return (vector != NULL) ? vector->_data._count : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

bool gds_vector_is_empty(const GDSVector* vector)
{
    return (vector != NULL) ? gds_array_is_empty(&vector->_data) : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_vector_get_element_size(const GDSVector* vector)
{
    return (vector != NULL) ? vector->_data._element_size : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_vector_get_capacity(const GDSVector* vector)
{
    return (vector != NULL) ? vector->_data._capacity : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_vector_get_struct_size()
{
    return sizeof(GDSVector);
}

// ------------------------------------------------------------------------------------------------------------------------------------------

size_t _gds_vector_get_min_size(const GDSVector* vector)
{
    // assert(vector != NULL);

    return _gds_vector_chunk_list_get_min_size(&vector->_chunks);
}

static bool _gds_vector_should_vector_shrink(const GDSVector* vector)
{
    if(vector == NULL) return false;

    const GDSArray* vector_data = &vector->_data;

    return (vector_data->_count <= (vector_data->_capacity - _gds_vector_chunk_list_get_min_size(&vector->_chunks)));
}

static gds_err _gds_vector_allocate(GDSVector* vector, ssize_t size_diff)
{
    if(vector == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(size_diff == 0) return GDS_GEN_ERR_INVALID_ARG(2);
    
    GDSArray* vector_data = &vector->_data;
    size_t current_capacity = vector_data->_capacity;

    if(size_diff > 0)
    {
        gds_err realloc_status = gds_array_realloc(vector_data, current_capacity + size_diff);

        if(realloc_status != GDS_SUCCESS) return GDS_VEC_ERR_REALLOC_FAIL;
        else _gds_vector_chunk_list_add_new_chunk(&vector->_chunks, (size_t)size_diff);
    }
    else
    {
        _GDSVectorChunkList* chunk_list = &vector->_chunks;

        size_t max_shrink = current_capacity - _gds_vector_chunk_list_get_min_size(chunk_list); // min_capacity
        if((-size_diff) > max_shrink) return GDS_GEN_ERR_INVALID_ARG(3); 

        gds_err realloc_status = gds_array_realloc(vector_data, current_capacity + size_diff);
        if(realloc_status != GDS_SUCCESS) return GDS_VEC_ERR_REALLOC_FAIL;

        _gds_vector_chunk_list_shrink_by(chunk_list, -size_diff);
    }

    return GDS_SUCCESS;

}
