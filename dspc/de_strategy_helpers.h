/*  Differential evolution solver heurisitc
    Lester E. Godwin
    Partially rewrote, plain ANSII C port,
    enhanced (EitherOr and Probabilist Parent Centric evolution strategies)
    plus maintained by Philippe Strauss, Spring 2012.  */


#ifndef _DIFFEV_STRATEGY_HELPERS_H
#define _DIFFEV_STRATEGY_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif


#include <dspc/de_rng_helpers.h>


/* used in de_strategy.c
   see p. 68 & 69 fig. 2.24
   also read p.61 § Stochastic Universal Sampling,
   a mean to spare CPU resources in case.
   -> p.65 §2.4.3, Table 2.4 */
INLINE void SelectSamples(de_t *st, int candidate, int *r1, int *r2, int *r3, int *r4, int *r5) {

    if (r1) {
        do {
            *r1 = rand_pop(st);
        /* ...different from the target vector index i... p.38, §2.1.3 */
        } while (*r1 == candidate);
    }

    if (r2) {
        do {
            *r2 = rand_pop(st);
        } while ((*r2 == candidate) || (*r2 == *r1));
    }

    if (r3) {
        do {
            *r3 = rand_pop(st);
        } while ((*r3 == candidate) || (*r3 == *r2) || (*r3 == *r1));
    }

    if (r4) {
        do {
            *r4 = rand_pop(st);
        } while ((*r4 == candidate) || (*r4 == *r3) || (*r4 == *r2) || (*r4 == *r1));
    }

    if (r5) {
        do {
            *r5 = rand_pop(st);
        } while ((*r5 == candidate) || (*r5 == *r4) || (*r5 == *r3) || (*r5 == *r2) || (*r5 == *r1));
    }

    return;
}


#ifdef RANDOMIZED_F_K

INLINE double fctF(de_t *st) {
    return (st->f + st->f_rnd_fact * normal_double(&(st->rng), 0.0, 1.0)
        - st->f_pop_fact * (double) st->nPop); 
}

INLINE double fctK(de_t *st) {
    return (st->k + st->k_rnd_fact * normal_double(&(st->rng), 0.0, 1.0)
        - st->f_pop_fact * (double) st->nPop); 
}

#endif /* RANDOMIZED_F_K */


/*  bounds checking, clipping. used in de_strategy.c
    for staying in a bounded solution domain  */
INLINE double clip(double trialn, int n, de_t *st) {
        if (trialn > st->min[n] && trialn < st->max[n])
            return trialn;
        else if (trialn > st->max[n])
            return st->max[n];
        else if (trialn < st->min[n])
            return st->min[n];
        /* oh dumb compiler */
        return 0.0;
}


/* most often used mutation strategy in DE */
INLINE void Mutate2(de_t *st, int i, double *r0, int r1idx, int r2idx) {

    double triali;

#ifndef RANDOMIZED_F_K
    triali = r0[i] + st->f * (
        Mat2Elt(st->nDim, st->population, r1idx, i) -
        Mat2Elt(st->nDim, st->population, r2idx, i)
    );
#else
    triali = r0[i] + fctF(st) * (
        Mat2Elt(st->nDim, st->population, r1idx, i) -
        Mat2Elt(st->nDim, st->population, r2idx, i)
    );
#endif
    st->trialSolution[i] = clip(triali, i, st); 
}

/* for the "2" named strategies, but their use is discouraged */
INLINE void Mutate4(de_t *st, int i, double *r0, int r1idx, int r2idx, int r3idx, int r4idx) {

    double triali;

#ifndef RANDOMIZED_F_K
    triali = r0[i] + st->f * (
        Mat2Elt(st->nDim, st->population, r1idx, i) +
        Mat2Elt(st->nDim, st->population, r2idx, i) -
        Mat2Elt(st->nDim, st->population, r3idx, i) -
        Mat2Elt(st->nDim, st->population, r4idx, i)
    );
#else
    triali = r0[i] + fctF(st) * (
        Mat2Elt(st->nDim, st->population, r1idx, i) +
        Mat2Elt(st->nDim, st->population, r2idx, i) -
        Mat2Elt(st->nDim, st->population, r3idx, i) -
        Mat2Elt(st->nDim, st->population, r4idx, i)
    );
#endif
    st->trialSolution[i] = clip(triali, i, st);
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
