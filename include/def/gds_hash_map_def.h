// INTERNAL HEADER FILE - DO NOT INCLUDE DIRECTLY.

#ifndef __GDS_HASH_MAP_DEF_H__
#define __GDS_HASH_MAP_DEF_H__

#ifndef __GDS_HASH_MAP_DEF_ALLOW__
#error "Do not include directly."
#endif // __GDS_HASH_MAP_DEF_ALLOW__

#include <stdbool.h>

#define __GDS_VECTOR_DEF_ALLOW__
#include "gds_vector_def.h"

struct GDSHashMap
{
    struct GDSVector _data;

    size_t _entry_count;
    size_t _key_data_size, _value_data_size;
    size_t (*_hash_func)(void* data, size_t max_val);
    bool (*_key_compare_func)(void* key1, void* key2);
    void (*_on_key_removal_func)(void*);
    void (*_on_value_removal_func)(void*);
};

#endif // __GDS_HASH_MAP_DEF_H__
