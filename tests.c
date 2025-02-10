#include "gds_array.h"
#include <assert.h>
#include "gds_forward_list.h"
#include "gds_vector.h"
#include <stdio.h>
#include <string.h>

// size_t __gds_vector_chunk_list_get_sum(const _GDSVectorChunkList* chunk_list);

void _debug_print_my_vec(GDSVector* vec)
{
    // printf("PRINTING VEC: %ld %ld %d\n", gds_vector_get_count(vec), gds_vector_get_capacity(vec), __gds_vector_chunk_list_get_sum(&vec->_chunks));
    // void* el_addr;
    // for(int i = 0; i < vec->_data._count; i++)
    // {
    //     el_addr = gds_vector_at(vec, i);
    //     printf("%p ", el_addr);
    //     if(el_addr != NULL)
    //     {
    //         printf("%d\t", *(int*)el_addr);
    //     }
    // }
    // printf("\n");
}

void _debug_print_my_list(GDSForwardList* list, int verbose)
{
}

size_t get_next_chunk(GDSVector* vec, size_t last_chunk_size)
{
    return last_chunk_size + 1;
}

int main(int argc, char *argv[])
{
    return 0;
}
