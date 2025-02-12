#ifndef __GDS_LIGHT_VECTOR_DEF_H__
#define __GDS_LIGHT_VECTOR_DEF_H__

#include "gds.h"

#include <stddef.h>

#ifndef __GDS_LIGHT_VECTOR_DEF_ALLOW__
#error "Don't include directly."
#endif // __GDS_LIGHT_VECTOR_DEF_ALLOW__

struct GDSLightVector
{
    void* _data;
    size_t _count;
    size_t _capacity;
    size_t _element_size;
    double _resize_factor;

    #ifdef GDS_TEMP_BUFF_USE_SWAP_BUFF
    void* _swap_buff;
    #endif // GDS_TEMP_BUFF_USE_SWAP_BUFF
};

#endif // __GDS_LIGHT_VECTOR_DEF_H__

