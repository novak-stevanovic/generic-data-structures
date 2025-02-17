// INTERNAL HEADER FILE - DO NOT INCLUDE DIRECTLY.

#ifndef __GDSE_VECTOR_DEF_H__
#define __GDSE_VECTOR_DEF_H__

#include "gds.h"

#ifndef __GDSE_VECTOR_DEF_ALLOW__
#error "Do not include directly."
#endif // __GDSE_VECTOR_DEF_ALLOW__

#include <stdbool.h>

#define __GDS_VECTOR_DEF_ALLOW__
#include "gds_vector_def.h"

struct GDSEVector
{
    struct GDSVector _vector;

    bool (*_compare_func)(void*, void*);
    void (*_on_removal_callback_func)(void*);

#ifdef GDS_TEMP_BUFF_USE_SWAP_BUFF
    void* __GDS_SWAP_BUFF_NAME;
#endif // GDS_TEMP_BUFF_USE_SWAP_BUFF

};

#endif // __GDSE_VECTOR_DEF_H__
