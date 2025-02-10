// INTERNAL HEADER FILE - DO NOT INCLUDE DIRECTLY.

#ifndef __GDS_VECTOR_CHUNK_LIST_H__
#define __GDS_VECTOR_CHUNK_LIST_H__

#include "gds.h"

#ifndef __GDS_VECTOR_CHUNK_LIST_H_ALLOW__
#error "Do not include directly."
#endif // __GDS_VECTOR_CHUNK_LIST_H_ALLOW__ 

#define __GDS_FORWARD_LIST_DEF_ALLOW__
#include "def/gds_forward_list_def.h"

typedef struct _GDSVectorChunkList
{
    struct GDSForwardList _list;
} _GDSVectorChunkList;

gds_err _gds_vector_chunk_list_init(_GDSVectorChunkList* chunk_list, size_t min_count);

void _gds_vector_chunk_list_destruct(_GDSVectorChunkList* chunk_list);

// Assumed not to shrink by more than the total alloced sum.
void _gds_vector_chunk_list_shrink_by(_GDSVectorChunkList* chunk_list, size_t amount);

gds_err _gds_vector_chunk_list_add_new_chunk(_GDSVectorChunkList* chunk_list, size_t new_chunk_amount);

// Function assumes that there are existing chunks in the chunk list and chunk list is not NULL.
size_t _gds_vector_chunk_list_get_last_chunk_size(const _GDSVectorChunkList* chunk_list);

// Function assumes that there are existing chunks in the chunk list and chunk list is not NULL.
size_t _gds_vector_chunk_list_get_min_size(const _GDSVectorChunkList* chunk_list);

#endif //__GDS_VECTOR_CHUNK_LIST_H__
