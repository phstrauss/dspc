/*  mem.h
    Memory allocation helpers for 2D matrix
    (c) Philippe Strauss, spring 2010  */

#include <stdio.h>
#include <stdlib.h>
//
#include <dspc/dumb_alloc2d.h>


// allocate a vector of pointers to vectors, a 2D matrix.
bool alloc_matrix(mat *ret, int m, int n) {
    float *tmp1;
    mat tmp2;
    int i;
    tmp2 = (float **) calloc(m, sizeof(float *));
    if (tmp2 == NULL)
        return false;
    for (i = 0; i < m; ++i) {
        tmp1 = (float *) calloc(n, sizeof(float));  // should free if we fail here
        if (tmp1 == NULL) {
            while (--i >= 0)
                free(tmp2[i]);
            free(tmp2);
            return false;
        } else {
            tmp2[i] = tmp1;
        }
    }
    *ret = tmp2;
    return true;
}

void free_matrix(mat arr, int m) {
    int i;
    for (i = 0; i < m; ++i) {
        free(arr[i]);
    }
    free(arr);
}
