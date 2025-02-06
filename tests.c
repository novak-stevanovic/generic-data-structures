#include "gds_array.h"
#include "gds_vector.h"
#include <stdio.h>

void on_del_func(void* ptr_to_data)
{
    (**(int**)ptr_to_data)++;
    // printf("happen\n");
}

int main(int argc, char *argv[])
{
    GDSArray* arr = gds_arr_create(1069, 8, on_del_func);

    int a = 10;

    int* a_ptr = &a;

    int status;
    if((status = gds_arr_append(arr, &a_ptr)) != 0) printf("%d\n", status);
    if((status = gds_arr_append(arr, &a_ptr)) != 0) printf("%d\n", status);
    if((status = gds_arr_append(arr, &a_ptr)) != 0) printf("%d\n", status);
    if((status = gds_arr_append(arr, &a_ptr)) != 0) printf("%d\n", status);
    if((status = gds_arr_append(arr, &a_ptr)) != 0) printf("%d\n", status);

    //printf("%d\n", **(int**)gds_arr_at(arr, 3));

    // if((status = gds_arr_empty(arr)) != 0) printf("%d\n", status);

    // printf("%d\n", *(int*)gds_arr_at(arr, 2));
    //
    // printf("%d\n", a);

    if((status = gds_arr_set_size_val(arr, 0, &a_ptr)) != 0) printf("%d\n", status);

    printf("%p %d %d\n", gds_arr_at(arr, 0), gds_arr_get_count(arr), gds_arr_get_max_count(arr));

    // assegds_arr_destruct(arr);

    if((status = gds_arr_realloc(arr, -1)) != 0) printf("%d\n", status);


    printf("Success!\n");
}
