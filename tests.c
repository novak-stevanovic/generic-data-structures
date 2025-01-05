#include <assert.h>
#include <stdio.h>
#include "array.h"
#include "vector.h"

void print_vector(struct Vector* v);

int main(int argc, char *argv[])
{
    int i,j;
    int a;
    struct Vector* v1 = vec_init(124, 20, vec_get_struct_size());
    struct Array* a1 = arr_init(1000000, sizeof(int));

    for (i = 0; i < 1000; i++) {
        int append_status1 = vec_append(v1, vec_init(4, 200, vec_get_struct_size()));
        struct Vector* curr_row = vec_at(v1, i);
        a = i;
        for(j = 140; j > 0; j--)
        {
            int append_status2 = vec_append(curr_row, &a);
            int append_status3 = arr_append(a1, &j);
        }
    }

    printf("VECTOR: ");
    printf("%ld %p %ld\n", vec_get_count(v1),vec_get_data(v1), vec_get_resize_count(v1));
    for(i = 0; i < 1000; i++)
    {
        struct Vector* curr_v = vec_at(v1, i);
        printf("%ld %p %ld\n", vec_get_count(curr_v),vec_get_data(curr_v), vec_get_resize_count(curr_v));
    }

    for(i = 0; i < 140000; i++)
    {
        int* curr_a = arr_at(a1, i);
        printf("i : %d, %d ", i, *curr_a);
    }
    return 0;
}
