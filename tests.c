#include "gds_array.h"
#include <assert.h>
#include "gds_forward_list.h"
#include "gds_vector.h"
#include <stdio.h>
#include <string.h>

// size_t __gds_vector_chunk_list_get_sum(const _GDSVectorChunkList* chunk_list);

int main(int argc, char *argv[])
{
    // GDSVector* v = gds_vector_create(sizeof(size_t), 100, 1.2, true);
    //
    // size_t a = 5;
    //
    // gds_err status;
    // status = gds_vector_push_back(v, &a);
    //
    // a = 6;
    // status = gds_vector_push_back(v, &a);
    //
    // a = 1;
    // status = gds_vector_insert_at(v, &a, 0);
    //
    // a = 1;
    // status = gds_vector_insert_at(v, &a, 0);
    //
    // a = 2;
    // status = gds_vector_insert_at(v, &a, 1);
    // status = gds_vector_insert_at(v, &a, 1);
    // status = gds_vector_insert_at(v, &a, 1);
    // status = gds_vector_insert_at(v, &a, 1);
    // status = gds_vector_insert_at(v, &a, 1);
    // status = gds_vector_insert_at(v, &a, 1);
    // status = gds_vector_insert_at(v, &a, 1);
    // status = gds_vector_insert_at(v, &a, 1);
    // status = gds_vector_insert_at(v, &a, 1);
    // status = gds_vector_insert_at(v, &a, 1);
    //
    // status = gds_vector_reserve(v, 1000000);
    //
    // status = gds_vector_insert_at(v, &a, 1);
    // status = gds_vector_insert_at(v, &a, 1);
    // status = gds_vector_insert_at(v, &a, 1);
    // status = gds_vector_insert_at(v, &a, 1);
    // status = gds_vector_insert_at(v, &a, 1);
    // status = gds_vector_insert_at(v, &a, 1);
    // status = gds_vector_insert_at(v, &a, 1);
    // status = gds_vector_insert_at(v, &a, 1);
    // status = gds_vector_insert_at(v, &a, 1);
    // status = gds_vector_insert_at(v, &a, 1);
    // status = gds_vector_insert_at(v, &a, 1);
    //
    // status = gds_vector_fit(v);
    //
    // // GDSArray* la = gds_array_create(100, sizeof(int), NULL, NULL);
    // // int a = 10000000;
    // //
    // // _debug_print_my_array(la);
    // // gds_array_push_back(la, &a);
    // // a = 17;
    // // gds_array_insert_at(la, &a, 0);
    // // a = 19;
    // // gds_array_push_back(la, &a);
    // // a = 12;
    // // gds_array_push_back(la, &a);
    // // _debug_print_my_array(la);
    // // gds_array_pop_back(la);
    // // _debug_print_my_array(la);
    // // gds_array_destruct(la);
    // // _debug_print_my_array(la);
    // a = 0;
    // status = gds_vector_insert_at(v, &a, 0);
    //
    char a[100] = {0};
    void* pok = *(void**)(a + 10);
    printf("%p\n", pok);
    int i;
    for(i = 0; i < 100; i++)
    {
        printf("%d ", a[i]);
    }
    printf("\n");
    return 0;
}
