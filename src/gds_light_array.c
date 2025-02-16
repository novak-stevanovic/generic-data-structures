#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "gds.h"
#include "gds_misc.h"
#include "gds_light_array.h"

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
#define __GDS_LIGHT_ARRAY_DEF_ALLOW__
#include "def/gds_light_array_def.h"
#endif // GDS_ENABLE_OPAQUE_STRUCTS

// ------------------------------------------------------------------------------------------------------------------------------------------

/* Shift elements right of index(including) 'start_idx' rightward by calling memmove().
 * This function cannot expand the array if there is not enough memory allocated. 
 * Make sure that: array->count < array->alloced_count before calling. 
 * This function assumes that it will not receive a NULL pointer as 'array' argument, and that 'start_idx' < array's count. */
static void _gds_light_array_shift_right(GDSLightArray* array, size_t start_idx);

// ---------------------------------------------------------------------------------------------------------------------

/* Shift elements right of(including) index 'start_idx' leftward by calling memmove(). 
 * This function assumes that it will not receive a NULL pointer as 'array' argument, and that 'start_idx' < array's count. */
static void _gds_light_array_shift_left(GDSLightArray* array, size_t start_idx);

// ------------------------------------------------------------------------------------------------------------------------------------------

gds_err gds_light_array_init(GDSLightArray* array, size_t capacity, size_t element_size)
{
#ifdef GDS_INIT_MAX_SIZE
    if(element_size > GDS_INIT_MAX_SIZE) return GDS_ERR_MAX_INIT_SIZE_EXCEED;
#endif // GDS_INIT_MAX_SIZE

    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(capacity == 0) return GDS_GEN_ERR_INVALID_ARG(2);
    if(element_size == 0) return GDS_GEN_ERR_INVALID_ARG(3);

    array->_capacity = capacity;
    array->_element_size = element_size;
    array->_count = 0;

    array->_data = malloc(capacity * element_size);

    if(array->_data == NULL) return GDS_ARR_ERR_MALLOC_FAIL;

#ifdef GDS_TEMP_BUFF_USE_SWAP_BUFF

    array->_swap_buff = malloc(element_size);
    if(array->_swap_buff == NULL)
    {
        free(array->_data);
        return GDS_ARR_ERR_MALLOC_FAIL;
    }

#endif // GDS_TEMP_BUFF_USE_SWAP_BUFF

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

GDSLightArray* gds_light_array_create(size_t capacity, size_t element_size)
{
    if((capacity == 0) || (element_size == 0)) return NULL;

    GDSLightArray* array = (GDSLightArray*)malloc(sizeof(GDSLightArray));
    if(array == NULL) return NULL;

    gds_err init_status = gds_light_array_init(array, capacity, element_size);

    if(init_status == GDS_SUCCESS) return array;
    else
    {
        free(array);
        return NULL;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void gds_light_array_destruct(GDSLightArray* array)
{
    if(array == NULL) return;

    gds_light_array_empty(array);
    free(array->_data);

    array->_count = 0;
    array->_capacity = 0;
    array->_element_size = 0;

#ifdef GDS_TEMP_BUFF_USE_SWAP_BUFF

    free(array->_swap_buff);
    array->_swap_buff = NULL;

#endif // GDS_TEMP_BUFF_USE_SWAP_BUFF
}

// ---------------------------------------------------------------------------------------------------------------------

void* gds_light_array_at(const GDSLightArray* array, size_t pos)
{
    if(array == NULL) return NULL;
    if(pos >= array->_count) return NULL;

    return array->_data + pos * array->_element_size;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_array_assign(GDSLightArray* array, const void* data, size_t pos)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(data == NULL) return GDS_GEN_ERR_INVALID_ARG(2);
    if(pos >= array->_count) return GDS_GEN_ERR_INVALID_ARG(3);

    void* addr = gds_light_array_at(array, pos);

    memcpy(addr, data, array->_element_size);

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_array_swap(GDSLightArray* array, size_t pos1, size_t pos2)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(pos1 >= array->_count) return GDS_GEN_ERR_INVALID_ARG(2);
    if(pos2 >= array->_count) return GDS_GEN_ERR_INVALID_ARG(3);

    if(pos1 == pos2) return GDS_SUCCESS;

    void* pos1_data = gds_light_array_at(array, pos1);
    void* pos2_data = gds_light_array_at(array, pos2);

    size_t data_size = array->_element_size;

#ifdef GDS_TEMP_BUFF_USE_SWAP_BUFF

    void* swap_buff = array->_swap_buff;

#endif // GDS_TEMP_BUFF_USE_SWAP_BUFF
#ifdef GDS_TEMP_BUFF_USE_VLA

    char swap_buff[data_size];

#endif // GDS_TEMP_BUFF_USE_VLA
#ifdef GDS_TEMP_BUFF_USE_ALLOCA

    void* swap_buff = alloca(data_size);

#endif // GDS_TEMP_BUFF_USE_ALLOCA

    memcpy(swap_buff, pos1_data, data_size);
    memcpy(pos1_data, pos2_data, data_size);
    memcpy(pos2_data, swap_buff, data_size);

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_array_push_back(GDSLightArray* array, const void* data)
{
    return gds_light_array_insert_at(array, data, array->_count);
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_array_insert_at(GDSLightArray* array, const void* data, size_t pos)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(data == NULL) return GDS_GEN_ERR_INVALID_ARG(2);
    if(pos > array->_count) return GDS_GEN_ERR_INVALID_ARG(3);
    if(array->_count == array->_capacity) return GDS_ARR_ERR_INSUFF_CAPACITY;

    if(pos < array->_count) _gds_light_array_shift_right(array, pos);

    array->_count++; // it is important to increase the count first, so that the gsd_array_assign() function will work.
    gds_light_array_assign(array, data, pos);

    return GDS_SUCCESS;

}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_array_pop_back(GDSLightArray* array)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(array->_count == 0) return GDS_ARR_ERR_ARR_EMPTY;

     gds_light_array_remove_at(array, array->_count - 1);

     return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_array_remove_at(GDSLightArray* array, size_t pos)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(pos >= array->_count) return GDS_GEN_ERR_INVALID_ARG(2);

    if(pos < (array->_count - 1)) _gds_light_array_shift_left(array, pos + 1);

    array->_count--;

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_array_empty(GDSLightArray* array)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);

    while(array->_count != 0) gds_light_array_pop_back(array);

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

gds_err gds_light_array_realloc(GDSLightArray* array, size_t new_capacity)
{
    if(array == NULL) return GDS_GEN_ERR_INVALID_ARG(1);
    if(new_capacity == 0) return GDS_GEN_ERR_INVALID_ARG(2);

    if(new_capacity == array->_capacity) return GDS_SUCCESS;

    while(new_capacity < array->_count) gds_light_array_pop_back(array); // shrink the array.

    void* realloc_status = realloc(array->_data, new_capacity * array->_element_size);

    if(realloc_status == NULL) return GDS_ARR_ERR_REALLOC_FAIL;
    else array->_data = realloc_status;

    array->_capacity = new_capacity;
    array->_count = gds_misc_min(array->_count, array->_capacity);

    return GDS_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_light_array_get_count(const GDSLightArray* array)
{
    return (array != NULL) ? array->_count : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

bool gds_light_array_is_empty(const GDSLightArray* array)
{
    return (array != NULL) ? (array->_count == 0) : true;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_light_array_get_capacity(const GDSLightArray* array)
{
    return (array != NULL) ? array->_capacity : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_light_array_get_element_size(const GDSLightArray* array)
{
    return (array != NULL) ? array->_element_size : 0;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t gds_light_array_get_struct_size()
{
    return sizeof(GDSLightArray);
}

// ------------------------------------------------------------------------------------------------------------------------------------------

static void _gds_light_array_shift_right(GDSLightArray* array, size_t start_idx)
{
    size_t array_count = array->_count;

    size_t step = array->_element_size;
    size_t elements_shifted = array_count - start_idx;

    void* start_pos = gds_light_array_at(array, start_idx);

    memmove(start_pos + step, start_pos, step * elements_shifted);
}

static void _gds_light_array_shift_left(GDSLightArray* array, size_t start_idx)
{
    size_t array_count = array->_count;

    size_t step = array->_element_size;
    size_t elements_shifted = array_count - start_idx;

    void* start_pos = gds_light_array_at(array, start_idx) - step;

    memmove(start_pos, start_pos + step, step * elements_shifted);
}


