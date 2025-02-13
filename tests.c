#include "gds_array.h"
#include <assert.h>
#include "gds_forward_list.h"
#include "gds_light_array.h"
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

void _debug_print_my_light_array(GDSLightArray* array)
{
    printf("PRINTING LIGHT ARRAY: %ld %ld\n", gds_light_array_get_count(array), gds_light_array_get_capacity(array));
    void* el_addr;
    for(int i = 0; i < gds_light_array_get_count(array); i++)
    {
        el_addr = gds_light_array_at(array, i);
        if(el_addr != NULL)
        {
            printf("%d\t", *(int*)el_addr);
        }
    }
    printf("\n");
}

void _debug_print_my_array(GDSArray* array)
{
    printf("PRINTING LIGHT ARRAY: %ld %ld\n", gds_array_get_count(array), gds_array_get_capacity(array));
    void* el_addr;
    for(int i = 0; i < gds_array_get_count(array); i++)
    {
        el_addr = gds_array_at(array, i);
        if(el_addr != NULL)
        {
            printf("%d\t", *(int*)el_addr);
        }
    }
    printf("\n");
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
    GDSArray* la = gds_array_create(100, sizeof(int), NULL, NULL);
    int a = 10;

    _debug_print_my_array(la);
    printf("S: %d\n", gds_array_push_back(la, &a));
    a = 17;
    printf("S: %d\n", gds_array_insert_at(la, &a, 0));
    a = 19;
    printf("S: %d\n", gds_array_push_back(la, &a));
    a = 12;
    printf("S: %d\n", gds_array_push_back(la, &a));
    _debug_print_my_array(la);
    printf("pop back status: %d\n", gds_array_pop_back(la));
    _debug_print_my_array(la);
    gds_array_destruct(la);
    _debug_print_my_array(la);
    return 0;
}
