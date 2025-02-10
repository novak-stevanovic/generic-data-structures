#ifndef _GDS_FORWARD_LIST_H_
#define _GDS_FORWARD_LIST_H_

#include <stdlib.h>
#include <stdbool.h>

#include "gds.h"

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
struct GDSForwardList;
#else
#define __GDS_FORWARD_LIST_DEF_ALLOW__
#include "def/gds_forward_list_def.h"
#endif

typedef struct GDSForwardList GDSForwardList;

// ------------------------------------------------------------------------------------------------------------------------------------------

#define GDS_FWDLIST_ERR_BASE 2100
#define GDS_FWDLIST_ERR_FLIST_EMPTY 2101
#define GDS_FWDLIST_ERR_MALLOC_FAIL 2102

// ------------------------------------------------------------------------------------------------------------------------------------------

#ifdef GDS_DISABLE_OPAQUE_STRUCTS
gds_err gds_forward_list_init(GDSForwardList* list, size_t data_size, void (*_on_element_removal_func)(void*));
#endif

// ---------------------------------------------------------------------------------------------------------------------

GDSForwardList* gds_forward_list_create(size_t data_size, void (*_on_element_removal_func)(void*));

// ---------------------------------------------------------------------------------------------------------------------

void gds_forward_list_destruct(GDSForwardList* list);

// ---------------------------------------------------------------------------------------------------------------------

void* gds_forward_list_at(const GDSForwardList* list, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_forward_list_assign(const GDSForwardList* list, const void* data, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_forward_list_swap(const GDSForwardList* list, size_t pos1, size_t pos2);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_forward_list_push_back(GDSForwardList* list, const void* data);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_forward_list_push_front(GDSForwardList* list, const void* data);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_forward_list_insert_at(GDSForwardList* list, const void* data, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_forward_list_pop_front(GDSForwardList* list);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_forward_list_remove_at(GDSForwardList* list, size_t pos);

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_forward_list_empty(GDSForwardList* list);

// ---------------------------------------------------------------------------------------------------------------------

ssize_t gds_forward_list_get_count(const GDSForwardList* list);

// ---------------------------------------------------------------------------------------------------------------------

/* Checks if the forward list is empty. 
 * This function doesn't return an error if list == NULL, so the caller must make sure that 'list' argument is non-NULL.
 * In the case that the user sends a NULL pointer, function returns true. */
bool gds_forward_list_is_empty(const GDSForwardList* list);

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_forward_list_get_struct_size();

// ------------------------------------------------------------------------------------------------------------------------------------------

#endif
