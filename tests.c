#include "gds_vector.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    struct GDSVector* v = gds_vec_create(10, 15, sizeof(int));

    int i = 10;
    gds_vec_append(v, &i);
    gds_vec_append(v, &i);
    gds_vec_append(v, &i);
    gds_vec_append(v, &i);
    gds_vec_append(v, &i);
    gds_vec_append(v, &i);
    gds_vec_append(v, &i);
    gds_vec_append(v, &i);
    gds_vec_append(v, &i);
    gds_vec_append(v, &i);

    gds_vec_empty(v);
    printf("%ld %ld %p\n", gds_vec_get_count(v), gds_vec_get_resize_count(v), gds_vec_get_data(v));
}
