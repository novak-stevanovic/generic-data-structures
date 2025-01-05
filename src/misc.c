#include "misc.h"
#include <stdio.h>

int misc_max(ssize_t x, ssize_t y)
{
    return (x > y ? x : y);
}

int misc_min(ssize_t x, ssize_t y)
{
    return (x < y ? x : y);
}
