#include <assert.h>
#include <stdio.h>
#include "vector.h"

void print_vector(struct Vector* v);

int main(int argc, char *argv[])
{
    int i,j;
    int a;
    struct Vector* v = vec_init(124, 20, vec_get_struct_size());

    for (i = 0; i < 100000; i++) {
        int append_status1 = vec_append(v, vec_init(4, 200, vec_get_struct_size()));
        assert(append_status1 == 0);
        struct Vector* curr_row = vec_at(v, i);
        a = i;
        for(j = 140; j > 0; j--)
        {
            int append_status2 = vec_append(curr_row, &a);
            assert(append_status2 == 0);
        }
    }
    return 0;
}
