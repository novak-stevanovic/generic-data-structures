// INTERNAL HEADER FILE - DO NOT INCLUDE DIRECTLY.

#ifndef __GDS_FORWARD_LIST_DEF_H__
#define __GDS_FORWARD_LIST_DEF_H__

#ifndef __GDS_FORWARD_LIST_DEF_ALLOW__
#error "Do not include directly."
#endif // __GDS_FORWARD_LIST_DEF_ALLOW__

// ---------------------------------------------------------------------------------------------------------------------------------------

#include <stddef.h>

struct _GDSForwardListNodeBase
{
    struct _GDSForwardListNodeBase* next;
};

struct GDSForwardList
{
    struct _GDSForwardListNodeBase* _head;
    struct _GDSForwardListNodeBase* _tail;

    size_t _count;
    size_t _data_size;

    void (*_on_element_removal_func)(void*); // pointer to a callback function that is called on element removal, for each removed element.
        // void* parameter - address of data in node.
        // - The node may store pointers to dynamically allocated objects. This function can be used 
        //   to properly free the dynamically allocated memory.
    
    void* _swap_buff;
};

#endif // __GDS_FORWARD_LIST_DEF_H__
