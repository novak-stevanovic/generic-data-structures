// INTERNAL HEADER FILE - DO NOT INCLUDE DIRECTLY.

#ifndef __GDSE_ARRAY_DEF_H__
#define __GDSE_ARRAY_DEF_H__

#include "gds.h"

#ifndef __GDSE_ARRAY_DEF_ALLOW__
#error "Do not include directly."
#endif // __GDSE_ARRAY_DEF_ALLOW__

#include <stdbool.h>

#define __GDS_ARRAY_DEF_ALLOW__
#include "gds_array_def.h"

struct GDSEArray
{
    struct GDSArray _array;

    bool (*_compare_func)(void*, void*);
    void (*_on_removal_callback_func)(void*);

#ifdef GDS_TEMP_BUFF_USE_SWAP_BUFF
    void* __GDS_SWAP_BUFF_NAME;
#endif // GDS_TEMP_BUFF_USE_SWAP_BUFF

};

#endif // __GDSE_ARRAY_DEF_H__
