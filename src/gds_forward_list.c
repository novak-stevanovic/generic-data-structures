#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "gds.h"
#include "gds_misc.h"
#include "gds_forward_list.h"

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
#define __GDS_FORWARD_LIST_DEF_ALLOW__
#include "def/gds_forward_list_def.h"
#endif // GDS_ENABLE_OPAQUE_STRUCTS

typedef struct _GDSForwardListNodeBase _GDSForwardListNodeBase;

// ------------------------------------------------------------------------------------------------------------------------------------------

/* Dynamically allocates sizeof(_GDSForwardListNodeBase) + list->_data_size bytes for the list node. Returns NULL if
 * it fails. The function assumes that 'list' is non-NULL and 'data' is non-NULL. */
static _GDSForwardListNodeBase* _gds_forward_list_alloc_node(const GDSForwardList* list, const void* data);

// ---------------------------------------------------------------------------------------------------------------------

/* Returns address of node with index 'pos'. Function assumes non-NULL 'list' argument and that 'pos' is in bounds
 * ('pos' < list->_count). */
static _GDSForwardListNodeBase* _gds_forward_list_at_node(const GDSForwardList* list, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

/* Function to call when removing nodes from the list. This function assumes non-NULL argument 'list' and 'node'.
 * First, the function calls list->_on_element_removal_func if non-NULL.
 * Then, it frees the node pointed to by 'node'. */
static void _gds_forward_list_on_node_removal(const GDSForwardList* list, _GDSForwardListNodeBase* node);

// ---------------------------------------------------------------------------------------------------------------------

/* Function assumes non-NULL 'list' argument. Returns size of one node in a list. */
static size_t _gds_forward_list_get_node_size(const GDSForwardList* list);

// ---------------------------------------------------------------------------------------------------------------------

/* Function assumes non-NULL 'node' argument. It returns the address of node's data. */
static void* _gds_forward_list_get_data_for_node(_GDSForwardListNodeBase* node);

// ------------------------------------------------------------------------------------------------------------------------------------------

gds_err gds_forward_list_init(GDSForwardList* list, size_t data_size, void (*_on_element_removal_func)(void*))
{
    if(list == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(data_size == 0) return GDS_GEN_ERR_INVALID_ARG(2);

    list->_count = 0;
    list->_head = NULL;
    list->_tail = NULL;
    list->_data_size = data_size;
    list->_on_element_removal_func = _on_element_removal_func;
    
    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

GDSForwardList* gds_forward_list_create(size_t data_size, void (*_on_element_removal_func)(void*))
{
    if(data_size == 0) return NULL;

    GDSForwardList* new_list = (GDSForwardList*)malloc(sizeof(GDSForwardList));
    if(new_list == NULL) return NULL;

    gds_err init_status = gds_forward_list_init(new_list, data_size, _on_element_removal_func);

    if(init_status != GDS_SUCCESS)
    {
        free(new_list);
        return NULL;
    }

    else return new_list;
}

// ---------------------------------------------------------------------------------------------------------------------

void gds_forward_list_destruct(GDSForwardList* list)
{
    if(list == NULL) return;

    gds_forward_list_empty(list);

    list->_head = NULL;
    list->_tail = NULL;
    list->_data_size = 0;
    list->_count = 0;
    list->_on_element_removal_func = NULL;
}

// ---------------------------------------------------------------------------------------------------------------------

void* gds_forward_list_at(const GDSForwardList* list, size_t pos)
{
    if(list == NULL) return NULL;
    if(pos >= list->_count) return NULL;

    _GDSForwardListNodeBase* node = _gds_forward_list_at_node(list, pos);

    return _gds_forward_list_get_data_for_node(node);
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_forward_list_assign(GDSForwardList* list, const void* data, size_t pos)
{
    if(list == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(data == NULL) return GDS_GEN_ERR_INVALID_ARG(2);
    if(pos >= list->_count) return GDS_GEN_ERR_INVALID_ARG(3);

    void* address = gds_forward_list_at(list, pos);
    memcpy(address, data, list->_data_size);

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_forward_list_swap(GDSForwardList* list, size_t pos1, size_t pos2, void* swap_buff)
{
    if(list == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(pos1 >= list->_count) return GDS_GEN_ERR_INVALID_ARG(2);
    if(pos2 >= list->_count) return GDS_GEN_ERR_INVALID_ARG(3);
    if(swap_buff == NULL) return GDS_GEN_ERR_INVALID_ARG(4);

    if(pos1 == pos2) return GDS_SUCCESS;

    _GDSForwardListNodeBase* data1 = gds_forward_list_at(list, pos1);
    _GDSForwardListNodeBase* data2 = gds_forward_list_at(list, pos2);

    size_t data_size = list->_data_size;

    gds_misc_swap(data1, data2, swap_buff, data_size);

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_forward_list_push_back(GDSForwardList* list, const void* data)
{
    if(list == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(data == NULL) return GDS_GEN_ERR_INVALID_ARG(2);

    _GDSForwardListNodeBase* new = _gds_forward_list_alloc_node(list, data);
    if(new == NULL) return GDS_FWDLIST_ERR_MALLOC_FAIL;

    if(list->_count == 0)
    {
        list->_head = new;
        list->_tail = new;
    }
    else
    {
        list->_tail->next = new;
        list->_tail = new;
    }

    list->_count++;

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_forward_list_push_front(GDSForwardList* list, const void* data)
{
    if(list == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(data == NULL) return GDS_GEN_ERR_INVALID_ARG(2);

    _GDSForwardListNodeBase* new = _gds_forward_list_alloc_node(list, data);
    if(new == NULL) return GDS_FWDLIST_ERR_MALLOC_FAIL;

    if(list->_count == 0)
    {
        list->_head = new;
        list->_tail = new;
    }
    else
    {
        new->next = list->_head;
        list->_head = new;
    }

    list->_count++;

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_forward_list_insert_at(GDSForwardList* list, const void* data, size_t pos)
{
    if(list == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(data == NULL) return GDS_GEN_ERR_INVALID_ARG(2);
    if(pos > list->_count) return GDS_GEN_ERR_INVALID_ARG(3);

    if(pos == list->_count) gds_forward_list_push_back(list, data);
    else if(pos == 0) gds_forward_list_push_front(list, data);
    else
    {
        _GDSForwardListNodeBase* new = _gds_forward_list_alloc_node(list, data);
        if(new == NULL) return GDS_FWDLIST_ERR_MALLOC_FAIL;

        _GDSForwardListNodeBase* previous = _gds_forward_list_at_node(list, pos - 1);

        new->next = previous->next;
        previous->next = new;
        list->_count++;
    }

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_forward_list_pop_front(GDSForwardList* list)
{
    if(list == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(list->_count == 0) return GDS_FWDLIST_ERR_LIST_EMPTY;

    _GDSForwardListNodeBase* next_head = list->_head->next;

    _gds_forward_list_on_node_removal(list, list->_head);

    list->_head = next_head;
    
    if(list->_count == 1) list->_tail = NULL;

    list->_count--;

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_forward_list_remove_at(GDSForwardList* list, size_t pos)
{
    if(list == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(pos >= list->_count) return GDS_GEN_ERR_INVALID_ARG(2);

    if(pos == 0) gds_forward_list_pop_front(list);
    else
    {
        _GDSForwardListNodeBase* prev = _gds_forward_list_at_node(list, pos - 1);
        _GDSForwardListNodeBase* next_next = prev->next->next;

        if(prev->next == list->_tail)
            list->_tail = prev;

        _gds_forward_list_on_node_removal(list, prev->next);
        prev->next = next_next;

        list->_count--;
    }

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_forward_list_empty(GDSForwardList* list)
{
    if(list == NULL) return GDS_GEN_ERR_INVALID_ARG(1);

    while(list->_count != 0) gds_forward_list_pop_front(list);

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

bool gds_forward_list_is_empty(const GDSForwardList* list)
{
    if(list == NULL) return true;

    return (list->_count == 0);
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_forward_list_get_count(const GDSForwardList* list)
{
    return (list != NULL) ? list->_count : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_forward_list_get_data_size(const GDSForwardList* list)
{
    return (list != NULL) ? list->_data_size : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_forward_list_get_struct_size()
{
    return sizeof(GDSForwardList);
}

// ------------------------------------------------------------------------------------------------------------------------------------------

/* Initializes the GDSForwardList iterator. The iterator will point at the first element of the list.
 * Return value:
 * on success: GDS_SUCCESS,
 * on failure: one of the generic error codes representing invalid arguments(if 'list' or 'iterator' is NULL.)
 * or GDS_FWDLIST_ERR_LIST_EMPTY. */
gds_err gds_forward_list_iterator_init(GDSForwardList* list, GDSForwardListIterator* iterator)
{
    if(list == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(iterator == NULL) return GDS_GEN_ERR_INVALID_ARG(2);

    if(list->_count == 0) return GDS_FWDLIST_ERR_LIST_EMPTY;

    iterator->_curr_node = list->_head;
    iterator->_pos = 0;

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

GDSForwardListIterator* gds_forward_list_iterator_create(GDSForwardList* list)
{
    if(list == NULL) return NULL;

    GDSForwardListIterator* iterator = (GDSForwardListIterator*)malloc(sizeof(GDSForwardListIterator));
    if(iterator == NULL) return NULL;

    gds_err init_status = gds_forward_list_iterator_init(list, iterator);
    
    if(init_status != GDS_SUCCESS)
    {
        free(iterator);
        return NULL;
    }
    else return iterator;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_forward_list_iterator_next(GDSForwardListIterator* iterator)
{
    if(iterator == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(iterator->_curr_node == NULL) return GDS_FAILURE;

    _GDSForwardListNodeBase* next_node = iterator->_curr_node->next;
    if(next_node == NULL) return GDS_FWDLIST_ITER_ERR_OUT_OF_BOUNDS;
    else
    {
        iterator->_curr_node = next_node;
        iterator->_pos += 1;
        return GDS_SUCCESS;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool gds_forward_list_iterator_has_next(GDSForwardListIterator* iterator)
{
    if(iterator == NULL) return false;

    return (iterator->_curr_node->next != NULL);
}

// ---------------------------------------------------------------------------------------------------------------------

void* gds_forward_list_iterator_get_data(GDSForwardListIterator* iterator)
{
    return (iterator != NULL) ? _gds_forward_list_get_data_for_node(iterator->_curr_node) : NULL;
}

// ------------------------------------------------------------------------------------------------------------------------------------------

/* Retrieves the position of the GDSForwardList node the iterator is pointing at. Function assumes non-NULL 'iterator' */
size_t gds_forward_list_iterator_get_pos(GDSForwardListIterator* iterator)
{
    return (iterator != NULL) ? iterator->_pos : 0;
}

// ------------------------------------------------------------------------------------------------------------------------------------------

static _GDSForwardListNodeBase* _gds_forward_list_alloc_node(const GDSForwardList* list, const void* data)
{
    assert(list != NULL);
    assert(data != NULL);

    _GDSForwardListNodeBase* new = (_GDSForwardListNodeBase*)malloc(_gds_forward_list_get_node_size(list));
    if(new == NULL) return NULL;

    void* node_data = _gds_forward_list_get_data_for_node(new);

    memcpy(node_data, data, list->_data_size);

    return new;
}

static _GDSForwardListNodeBase* _gds_forward_list_at_node(const GDSForwardList* list, size_t pos)
{
    assert(list != NULL);
    assert(pos < list->_count);

    if(pos == (list->_count - 1)) return list->_tail;
    else
    {
        _GDSForwardListNodeBase* it = list->_head;
        size_t i;
        for(i = 0; i < pos; i++) it = it->next;
        return it;
    }

}

static void _gds_forward_list_on_node_removal(const GDSForwardList* list, _GDSForwardListNodeBase* node)
{
    assert(list != NULL);
    assert(node != NULL);

    void* node_data = _gds_forward_list_get_data_for_node(node);
    if(list->_on_element_removal_func != NULL) list->_on_element_removal_func(node_data);

    free(node);
}

static size_t _gds_forward_list_get_node_size(const GDSForwardList* list)
{
    assert(list != NULL);

    return (sizeof(_GDSForwardListNodeBase) + list->_data_size);
}

static void* _gds_forward_list_get_data_for_node(_GDSForwardListNodeBase* node)
{
    assert(node != NULL);

    return (((void*)node) + sizeof(_GDSForwardListNodeBase*));
}
