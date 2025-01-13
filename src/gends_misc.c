#include <stdio.h>

#include "gends_misc.h"

int gends_misc_max(ssize_t x, ssize_t y)
{
    return (x > y ? x : y);
}

int gends_misc_min(ssize_t x, ssize_t y)
{
    return (x < y ? x : y);
}
