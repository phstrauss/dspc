/*  Differential evolution solver heurisitc
    Lester E. Godwin
    Partially rewrote, plain ANSII C port,
    enhanced (EitherOr and Probabilist Parent Centric evolution strategies)
    plus maintained by Philippe Strauss, Spring 2012.  */


#include <stdio.h>
#include <stdlib.h>
#include <dspc/de.h>
#include <dspc/perr.h>


/* G L O B A L S */

/* primary trial solution computation fct (handling crossover/mutation)
   may be ProbParentCentric

   void (*fctTrialSolution)(de_t *, int); */
cb_trial_t fctTrialSolution;

/* secondary trial solution computation fct in case of ProbParentCentric */
cb_trial_t fctProbPCX_StdTrial;

/* double (*fctEnergyFunction)(int, double, double *, bool *); */
cb_energy_t fctEnergyFunction;

/* E N D  G L O B A L S */


// private
static void de_Init_Alloc(de_t *st, int dim, int popSize) {

    st->nDim            = dim;
    st->nPop            = popSize;
    st->f               = 0.7;
    st->k               = 0.5;
    st->probability     = 0.5;
    st->probabilityPCX  = 0.02;
    st->bestEnergy      = 0.0;

    st->min             = (double *) malloc(sizeof(double) * st->nDim);
    st->max             = (double *) malloc(sizeof(double) * st->nDim);
    st->trialSolution   = (double *) malloc(sizeof(double) * st->nDim);
    st->bestSolution    = (double *) malloc(sizeof(double) * st->nDim);
    st->popEnergy       = (double *) malloc(sizeof(double) * st->nPop);
    st->population      = (double *) malloc(sizeof(double) * st->nPop * st->nDim);
    /* parent centric xover/mutation */
    st->pcx_g           = (double *) malloc(sizeof(double) * st->nDim);
    st->pcx_dp          = (double *) malloc(sizeof(double) * st->nDim);
    st->pcx_po_cand     = (double *) malloc(sizeof(double) * st->nDim);
    st->pcx_po_r1       = (double *) malloc(sizeof(double) * st->nDim);
    st->pcx_sigma2a     = 0.7;
    st->pcx_sigma2b     = 0.3;

#ifdef RANDOMIZED_F_K
    st->fk_randomized = false; /* maybe usefull later */
    st->f_rnd_fact = 0.001;
    st->f_pop_fact = 0.0;
    st->k_rnd_fact = 0.001;
    st->k_pop_fact = 0.0;
#endif

    init_rng_from_devrandom(&(st->rng));

    return;
}

// private
static void de_Init_Energy(de_t *st, int npop) {
    switch (st->comparison) {
    case DE_MINIMIZE:
        st->popEnergy[npop] = 1E308;
        break;
    case DE_MINIMIZE_MAGNITUDE:
        st->popEnergy[npop] = 1E308;
        break;
    case DE_MAXIMIZE:
        st->popEnergy[npop] = -1E308;
        break;
    }
}

// private
static void de_Init_Population_Uniform(de_t *st, double *min, double *max) {
    int i, j;

    /* initial population p. 38 §2.1.2, p. 56 end of § 2.3.1 and § 2.3.2 */ 
    for (i=0; i < st->nPop; i++) {
        for (j=0; j < st->nDim; j++)
            Mat2Elt(st->nDim, st->population, i, j) = bounded_double(&(st->rng), min[j], max[j]);

        de_Init_Energy(st, i);

    }
}

void de_Init_Uniform(de_t *st, int dim, int popSize, double *min, double *max, int comparison) {
    int i;

    de_Init_Alloc(st, dim, popSize);

    st->comparison      = comparison;

    /* copy bounds of the solution domain */
    VecCopy(st->nDim, st->min, min);
    VecCopy(st->nDim, st->max, max);

    /*  initial best solution : uniform random  */
    for (i=0; i < st->nDim; i++)
        st->bestSolution[i] = bounded_double(&(st->rng), min[i], max[i]);
 
    de_Init_Population_Uniform(st, min, max);

    return;
}

// we may also have the needs of setting some dimensions with a rel. good apriori, and let others
// beeing totaly solved by diffev
void de_Init_Solution(de_t *st, int dim, int popSize, double *min, double *max, int comparison,
    double *aprioriSolution, double *popGaussianSigmas /* , bool *aprioris */) {
    int i, j;

    de_Init_Alloc(st, dim, popSize);

    st->comparison      = comparison;

    /* copy bounds of the solution domain */
    VecCopy(st->nDim, st->min, min);
    VecCopy(st->nDim, st->max, max);

    VecCopy(st->nDim, st->bestSolution, aprioriSolution);

    /* initial population parent centric from apriori solution
       gaussian mean set on the a priori value, gaussian sigmas left to the user */ 
    for (i=0; i < st->nPop; i++) {
        for (j=0; j < st->nDim; j++)
            Mat2Elt(st->nDim, st->population, i, j) =
                normal_double(&(st->rng), st->bestSolution[i], popGaussianSigmas[i]);

        de_Init_Energy(st, i);
    }

    return;
}


