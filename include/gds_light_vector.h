#ifndef _GDS_LIGHT_VECTOR_H_
#define _GDS_LIGHT_VECTOR_H_

#include "gds.h"

#include <stdbool.h>
#include <stddef.h>

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
struct GDSLightVector;
#else
#define __GDS_LIGHT_VECTOR_DEF_ALLOW__
#include "def/gds_light_vector.h"
#endif // GDS_ENABLE_OPAQUE_STRUCTS

typedef struct GDSLightVector GDSLightVector;

// ------------------------------------------------------------------------------------------------------------------------------------------

#define GDS_LVECTOR_ERR_BASE 4000
#define GDS_LVECTOR_ERR_MALLOC_FAIL 4001
#define GDS_LVECTOR_ERR_REALLOC_FAIL 4002
#define GDS_LVECTOR_ERR_VEC_EMPTY 4003

// ------------------------------------------------------------------------------------------------------------------------------------------

void gds_light_vector_init(GDSLightVector* vector, size_t initial_capacity);

// ---------------------------------------------------------------------------------------------------------------------

GDSLightVector* gds_light_vector_create(size_t initial_capacity, size_t element_size, double resize_factor);

// ---------------------------------------------------------------------------------------------------------------------

GDSLightVector* gds_light_vector_destruct(GDSLightVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

void* gds_light_vector_at(const GDSLightVector* vector, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

void* gds_light_vector_assign(const GDSLightVector* vector, const void* data, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_vector_swap(const GDSLightVector* vector, size_t pos1, size_t pos2);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_vector_push_back(GDSLightVector* vector, const void* data);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_vector_insert_at(GDSLightVector* vector, const void* data, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_vector_pop_back(GDSLightVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_vector_remove_at(GDSLightVector* vector, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_vector_empty(GDSLightVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_vector_fit(GDSLightVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_vector_prealloc(GDSLightVector* vector, size_t prealloc_element_count);

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_light_vector_get_count(const GDSLightVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_light_vector_get_capacity(const GDSLightVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

bool gds_light_vector_is_empty(const GDSLightVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_light_vector_get_element_size(const GDSLightVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_light_vector_get_resize_factor(const GDSLightVector* vector);

// ---------------------------------------------------------------------------------------------------------------------

void gds_light_vector_set_resize_factor(GDSLightVector* vector, size_t resize_factor);

// ------------------------------------------------------------------------------------------------------------------------------------------

#endif // _GDS_LIGHT_VECTOR_H_
