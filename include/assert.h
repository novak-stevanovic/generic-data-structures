#ifndef _ASSERT_H
#define _ASSERT_H
#include <stdio.h>

#define ASSERT(condition, err_msg) if(!(condition)) { \
    printf("%s", err_msg); \
    exit(1); } \

#define ASSERT_NON_NULL_ARG(arg_val, arg_name) if(arg_val == NULL) { \
    printf("Argument " #arg_name " must be non-null.\n"); \
    exit(1); } \

#define ASSERT_NON_NULL_VAL(val, val_name) if(val == NULL) { \
    printf("Argument " #val_name " must be non-null.\n"); \
    exit(1); } \

#endif
