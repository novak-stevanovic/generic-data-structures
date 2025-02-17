#include "gds.h"
#include "gds_array.h"
#include "gds_vector.h"
#include "gds_hash_map.h"

#include <stdlib.h>

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
#define __GDS_HASH_MAP_DEF_ALLOW__
#include "def/gds_hash_map_def.h"
#endif

gds_err gds_hash_map_init(GDSHashMap* hash_map,
        size_t key_data_size, size_t value_data_size,
        size_t (*hash_func)(void* key))
{
    if(hash_map == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(key_data_size == 0) return GDS_GEN_ERR_INVALID_ARG(2);
    if(value_data_size == 0) return GDS_GEN_ERR_INVALID_ARG(3);
    if(hash_map == NULL) return GDS_GEN_ERR_INVALID_ARG(4);

    hash_map->_hash_func = hash_func;
    hash_map->_key_data_size = key_data_size;
    hash_map->_value_data_size = value_data_size;
    hash_map->_entry_count = 0;

    gds_vector_init(&hash_map->_data, sizeof(GDSVector*), 16, 2);

    return GDS_SUCCESS;
}

GDSHashMap* gds_hash_map_create(size_t key_data_size, size_t value_data_size,
        size_t (*hash_func)(void* data))
{
    GDSHashMap* hash_map = (GDSHashMap*)malloc(sizeof(GDSHashMap));

    if(hash_map == NULL) return NULL;

    gds_err init_status = gds_hash_map_init(hash_map, key_data_size, value_data_size, hash_func);

    if(init_status == GDS_SUCCESS) return hash_map;
    else
    {
        free(hash_map);
        return NULL;
    }
}

gds_err gds_hash_map_set(GDSHashMap* hash_map, void* key, void* value)
{
}
