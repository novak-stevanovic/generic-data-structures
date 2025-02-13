#include "gds.h"
#include "gds_hash_map.h"

#include <stdlib.h>

#include "gds_vector.h"

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
#define __GDS_HASH_MAP_DEF_ALLOW__
#include "def/gds_hash_map_def.h"
#endif

#define __GDS_LIGHT_VECTOR_DEF_ALLOW__
#include "gds_light_vector.h"

#define _GDS_HASH_MAP_DATA_MIN_CAP 5

static size_t _gds_hash_map_data_get_next_chunk_size_func(GDSVector* data, size_t last_chunk_count)
{
    return last_chunk_count * 2;
}

static void _gds_hash_map_data_on_vector_removal_func(void* removed_vector)
{
    // TODO
}

// ------------------------------------------------------------------------------------------------------------------------------------------

gds_err gds_hash_map_init(GDSHashMap* hash_map,
        size_t key_data_size, size_t value_data_size,
        size_t (*hash_func)(void* data, size_t max_val),
        void (*on_removal_func)(void*))
{
#ifdef GDS_INIT_MAX_SIZE

    if((key_data_size > GDS_INIT_MAX_SIZE) || (value_data_size)) return GDS_ERR_MAX_INIT_SIZE_EXCEED;

#endif // GDS_INIT_MAX_SIZE

    if(hash_map == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(key_data_size == 0) return GDS_GEN_ERR_INVALID_ARG(2);
    if(value_data_size == 0) return GDS_GEN_ERR_INVALID_ARG(3);
    if(hash_func == NULL) return GDS_GEN_ERR_INVALID_ARG(4);

    hash_map->_key_data_size = key_data_size;
    hash_map->_value_data_size = value_data_size;
    hash_map->_on_key_removal_func = on_removal_func;
    hash_map->_hash_func = hash_func;
    hash_map->_entry_count = 0;
    // TODO add removal func, check the get next size func

    gds_err data_init_status = gds_vector_init(&hash_map->_data,
            sizeof(GDSLightVector*),
            _gds_hash_map_data_on_vector_removal_func,
            _GDS_HASH_MAP_DATA_MIN_CAP,
            _gds_hash_map_data_get_next_chunk_size_func, NULL);

    if(data_init_status != GDS_SUCCESS) return GDS_HASH_MAP_ERR_INIT_FAIL;

    return GDS_SUCCESS;
}

GDSHashMap* gds_hash_map_create(size_t key_data_size, size_t value_data_size,
        size_t (*hash_func)(void* data, size_t max_val),
        void (*on_removal_func)(void*))
{
    GDSHashMap* hash_map = (GDSHashMap*)malloc(sizeof(GDSHashMap));

    if(hash_map == NULL) return NULL;

    gds_err init_status = gds_hash_map_init(hash_map, key_data_size, value_data_size, hash_func, on_removal_func);

    if(init_status != GDS_SUCCESS)
    {
        free(hash_map);
        return NULL;
    }
    else return hash_map;
}

gds_err gds_hash_map_set(GDSHashMap* hash_map, void* key, void* value)
{
    // TODO
    //
    return GDS_SUCCESS;
}
