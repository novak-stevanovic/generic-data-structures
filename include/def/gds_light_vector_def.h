// INTERNAL HEADER FILE - DO NOT INCLUDE DIRECTLY.

#ifndef __GDS_LIGHT_VECTOR_DEF_H__
#define __GDS_LIGHT_VECTOR_DEF_H__

#include "gds.h"

#ifndef __GDS_LIGHT_VECTOR_DEF_ALLOW__
#error "Do not include directly."
#endif // __GDS_LIGHT_VECTOR_DEF_ALLOW__

// ---------------------------------------------------------------------------------------------------------------------------------------

#define __GDS_LIGHT_ARRAY_DEF_ALLOW__
#include "gds_light_array_def.h"

struct GDSLightVector
{
    struct GDSLightArray _data;
    double _resize_factor;
};

#endif // __GDS_LIGHT_VECTOR_DEF_H__
