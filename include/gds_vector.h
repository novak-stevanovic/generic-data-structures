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

// ------------------------------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_init(GDSVector* vector,
        size_t element_size,
        void (*on_element_removal_func)(void*),
        size_t min_capacity,
        size_t (*_get_next_chunk_size_func)(struct GDSVector* vector, size_t last_chunk_size));

// ---------------------------------------------------------------------------------------------------------------------

GDSVector* gds_vector_create(size_t element_size,
        void (*on_element_removal_func)(void*),
        size_t min_capacity,
        size_t (*get_next_chunk_size_func)(struct GDSVector* vector, size_t last_chunk_size));

// ---------------------------------------------------------------------------------------------------------------------

void gds_vector_destruct(GDSVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

void* gds_vector_at(const GDSVector* vector, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_assign(const GDSVector* vector, const void* data, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_swap(const GDSVector* vector, size_t pos1, size_t pos2);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_push_back(GDSVector* vector, const void* data);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_insert_at(GDSVector* vector, const void* data, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_pop_back(GDSVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_remove_at(GDSVector* vector, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_empty(GDSVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_prealloc(GDSVector* vector, size_t new_chunk_size);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_vector_fit(GDSVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_vector_get_count(const GDSVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

bool gds_vector_is_empty(const GDSVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_vector_get_element_size(const GDSVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_vector_get_capacity(const GDSVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_vector_get_struct_size();

// ------------------------------------------------------------------------------------------------------------------------------------------

#endif // _GDS_VECTOR_H_
