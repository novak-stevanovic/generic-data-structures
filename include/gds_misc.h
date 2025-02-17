#ifndef _GDS_MISC_H_
#define _GDS_MISC_H_

#include "gds.h"
#include <stdio.h>

int gds_misc_max(ssize_t x, ssize_t y);
int gds_misc_min(ssize_t x, ssize_t y);
void gds_misc_swap(void* data1, void* data2, void* swap_buff, size_t data_size);

// ---------------------------------------------------------------------------------------------------------------------

#ifdef GDS_TEMP_BUFF_USE_SWAP_BUFF
#define __GDS_GET_SWAP_BUFF(struct_name,data_size) void* swap_buff = struct_name->__GDS_SWAP_BUFF_NAME;
#endif // GDS_TEMP_BUFF_USE_SWAP_BUFF

#ifdef GDS_TEMP_BUFF_USE_VLA
#define __GDS_GET_SWAP_BUFF(struct_name,data_size) char swap_buff[data_size];
#endif // GDS_TEMP_BUFF_USE_VLA

#ifdef GDS_TEMP_BUFF_USE_ALLOCA
#define __GDS_GET_SWAP_BUFF(struct_name,data_size) void* swap_buff = alloca(data_size);
#endif // GDS_TEMP_BUFF_USE_ALLOCA

// ---------------------------------------------------------------------------------------------------------------------

#endif
