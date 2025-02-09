#ifndef _GDS_ARRAY_DEF_H_
#define _GDS_ARRAY_DEF_H_

#include <stddef.h>

struct GDSArray
{
    size_t _count; // current count of elements,
    size_t _max_count; // array capacity,
    size_t _element_size; // size of each element,
    void* _data; // address of array's data beginning.
    void (*_on_element_removal_func)(void*); // pointer to a callback function that is called on element removal, for each removed element.
        // void* parameter - a pointer to the element stored in the array. Note:
        // - The array may store pointers to dynamically allocated objects. This function can be used 
        //   to properly free the memory of elements removed from the array.
        // - In this case, the `void*` parameter represents a pointer to an element inside the array,
        //   which itself is a pointer to a dynamically allocated object */
};

#endif
