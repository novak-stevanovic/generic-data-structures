#ifndef _GDS_FORWARD_LIST_DEF_H_
#define _GDS_FORWARD_LIST_DEF_H_

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
        // void* parameter - a pointer to the 'data' part of a list element.
        // - The data may store pointers to dynamically allocated objects. This function can be used 
        //   to properly free the memory of elements removed from the array.
    
};

#endif
