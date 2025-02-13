// INTERNAL HEADER FILE - DO NOT INCLUDE DIRECTLY.

#include "gds.h"

#ifndef __GDS_LIGHT_ARRAY_DEF_H__
#define __GDS_LIGHT_ARRAY_DEF_H__

#ifndef __GDS_LIGHT_ARRAY_DEF_ALLOW__
#error "Do not include directly."
#endif // __GDS_LIGHT_ARRAY_DEF_ALLOW__

// ---------------------------------------------------------------------------------------------------------------------------------------

#include <stddef.h>

struct GDSLightArray
{
    size_t _count; // current count of elements,
    size_t _capacity; // array capacity,
    size_t _element_size; // size of each element,
    void* _data; // address of array's data beginning.
    #ifdef GDS_TEMP_BUFF_USE_SWAP_BUFF
    void* _swap_buff;
    #endif // GDS_TEMP_BUFF_USE_SWAP_BUFF
};

#endif // __GDS_LIGHT_ARRAY_DEF_H__
