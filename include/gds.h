#ifndef GDS_H
#define GDS_H

// TODO add restrict to func params and inline to func signatures
// TODO check error codes for everything
// TODO bring back GDS and GDSE naming
// TODO test light vector and other stuff

// ------------------------------------------------------------------------------------------------------------------------------------

/* GDS(Generic Data Structures) is a C library designed to aid C developers in their development process by
providing them with data structures they can use throughout their projects. Besides efficiency, the focus
for most of these data structures is on user-control, allowing the programmer using this 
library to do what he wants, excatly how he wants it. */

// Error Handling ----------------------------------------------------------------------------------------------------------

/* Some functions return error codes. There are a number of generic error codes defined below.
1. GDS_GEN_ERR_INCONSISTENT_ARGS - this is returned when there is a mismatch in multiple arguments sent 
to a function. For example, this value may be returned from a function that accepts parameters start_pos
and end_pos, where end_pos must be greater than start_pos.

2. GDS_GEN_ERR_INVALID_ARG - refers to the case where a call to a function has been performed,
while providing an invalid argument. Why the argument may be invalid, depends on the function in 
question - perhaps it was a NULL pointer, perhaps it was an invalid number. The function will return
a value, x being the 'index' of the invalid argument(starting from 1). 

3. GDS_GEN_ERR_INERNAL_ERR - refers to the case where a function in the API relies on a call to an internal
function which failed. In this case, the library is at fault. The API doesn't mention this error because it
is a valid return value for functions that return gds_err by default. */

typedef int gds_err;

#define GDS_SUCCESS 0
#define GDS_FAILURE -1

#define _GDS_GEN_ERR_BASE 1000

#define _GDS_GEN_ERR_INVALID_ARG_BASE (_GDS_GEN_ERR_BASE + 10)
#define GDS_GEN_ERR_INCONSISTENT_ARGS (_GDS_GEN_ERR_INVALID_ARG_BASE + 0)
#define GDS_GEN_ERR_INVALID_ARG(x) (_GDS_GEN_ERR_INVALID_ARG_BASE + x)
#define GDS_GEN_ERR_INTERNAL_ERR 1002

// Struct opaqueness ------------------------------------------------------------------------------------------------------

/* Each data structure is nicely encapsulated by making the struct itself opaque. As such, the definitions are 
found in the corresponding .c files. However, if the user wishes for more control, he may want to make the 
structs not opaque. This can be done by commenting the macro below (GDS_ENABLE_OPAQUE_STRUCTS). */

#define GDS_ENABLE_OPAQUE_STRUCTS

// Stack allocation and temporary memory buffers --------------------------------------------------------------------------

/* Many data structures have built-in supported functions that require allocating a tempoprary memory buffer with the
 * size equal to the data size stored by that data structure. The prime example of this are the swap functions for
 * each data structure. However, it's impossible to know the data size at compile time.
 * This leaves a few ways for the library to handle this situation:
 * 1. Each data structure that requires such a buffer will, when initializing, dynamically allocate the buffer and store it.
 * It will then use this buffer for operations that require it. This results in a slightly bigger memory footprint.
 * 2. Variable-length arrays(VLA's). Operations that require such a buffer will initialize a VLA that is big enough
 * to accomodate the data size.
 * 3. The usage of alloca(). Operations that require such a buffer will use the alloca() function as the temporary buffer. 
 *
 * If the user wishes for portability, he should choose the first option. If he wishes for less memory footprint, choose
 * the 2. or 3. options. Keep in mind that 2. and 3. options are theoretically unsafe because stack allocations may fail. This
 * means that the user must be careful when passing data/element_size arguments to DS init functions. To change this behavior,
 * define **one** of the macros below and comment out the others. */

#define GDS_TEMP_BUFF_USE_SWAP_BUFF // 1. option
// #define GDS_TEMP_BUFF_USE_VLA // 2. option
// #define GDS_TEMP_BUFF_USE_ALLOCA // 3. option

#define __GDS_SWAP_BUFF_NAME __swap_buff

/* As an extra precaution, when using VLA's or alloca(), you may want to define the macro below(and adjust its definition).
 * Whenever an init function is called for any data structure, a check will be performed to see if the passed data_size
 * argument doesn't exceed the value defined. If it isn't a special error code will be returned. */

#define GDS_INIT_MAX_SIZE 200

#define GDS_ERR_MAX_INIT_SIZE_EXCEED 1001

// ------------------------------------------------------------------------------------------------------------------------------------

#endif // GDS_H
