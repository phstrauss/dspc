/*  Differential evolution solver heurisitc
    Lester E. Godwin
    Partially rewrote, plain ANSII C port,
    enhanced (EitherOr and Probabilist Parent Centric evolution strategies)
    plus maintained by Philippe Strauss, Spring 2012.  */


#ifndef _DIFFEV_RNG_HELPERS_H
#define _DIFFEV_RNG_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif


#include <dspc/gnuinline.h>


/* 2 often used random helpers */

INLINE int rand_pop(de_t *st) {
    /* p. 61 § 2.4.1 fig. 2.18 : floor, ...strictly less than 1. */
    int ret;
    ret = (int) bounded_double(&(st->rng), 0.0, (double) st->nPop);
    if (ret == st->nPop) /* very low prob. but not impossible */
        return ret-1;
    else
        return ret;
}

INLINE int rand_dim(de_t *st) {
    int ret;
    ret = (int) bounded_double(&(st->rng), 0.0, (double) st->nDim);
    if (ret == st->nDim) /* very low prob. but not impossible */
        return ret-1;
    else
        return ret;
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
