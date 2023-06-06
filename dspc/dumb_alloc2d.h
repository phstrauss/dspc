/*  mem.h
    Memory allocation helpers for 2D matrix
    Philippe Strauss, spring 2010 */

#ifndef _SPS_MEM_H
#define _SPS_MEM_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdbool.h>


    typedef float **mat;


// mem.c
    bool alloc_matrix(mat *, int, int);
    void free_matrix(mat, int);


#ifdef __cplusplus
}                               /* extern "C" */
#endif
#endif                          /* _MEM_H */
