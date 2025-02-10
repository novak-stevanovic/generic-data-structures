// INTERNAL HEADER FILE - DO NOT INCLUDE DIRECTLY.

#ifndef __GDS_VECTOR_DEF_H__
#define __GDS_VECTOR_DEF_H__

#ifndef __GDS_VECTOR_DEF_ALLOW__
#error "Do not include directly."
#endif // __GDS_VECTOR__DEF__ALLOW__

// ---------------------------------------------------------------------------------------------------------------------------------------

#define __GDS_ARRAY_DEF_ALLOW__
#include "gds_array_def.h"

#define __GDS_VECTOR_CHUNK_LIST_H_ALLOW__
#include "_gds_vector_chunk_list.h"

struct GDSVector
{
    struct GDSArray _data;

    struct _GDSVectorChunkList _chunks;
    size_t (*_get_next_chunk_size_func)(struct GDSVector* vector, size_t last_chunk_size);
};

#endif // __GDS_VECTOR_DEF_H__
