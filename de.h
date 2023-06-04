/*  Differential evolution solver heurisitc
    Lester E. Godwin
    Partially rewrote, plain ANSII C port,
    enhanced (EitherOr and Probabilist Parent Centric evolution strategies)
    plus maintained by Philippe Strauss, Spring 2012.

    Reference :

        "Differential Evolution, A Practical Approach to Global Optimization"
        Kenneth V. Price, Rainer M. Storn, Jouni A. Lampinen
        Springer (Natural Computing Series)
        ISBN: 978-3-540-20950-8  */


#ifndef _DIFFEV_H
#define _DIFFEV_H

#ifdef __cplusplus
extern "C" {
#endif


#include <string.h>
#include <stdbool.h>
#include <dspc/rng.h>

// vector/matrix helpers
#define Mat2Elt(nDim, a, b, c)      a[b * nDim + c]
#define Mat2Row(nDim, a, b)         (&a[b * nDim])
#define VecCopy(nDim, a, b)         memcpy((a), (b), nDim * sizeof(double))

typedef struct {
    int nDim;
    int nPop;

    /*  p38. §2.1.3 eq. 2.5 ; p. 102 § Rotational invariance
        see p. 74 on top, about randomizing F., and p.79 §2.5.2
        also p. 77 eq. 2.31  */
    double f;
    double k; /* for either-or */
    double probability; /* Cr or either-or probability */
    double probabilityPCX;

    double trialEnergy;
    double bestEnergy;
    int comparison;

    double *min;
    double *max;

    double *trialSolution;
    double *bestSolution;
    double *popEnergy;
    double *population;

    struct rng_s rng;

    /* parent centric cross-over/mutation */
    bool pcx;
    double *pcx_g; /* PCX mean vector g */
    double *pcx_dp; /* PCX directional vector dp */
    /* pcx_po_* : PCX vector from not best vector to best (for mean perp. distance) */
    double *pcx_po_cand;
    double *pcx_po_r1;
    /* normal random squared sigmas */
    double pcx_sigma2a;
    double pcx_sigma2b;
    /* dmean - for simple reporting */
    double pcx_dmean;

#ifdef RANDOMIZED_F_K

    /* randomized F */
    bool fk_randomized;
    /* factor in front of gaussian random process N(0,1) mu = 0, sigma2 = 1 */
    double f_rnd_fact;
    /* factor multiplying the (double casted) number of elt. in the population */
    double f_pop_fact;

    /* randomized K */
    double k_rnd_fact;
    double k_pop_fact;

#endif /* RANDOMIZED_F_K */

} de_t;

/* trial solution computation fct (handling crossover/mutation)
   may be ProbParentCentric

   void (*fctTrialSolution)(de_t *, int);
   void (*fctProbPCX_StdTrial)(de_t *, int); */
typedef void (*cb_trial_t)(de_t *, int);

/* double (*fctEnergyFunction)(int, double, double *, bool *); */
typedef double (*cb_energy_t)(int, double, double *, bool *);

// de_strategy.c
void Best1Exp(de_t *, int);
void Best1Bin(de_t *, int);
void Rand1Exp(de_t *, int);
void Rand1Bin(de_t *, int);
void EitherOr(de_t *, int);
void ProbParentCentric(de_t *, int);
// read p.73 before using, discouraged
void Best2Exp(de_t *, int);
void Best2Bin(de_t *, int);
void Rand2Exp(de_t *, int);
void Rand2Bin(de_t *, int);
// end de_strategy.c

#define DE_MINIMIZE 0
#define DE_MINIMIZE_MAGNITUDE 1
#define DE_MAXIMIZE 2


// de_solve.c public

void de_Init_Uniform(de_t *st, int dim, int popSize, double *min, double *max, int comparison);

void de_Init_Solution(de_t *st, int dim, int popSize, double *min, double *max, int comparison,
        double *aprioriSolution, double *popGaussianSigmas);

void de_Finalize(de_t *);

void de_SetupStd(de_t *st, double diffScale, double crossoverProb,
        void (*TrialSolution)(de_t *, int),
        double (*EnergyFunction)(int, double, double *, bool *));

void de_SetupRandomFK(de_t *st, double f_rand_fact, double f_pop_fact,
        double k_rand_fact, double k_pop_fact);

void de_SetupEitherOr(de_t *st, double f, double k, double crossoverProb,
        double (*EnergyFunction)(int, double, double *, bool *));

void de_SetupPCX(de_t *st, double pcxProb, double diffScale, double crossoverProb,
        void (*StdTrialSolution)(de_t *, int),
        double (*EnergyFunction)(int, double, double *, bool *),
        double sigma2a, double sigma2b);

bool de_Solve(de_t *st, int maxGenerations);

// end de_solve.c


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif