#include "gends_vector.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    struct Vector* v = vec_init(10, 15, sizeof(int));

    int i = 10;
    vec_append(v, &i);
    vec_append(v, &i);
    vec_append(v, &i);
    vec_append(v, &i);
    vec_append(v, &i);
    vec_append(v, &i);
    vec_append(v, &i);
    vec_append(v, &i);
    vec_append(v, &i);
    vec_append(v, &i);

    vec_empty(v);
    printf("%ld %ld %p\n", vec_get_count(v), vec_get_resize_count(v), vec_get_data(v));
}
