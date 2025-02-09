// INTERNAL HEADER FILE - DO NOT INCLUDE DIRECTLY.

#ifndef __GDS_VECTOR_DEF_H__
#define __GDS_VECTOR_DEF_H__

#ifndef __GDS_VECTOR_DEF_ALLOW__
#error "Do not include directly."
#endif // __GDS_VECTOR__DEF__ALLOW__

// ---------------------------------------------------------------------------------------------------------------------------------------

#define __GDS_ARRAY_DEF_ALLOW__
#include "gds_array_def.h"

#define __GDS_FORWARD_LIST_DEF_ALLOW__
#include "gds_forward_list_def.h"

struct GDSVector
{
    struct GDSArray _data;

    struct GDSForwardList _chunks;
    size_t (*_get_next_chunk_size)(struct GDSVector*, void*);
};

#endif // __GDS_VECTOR_DEF_H__
