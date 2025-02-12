// INTERNAL HEADER FILE - DO NOT INCLUDE DIRECTLY.

#include "gds.h"

#ifndef __GDS_ARRAY_DEF_H__
#define __GDS_ARRAY_DEF_H__

#ifndef __GDS_ARRAY_DEF_ALLOW__
#error "Do not include directly."
#endif // __GDS_ARRAY_DEF_ALLOW__

// ---------------------------------------------------------------------------------------------------------------------------------------

#include <stddef.h>

struct GDSArray
{
    size_t _count; // current count of elements,
    size_t _capacity; // array capacity,
    size_t _element_size; // size of each element,
    void* _data; // address of array's data beginning.
    void (*_on_element_removal_func)(void*); // pointer to a callback function that is called on element removal, for each removed element.
        // void* parameter - a pointer to the element stored in the array. Note:
        // - The array may store pointers to dynamically allocated objects. This function can be used 
        //   to properly free the memory of elements removed from the array.
        // - In this case, the `void*` parameter represents a pointer to an element inside the array,
        //   which itself is a pointer to a dynamically allocated object */
    #ifdef GDS_TEMP_BUFF_USE_SWAP_BUFF
    void* _swap_buff;
    #endif // GDS_TEMP_BUFF_USE_SWAP_BUFF
};

#endif
