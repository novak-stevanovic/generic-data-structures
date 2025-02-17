#include "gds.h"
#include "gds_array.h"
#include "gds_vector.h"
#include "gds_hash_map.h"

#include <stdlib.h>

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
#define __GDS_HASH_MAP_DEF_ALLOW__
#include "def/gds_hash_map_def.h"
#endif

typedef struct 
{
    void* key_ptr;
    void* value_ptr;
} _GDSBucketElement;

static _GDSBucketElement* _gds_hash_map_get_bucket_element(const GDSHashMap* hash_map, const void* key)
{
    size_t hash_code = hash_map->_hash_func(key, gds_vector_get_capacity(&hash_map->_data));

    GDSVector** bucket = gds_vector_at(&hash_map->_data, hash_code);

    if((*bucket) == NULL) return NULL;
    size_t bucket_element_count = gds_vector_get_count(*bucket);
    size_t i;
    _GDSBucketElement* curr_bucket_element;

    for(i = 0; i < bucket_element_count; i++)
    {
        curr_bucket_element = gds_vector_at(*bucket, i);

        if(hash_map->_key_compare_func(curr_bucket_element->key_ptr, key) == 0) 
            return curr_bucket_element;
    }

    return NULL;
}

static void _gds_bucket_element_init(_GDSBucketElement* bucket_element, GDSHashMap* hash_map, const void* key, const void* value)
{
    size_t key_size = hash_map->_key_data_size;
    size_t value_size = hash_map->_value_data_size;

    bucket_element->key_ptr = malloc(key_size);
    bucket_element->value_ptr = malloc(value_size);

    memcpy(bucket_element->key_ptr, key, key_size);
    memcpy(bucket_element->value_ptr, value, value_size);
}

static void _gds_hash_map_fill_vector(GDSHashMap* hash_map)
{
    GDSVector* vector = &hash_map->_data;
    size_t vec_count = gds_vector_get_count(vector);
    size_t vec_capacity = gds_vector_get_capacity(vector);

    size_t i;
    
    for(i = vec_count * vector->_data._element_size; i < vec_capacity * vector->_data._element_size; i++)
    {
        ((char*)vector->_data._data)[i] = 0;
    }

    vector->_data._count = vec_capacity;
}

gds_err gds_hash_map_init(GDSHashMap* hash_map, size_t key_data_size, size_t value_data_size,
        size_t (*hash_func)(const void* key, size_t max_value),
        bool (*key_compare_func)(const void* key1, const void* key2))
{
    if(hash_map == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(key_data_size == 0) return GDS_GEN_ERR_INVALID_ARG(2);
    if(value_data_size == 0) return GDS_GEN_ERR_INVALID_ARG(3);
    if(hash_map == NULL) return GDS_GEN_ERR_INVALID_ARG(4);
    if(key_compare_func == NULL) return GDS_GEN_ERR_INVALID_ARG(5);

    hash_map->_hash_func = hash_func;
    hash_map->_key_data_size = key_data_size;
    hash_map->_value_data_size = value_data_size;
    hash_map->_key_compare_func = key_compare_func;
    hash_map->_entry_count = 0;

    gds_vector_init(&hash_map->_data, sizeof(GDSVector*), 16, 2);
    _gds_hash_map_fill_vector(hash_map);

    return GDS_SUCCESS;
}

GDSHashMap* gds_hash_map_create(size_t key_data_size, size_t value_data_size, size_t (*hash_func)(const void* key, size_t max_value),
        bool (*key_compare_func)(const void* key1, const void* key2))
{
    GDSHashMap* hash_map = (GDSHashMap*)malloc(sizeof(GDSHashMap));

    if(hash_map == NULL) return NULL;

    gds_err init_status = gds_hash_map_init(hash_map, key_data_size, value_data_size, hash_func, key_compare_func);

    if(init_status == GDS_SUCCESS) return hash_map;
    else
    {
        free(hash_map);
        return NULL;
    }
}

gds_err gds_hash_map_set(GDSHashMap* hash_map, void* key, void* value)
{
    size_t vector_capacity = gds_vector_get_capacity(&hash_map->_data);
    size_t hash_code = hash_map->_hash_func(key, vector_capacity);

    GDSVector** bucket = gds_vector_at(&hash_map->_data, hash_code);

    if((*bucket) == NULL) 
    {
        *bucket = gds_vector_create_default(sizeof(_GDSBucketElement));

    }
    else
    {
        _GDSBucketElement* bucket_element = _gds_hash_map_get_bucket_element(hash_map, key);
        if(bucket_element == NULL)
        {
            _GDSBucketElement bucket_element;
            _gds_bucket_element_init(&bucket_element, hash_map, key, value);
            gds_vector_push_back(*bucket, &bucket_element); // the structure GDSBucketElement is copied into the vector
        }
        else
        {
            bucket_element->value_ptr = value;
        }
    }

    return GDS_SUCCESS;
}

void* gds_hash_map_get(const GDSHashMap* hash_map, void* key)
{
    _GDSBucketElement* bucket_element = _gds_hash_map_get_bucket_element(hash_map, key);
    return (bucket_element != NULL) ? bucket_element->value_ptr : NULL;
}