void de_Finalize(de_t *st) {

    free(st->trialSolution);
    free(st->bestSolution);
    free(st->popEnergy);
    free(st->population);
    free(st->pcx_g);
    free(st->pcx_dp);
    free(st->pcx_po_cand);
    free(st->pcx_po_r1);

    return;
}

void de_SetupStd(de_t *st, double diffScale, double crossoverProb,
        cb_trial_t cb_trial,
        cb_energy_t cb_energy) {

    st->pcx = false;
    st->f = diffScale;
    st->probability = crossoverProb;
    fctTrialSolution = cb_trial;
    fctEnergyFunction = cb_energy;

}


#ifdef RANDOMIZED_F_K

void de_SetupRandomFK(de_t *st, double f_rnd_fact, double f_pop_fact, double k_rnd_fact, double k_pop_fact) {
    st->fk_randomized = true;
    st->f_rnd_fact = f_rnd_fact;
    st->f_pop_fact = f_pop_fact;
    st->k_rnd_fact = k_rnd_fact;
    st->k_pop_fact = k_pop_fact;
}

#endif /* RANDOMIZED_F_K */


void de_SetupEitherOr(de_t *st, double f, double k, double crossoverProb,
        cb_energy_t cb_energy) {

    st->pcx = false;
    st->f = f;
    st->k = k;
    st->probability = crossoverProb;
    fctTrialSolution = EitherOr;
    fctEnergyFunction = cb_energy;

}

void de_SetupPCX(de_t *st, double pcxProb, double diffScale, double crossoverProb,
        cb_trial_t cb_trial,
        cb_energy_t cb_energy,
        double sigma2a, double sigma2b) {

    st->pcx = true;
    st->f = diffScale;
    st->probabilityPCX = pcxProb;
    st->probability = crossoverProb;
    st->pcx_sigma2a = sigma2a;
    st->pcx_sigma2b = sigma2b;
    /* PST 2023-04-03 FIXME */
    fctTrialSolution = ProbParentCentric;
    fctProbPCX_StdTrial = cb_trial;
    fctEnergyFunction = cb_energy;

}

bool de_Solve(de_t *st, int maxGenerations) {
    int generation;
    int candidate;
    bool bAtSolution;

    st->bestEnergy = 1.0E308;
    bAtSolution = false;

    for (generation = 0; (generation < maxGenerations) && !bAtSolution; generation++) {
        for (candidate = 0; candidate < st->nPop; candidate++) { // FIX: PCX
            //dprintf("de_Solve: candidate=%d\n", candidate);

            (*fctTrialSolution)(st, candidate);

            st->trialEnergy = fctEnergyFunction(st->nDim, st->bestEnergy, st->trialSolution, &bAtSolution);

            switch (st->comparison) {
            case DE_MINIMIZE:
                if (st->trialEnergy < st->popEnergy[candidate]) {
                    // New low for this candidate
                    st->popEnergy[candidate] = st->trialEnergy;
                    VecCopy(st->nDim, Mat2Row(st->nDim, st->population, candidate), st->trialSolution);

                    // Check if all-time low
                    if (st->trialEnergy < st->bestEnergy) {
                        st->bestEnergy = st->trialEnergy;
                        VecCopy(st->nDim, st->bestSolution, st->trialSolution);
                    }
                }
                break;

            case DE_MAXIMIZE:
                if (st->trialEnergy > st->popEnergy[candidate]) {
                    // New high for this candidate
                    st->popEnergy[candidate] = st->trialEnergy;
                    VecCopy(st->nDim, Mat2Row(st->nDim, st->population, candidate), st->trialSolution);

                    // Check if all-time high
                    if (st->trialEnergy > st->bestEnergy) {
                        st->bestEnergy = st->trialEnergy;
                        VecCopy(st->nDim, st->bestSolution, st->trialSolution);
                    }
                }
                break;

            case DE_MINIMIZE_MAGNITUDE:
                /* FIX: memoize fabs(st->trialEnergy) ? */
                if (fabs(st->trialEnergy) < fabs(st->popEnergy[candidate])) {
                    // New low for this candidate
                    st->popEnergy[candidate] = st->trialEnergy;
                    VecCopy(st->nDim, Mat2Row(st->nDim, st->population, candidate), st->trialSolution);

                    // Check if all-time low
                    if (fabs(st->trialEnergy) < fabs(st->bestEnergy)) {
                        st->bestEnergy = st->trialEnergy;
                        VecCopy(st->nDim, st->bestSolution, st->trialSolution);
                    }
                }
                break;
            }
        }
        if (st->pcx)
            dprintf("de_Solve: generation %d; bestEnergy=%+1.6E; trialEnergy=%+1.6E; pcx_dmean=%+1.6E; AtSolution=%d\r",
                generation, st->bestEnergy, st->trialEnergy, st->pcx_dmean, bAtSolution);
        else
            dprintf("de_Solve: generation %d; bestEnergy=%+1.6E; trialEnergy=%+1.6E; AtSolution=%d\r",
                generation, st->bestEnergy, st->trialEnergy, bAtSolution);
    }
    dprintf("\n");
    return(bAtSolution);
}
