#include "gds.h"

#define __GDS_VECTOR_CHUNK_LIST_DEF_ALLOW__
#include "internal/_gds_vector_chunk_list.h"

#include <assert.h>

#include "gds_forward_list.h"

gds_err _gds_vector_chunk_list_init(_GDSVectorChunkList* chunk_list, size_t min_count)
{
    assert(chunk_list != NULL);

    GDSForwardList* _chunk_list = &chunk_list->_list;

    gds_err init_status = gds_forward_list_init(_chunk_list, sizeof(size_t), NULL);
    if(init_status != GDS_SUCCESS) return GDS_FAILURE;

    gds_forward_list_push_back(_chunk_list, &min_count);

    return GDS_SUCCESS;
}

void _gds_vector_chunk_list_destruct(_GDSVectorChunkList* chunk_list)
{
    assert(chunk_list != NULL);

    gds_forward_list_destruct(&chunk_list->_list);
}

// Assumed not to shrink by more than the total alloced sum.
void _gds_vector_chunk_list_shrink_by(_GDSVectorChunkList* chunk_list, size_t amount)
{
    assert(chunk_list != NULL);

    GDSForwardList* _chunk_list = &chunk_list->_list;

    size_t curr_amount = 0;
    size_t buff;
    while(amount != 0)
    {
        curr_amount = *(size_t*)gds_forward_list_at(_chunk_list, 0);
        if(curr_amount > amount)
        {
            buff = curr_amount - amount;
            gds_forward_list_assign(_chunk_list, &buff, 0); // assign the leftover value to head.
            amount = 0;

        }
        else if(curr_amount == amount) // 
        {
            gds_forward_list_pop_front(_chunk_list);
            amount = 0;
        }
        else // curr_amount < amount
        {
            amount -= curr_amount;
            gds_forward_list_pop_front(_chunk_list);
        }
    }
    
} 

gds_err _gds_vector_chunk_list_add_new_chunk(_GDSVectorChunkList* chunk_list, size_t new_chunk_amount)
{
    assert(chunk_list != NULL);

    gds_forward_list_push_front(&chunk_list->_list, &new_chunk_amount);

    return GDS_SUCCESS;
}

size_t _gds_vector_chunk_list_get_last_chunk_size(const _GDSVectorChunkList* chunk_list)
{
    return (*(size_t*)gds_forward_list_at(&chunk_list->_list, 0));
}

size_t _gds_vector_chunk_list_get_min_size(const _GDSVectorChunkList* chunk_list)
{
    return (*(size_t*)gds_forward_list_at(&chunk_list->_list,
                gds_forward_list_get_count(&chunk_list->_list) - 1));
}

size_t __gds_vector_chunk_list_get_sum(const _GDSVectorChunkList* chunk_list)
{
    int i;
    void* curr_el_data;
    int sum = 0;
    for(i = 0; i < chunk_list->_list._count; i++)
    {
        curr_el_data = gds_forward_list_at(&chunk_list->_list, i);
        sum += *(size_t*)curr_el_data;
    }

    return sum;
}

