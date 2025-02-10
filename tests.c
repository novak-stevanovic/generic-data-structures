#include "gds_array.h"
#include <assert.h>
#include "gds_forward_list.h"
#include <stdio.h>
#include <string.h>

void _debug_print_my_list(GDSForwardList* list, int verbose);

int main(int argc, char *argv[])
{
    GDSForwardList* fw = gds_forward_list_create(sizeof(int), NULL);
    // GDSForwardList l;

    int a = 2;
    gds_forward_list_push_front(fw, &a);
    a = 1;
    gds_forward_list_push_front(fw, &a);

    gds_forward_list_remove_at(fw, 3);

    _debug_print_my_list(fw, 0);

    return 0;
}
