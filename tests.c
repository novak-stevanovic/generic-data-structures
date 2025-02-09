#include "gds_array.h"
#include <assert.h>
#include "gds_forward_list.h"
#include <stdio.h>
#include <string.h>

#define __GDS_VECTOR_DEF_ALLOW__
#include "def/gds_vector.h"

void _debug_print_my_list(GDSForwardList* list, int verbose);

int main(int argc, char *argv[])
{
    GDSForwardList* fw = gds_forward_list_create(sizeof(int), NULL);
    // GDSForwardList l;

    int a = 0;
    assert(gds_forward_list_push_back(fw, &a) == 0);
    a = 1;
    _debug_print_my_list(fw, 0);
    gds_forward_list_insert_at(fw, &a, 0);
    _debug_print_my_list(fw, 0);
    gds_forward_list_swap(fw, 1, 0);
    _debug_print_my_list(fw, 0);

    printf("Success - %ld\n", sizeof(struct GDSVector));
    return 0;
}
