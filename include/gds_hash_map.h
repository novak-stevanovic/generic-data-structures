// #ifndef _GDS_HASH_MAP_H_
// #define _GDS_HASH_MAP_H_
//
// #include "gds.h"
// #include <stddef.h>
//
// #ifdef GDS_ENABLE_OPAQUE_STRUCTS
// struct GDSHashMap;
// #else
// #define __GDS_HASH_MAP_DEF_ALLOW__
// #include "def/gds_hash_map_def.h"
// #endif
//
// typedef struct GDSHashMap GDSHashMap;
//
// #define GDS_HASH_MAP_ERR_BASE 3000
// // #define GDS_VEC_ERR_VEC_EMPTY 3001
// #define GDS_HASH_MAP_ERR_MALLOC_FAIL 3002
// #define GDS_HASH_MAP_ERR_REALLOC_FAIL 3003
// #define GDS_HASH_MAP_ERR_INIT_FAIL 3004
// // #define GDS_VEC_ERR_CHUNK_GEN_FUNC_FAIL 3005 // Occurs when vector->_get_next_chunk_size_func returns 0
//
// gds_err gds_hash_map_init(GDSHashMap* hash_map,
//         size_t key_data_size, size_t value_data_size,
//         size_t (*hash_func)(void* data, size_t max_val),
//         void (*on_removal_func)(void*));
//
// GDSHashMap* gds_hash_map_create(size_t key_data_size, size_t value_data_size,
//         size_t (*hash_func)(void* data, size_t max_val),
//         void (*on_removal_func)(void*));
//
// gds_err gds_hash_map_set(GDSHashMap* hash_map, void* key, void* value);
//
// #endif // _GDS_HASH_MAP_H_
