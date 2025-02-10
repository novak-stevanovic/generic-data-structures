#include "gds_array.h"
#include <assert.h>
#include "gds_forward_list.h"
#include <stdio.h>
#include <string.h>

void _debug_print_my_list(GDSForwardList* list, int verbose)
{
    if(verbose)
    {
    }
    else
    {
        void* it = NULL;
        int i;
        for(i = 0; i < gds_forward_list_get_count(list); i++)
        {
            it = gds_forward_list_at(list, i);
            printf("%d ", *(int*)it);

        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    GDSForwardList* fw = gds_forward_list_create(sizeof(int), NULL);
    // GDSForwardList l;

    int a = 1;
    gds_forward_list_push_back(fw, &a);

    a = 2;
    gds_forward_list_push_back(fw, &a);
    a = 0;
    gds_forward_list_push_front(fw, &a);
    a = 3;
    gds_forward_list_insert_at(fw, &a, 2);
    a = 3000;
    // gds_forward_list_push_front(fw, &a);

    _debug_print_my_list(fw, 0);

    gds_forward_list_empty(fw);

    _debug_print_my_list(fw, 0);

    printf("%d\n", gds_forward_list_is_empty(fw));

    return 0;
}
