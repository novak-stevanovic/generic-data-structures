#ifndef _GDS_HASH_MAP_DEF_H_
#define _GDS_HASH_MAP_DEF_H_

#ifndef __GDS_HASH_MAP_DEF_ALLOW__
#error "Do not include directly."
#endif // __GDS_HASHMAP_DEF_ALLOW__

#define __GDS_VECTOR_DEF_ALLOW__
#include "gds_vector_def.h"

struct GDSHashMap
{
    struct GDSVector data;

    size_t (*_hash_func)(void* data, size_t max_val);
    void (*_on_removal_func)(void*);
};

#endif // _GDS_HASH_MAP_DEF_H_
