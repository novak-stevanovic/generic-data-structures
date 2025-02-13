// INTERNAL HEADER FILE - DO NOT INCLUDE DIRECTLY.

#ifndef __GDS_ARRAY_DEF_H__
#define __GDS_ARRAY_DEF_H__

#include "gds.h"

#include <stdbool.h>

#ifndef __GDS_ARRAY_DEF_ALLOW__
#error "Do not include directly."
#endif // __GDS_ARRAY_DEF_ALLOW__

#define __GDS_LIGHT_ARRAY_DEF_ALLOW__
#include "def/gds_light_array_def.h"

// ---------------------------------------------------------------------------------------------------------------------------------------

#include <stddef.h>

struct GDSArray
{
    struct GDSLightArray _base;
    bool (*_compare_func)(void*, void*);
    void (*_on_element_removal_func)(void*); // pointer to a callback function that is called on element removal, for each removed element.
        // void* parameter - a pointer to the element stored in the array. Note:
        // - The array may store pointers to dynamically allocated objects. This function can be used 
        //   to properly free the memory of elements removed from the array.
        // - In this case, the `void*` parameter represents a pointer to an element inside the array,
        //   which itself is a pointer to a dynamically allocated object */
};

#endif // __GDS_ARRAY_DEF_H__
