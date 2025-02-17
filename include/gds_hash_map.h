#ifndef _GDS_HASH_MAP_H_
#define _GDS_HASH_MAP_H_

#include "gds.h"
#include <stddef.h>

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
struct GDSHashMap;
#else
#define __GDS_HASH_MAP_DEF_ALLOW__
#include "def/gds_hash_map_def.h"
#endif

typedef struct GDSHashMap GDSHashMap;

gds_err gds_hash_map_init(GDSHashMap* hash_map,
        size_t key_data_size, size_t value_data_size,
        size_t (*hash_func)(void* key));

GDSHashMap* gds_hash_map_create(size_t key_data_size, size_t value_data_size,
        size_t (*hash_func)(void* key));

gds_err gds_hash_map_set(GDSHashMap* hash_map, void* key, void* value);

#endif // _GDS_HASH_MAP_H_
