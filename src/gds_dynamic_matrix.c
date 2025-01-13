#include <assert.h>

#include "gds_dynamic_matrix.h"
#include "gds_vector.h"

void gds_dyn_matrix_init(struct GDSDynamicMatrix* dyn_matrix,
        struct GDSVector* (*gen_row_func)(void),
        struct GDSVector* (*gen_element_func)(void),
        size_t row_min_count,
        size_t row_chunk_count)
{
    assert(dyn_matrix != NULL);

    dyn_matrix->_gen_row_func = gen_row_func;
    dyn_matrix->_gen_element_func = gen_element_func;

    dyn_matrix->_rows = gds_vec_create(row_min_count, row_chunk_count, gds_vec_get_struct_size());
}

void gds_dyn_matrix_set_size(struct GDSDynamicMatrix* dyn_matrix, size_t height, size_t width)
{
    assert(dyn_matrix != NULL);
    assert(dyn_matrix->_rows != NULL);

    while(gds_dyn_matrix_get_height(dyn_matrix) < height)
        gds_vec_append(dyn_matrix->_rows, dyn_matrix->_gen_row_func());

    while(gds_dyn_matrix_get_height(dyn_matrix) > height)
        gds_vec_pop(dyn_matrix->_rows);
    
    int i;
    struct GDSVector* curr_row;
    for(i = 0; i < gds_vec_get_count(dyn_matrix->_rows); i++)
    {
        curr_row = gds_vec_at(dyn_matrix->_rows, i);
        gds_vec_append(curr_row, dyn_matrix->_gen_element_func());

        while(gds_vec_get_count(curr_row) < height)
            gds_vec_append(curr_row, dyn_matrix->_gen_element_func());

        while(gds_vec_get_count(curr_row) > height)
            gds_vec_pop(curr_row);
    }
}

void* gds_dyn_matrix_at(struct GDSDynamicMatrix* dyn_matrix, size_t y, size_t x)
{
    assert(dyn_matrix != NULL);
    assert(dyn_matrix->_rows != NULL);

    struct GDSVector* row = gds_vec_at(dyn_matrix->_rows, y);

    assert(row != NULL);

    return gds_vec_at(row, x);
}

size_t gds_dyn_matrix_get_height(struct GDSDynamicMatrix* dyn_matrix)
{
    assert(dyn_matrix->_rows != NULL);
    return gds_vec_get_count(dyn_matrix->_rows);
}

size_t gds_dyn_matrix_get_width(struct GDSDynamicMatrix* dyn_matrix)
{
    assert(dyn_matrix != NULL);
    assert(dyn_matrix->_rows != NULL);
    assert(gds_vec_get_count(dyn_matrix->_rows) > 0);

    struct GDSVector* row = gds_vec_at(dyn_matrix->_rows, 0);
    return gds_vec_get_count(row);
}
