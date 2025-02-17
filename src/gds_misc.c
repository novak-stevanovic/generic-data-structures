#include <stdio.h>
#include <string.h>

#include "gds_misc.h"

int gds_misc_max(ssize_t x, ssize_t y)
{
    return (x > y ? x : y);
}

int gds_misc_min(ssize_t x, ssize_t y)
{
    return (x < y ? x : y);
}

void gds_misc_swap(void* data1, void* data2, void* swap_buff, size_t data_size)
{
    memcpy(swap_buff, data1, data_size);
    memcpy(data1, data2, data_size);
    memcpy(data2, swap_buff, data_size);

}
