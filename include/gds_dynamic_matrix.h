#ifndef GDS_DYNAMIC_MATRIX_H
#define GDS_DYNAMIC_MATRIX_H

#include <stddef.h>

struct GDSDynamicMatrix
{
    struct GDSVector* _rows; 
    struct GDSVector* (*_gen_row_func)(void);
    struct GDSVector* (*_gen_element_func)(void);
};

void gds_dyn_matrix_init(struct GDSDynamicMatrix* dyn_matrix,
        struct GDSVector* (*gen_row_func)(void),
        struct GDSVector* (*gen_element_func)(void),
        size_t row_min_count,
        size_t row_chunk_count);

void gds_dyn_matrix_set_size(struct GDSDynamicMatrix* dyn_matrix, size_t height, size_t width);

void* gds_dyn_matrix_at(struct GDSDynamicMatrix* dyn_matrix, size_t y, size_t x);

size_t gds_dyn_matrix_get_height(struct GDSDynamicMatrix* dyn_matrix);
size_t gds_dyn_matrix_get_width(struct GDSDynamicMatrix* dyn_matrix);

#endif
