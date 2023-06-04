/*  Differential evolution solver heurisitc
    Lester E. Godwin
    Partially rewrote, plain ANSII C port,
    enhanced (EitherOr and Probabilist Parent Centric evolution strategies)
    plus maintained by Philippe Strauss, Spring 2012.  */


#include <dspc/de.h>
#include <dspc/perr.h>
#include <dspc/de_strategy_helpers.h>
#include <dspc/de_rng_helpers.h>


extern cb_trial_t fctProbPCX_StdTrial;


/*  Exponential crossover : p.93,
    Binomial crossover : p.95 & p.96, synonym : uniform. */

/* Ph.Strauss : partial rewrite for the obligatory initial mutation of at least one dimension param 
   as defined by Storn & Price */
void Best1Exp(de_t *st, int candidate) {
    int r1, r2;
    int i, in, n;

    SelectSamples(st, candidate, &r1, &r2, NULL, NULL, NULL);
    n = rand_dim(st);

    VecCopy(st->nDim, st->trialSolution, Mat2Row(st->nDim, st->population, candidate));
    /* crossover : eq 2.6 p. 39; p.101 § Rotational Invariance */
    /* eq. p.39, §2.1.4 eq. 2.6, also read p.76 Zaharie work, ...By Dropping... */
    Mutate2(st, n, st->bestSolution, r1, r2);
    in = n;

    for (i = 1; (uniform_double(&(st->rng)) < st->probability) && (i < st->nDim); i++) {

        in = (n + i) % st->nDim;
        Mutate2(st, in, st->bestSolution, r1, r2);

    }

    return;
}

/* DE/best/1/bin : p.72 § 2.4.7 */

void Best1Bin(de_t *st, int candidate) {
    int r1, r2;
    int i, in, n;

    SelectSamples(st, candidate, &r1, &r2, NULL, NULL, NULL);
    n = rand_dim(st);

    VecCopy(st->nDim, st->trialSolution, Mat2Row(st->nDim, st->population, candidate));
    /* crossover : eq 2.6 p. 39; p.101 § Rotational Invariance */
    /* eq. p.39, §2.1.4 eq. 2.6, also read p.76 Zaharie work, ...By Dropping... */
    Mutate2(st, n, st->bestSolution, r1, r2);
    in = n;

    for (i = 1; i < st->nDim; i++) {

        in = (n + i) % st->nDim;
        if (uniform_double(&(st->rng)) < st->probability)
            Mutate2(st, in, st->bestSolution, r1, r2);

    }

    return;
}

void Rand1Exp(de_t *st, int candidate) {
    int r1, r2, r3;
    int i, in, n;

    SelectSamples(st, candidate, &r1, &r2, &r3, NULL, NULL);
    n = rand_dim(st);

    VecCopy(st->nDim, st->trialSolution, Mat2Row(st->nDim, st->population, candidate));

    Mutate2(st, n, Mat2Row(st->nDim, st->population, r1), r2, r3);
    in = n;

    for (i = 1; (uniform_double(&(st->rng)) < st->probability) && (i < st->nDim); i++) {

        in = (n + i) % st->nDim;
        Mutate2(st, in, Mat2Row(st->nDim, st->population, r1), r2, r3);

    }

    return;
}

void Rand1Bin(de_t *st, int candidate) {
    int r1, r2, r3;
    int i, in, n;

    SelectSamples(st, candidate, &r1, &r2, &r3, NULL, NULL);
    n = rand_dim(st);

    VecCopy(st->nDim, st->trialSolution, Mat2Row(st->nDim, st->population, candidate));

    Mutate2(st, n, Mat2Row(st->nDim, st->population, r1), r2, r3);
    in = n;

    for (i = 1; i < st->nDim; i++) {

        in = (n + i) % st->nDim;
        if (uniform_double(&(st->rng)) < st->probability)
            Mutate2(st, in, Mat2Row(st->nDim, st->population, r1), r2, r3);

    }

    return;
}

/*  Arithmetic recombination, Three vectors recombination :
    § 2.1.3 p. 38
    § 2.4.7 p. 73 below eq. 2.21
    -> p. 106 § Line recombination vs. diff. mutation & fig. 2.48
    => p. 117  */

void EitherOr(de_t *st, int candidate) {
    int i, r1, r2;
    double r0;

    SelectSamples(st, candidate, &r1, &r2, NULL, NULL, NULL);

    if(uniform_double(&(st->rng)) < st->probability) {
        /* mutate only */
        for(i = 0; i < st->nDim; ++i)
            Mutate2(st, i, Mat2Row(st->nDim, st->population, candidate), r1, r2);
    } else {
        /* recombine only */
        for(i = 0; i < st->nDim; ++i) {
            r0 = Mat2Elt(st->nDim, st->population, candidate, i);
            st->trialSolution[i] = r0 +
#ifndef RANDOMIZED_F_K
            st->k * (Mat2Elt(st->nDim, st->population, r1, i) +
                     Mat2Elt(st->nDim, st->population, r2, i) - 2 * r0);
#else
            fctK(st) * (Mat2Elt(st->nDim, st->population, r1, i) +
                      Mat2Elt(st->nDim, st->population, r2, i) - 2 * r0);           
#endif
        }
    }
}

/*  Probabilist parent centric mutation/crossover
    use it with low st->probabilityPCX (0.01 - 0.07) for giving an edge to convergence.

    Written by Philippe Strauss.

    TODO :
        - maybe implement some form of crossover (ambiguity is left in the papers),
        - investigate rotational invariance, 

    Reference:
        "Two modified differential evolution algorithms
        and their applications to engineering design problems",

        Musrrat Ali, Millie Pant, V.P. Singh,

        World Journal of Modelling and Simulation,
        Vol. 6 (2010) No. 1, pp. 72-80,
        ISSN 1 746-7233, England, UK  */

void ProbParentCentric(de_t *st, int candidate) {
    int r1, i;
    double besti, candi, r1i, di;
    double d_norm = 0.0;
    double po_norm2_cand = 0.0;
    double po_norm2_r1 = 0.0;
    double dotp_cand = 0.0;
    double dotp_r1 = 0.0;
    double dist_cand, dist_r1;
    double normranda, normrandb;
    double sqcand, sqr1;
    double trialn;

    if (uniform_double(&(st->rng)) < st->probabilityPCX) {
        SelectSamples(st, candidate, &r1, NULL, NULL, NULL, NULL);
        for (i = 0; i < st->nDim; ++i) {
            besti = st->bestSolution[i];
            candi = Mat2Elt(st->nDim, st->population, candidate, i);
            r1i = Mat2Elt(st->nDim, st->population, r1, i);
            /* for mean vector g */
            st->pcx_g[i] = (besti + candi + r1i) / 3.0;
            /* directional vector dp */
            di = st->bestSolution[i] - st->pcx_g[i];
            st->pcx_dp[i] = di;
            d_norm += pow(di, 2.0);
            /*  for mean perpendicular distance to dp vector support.

                d^2 = ||[P,O]||^2 - <[P,O].dp_u>^2

                In PO, O can be the tip of the best vector (prefered), or the base of dp
                P beeing the tip of the candidate or r1 vector  */
            st->pcx_po_cand[i] = besti - candi;
            po_norm2_cand += pow(st->pcx_po_cand[i], 2.0);

            st->pcx_po_r1[i] = besti - r1i;
            po_norm2_r1 += pow(st->pcx_po_r1[i], 2.0);
            vdprintf("candidate=%d, i=%d, besti=%f, candi=%f, r1i=%f\n", candidate, i, besti, candi, r1i);
        }
        d_norm = sqrt(d_norm);
        /* dot product of candidate and r1 w. dp */
        for (i = 0; i < st->nDim; ++i) {
            dotp_cand += st->pcx_dp[i] * st->pcx_po_cand[i] / d_norm;
            dotp_r1 += st->pcx_dp[i] * st->pcx_po_r1[i] / d_norm;
        }
        /*  distances
            got small errors of 10^-15 to 10^-22 of negative sign */
        sqcand = po_norm2_cand - pow(dotp_cand, 2.0);
        sqr1 = po_norm2_r1 - pow(dotp_r1, 2.0);
        /* quick workaround (FIXME : use float.h ?)*/
        if (sqcand <= 0.0)
            sqcand = 0.0;
        if (sqr1 <= 0.0)
            sqr1 = 0.0;
        dist_cand = sqrt(sqcand);
        dist_r1 = sqrt(sqr1);
        st->pcx_dmean = (dist_cand + dist_r1) / 2.0;
        normranda = normal_double(&(st->rng), 0.0, st->pcx_sigma2a);
        normrandb = normal_double(&(st->rng), 0.0, st->pcx_sigma2b);
        /* new trial */
        for (i = 0; i < st->nDim; ++i) {
            /* FIXME: i != p */
            trialn = st->bestSolution[i] + normranda * st->pcx_dp[i] + st->pcx_dmean * normrandb;
            st->trialSolution[i] = clip(trialn, i, st);
        }
    } else {
        /* FIXME : PST 2023-04-03 : A function pointer w. its storage as a global */
        (*fctProbPCX_StdTrial)(st, candidate);
    }
}


/*  see p.73 below eq. 2.21 for strategy target-to-best/1/bin also named rand-to-best
    described with more details on § 2.6.3

    also on p.73 § Compensating Lost Diversity :
    ... Except for a few early successes on relatively simple funtions, this method
    has not shown much promise, perhaps because adding difference vector destroys
    the correlation that the objective functions's topography imparts to the one-difference
    vector differentials ... */
void Best2Exp(de_t *st, int candidate) {
    int r1, r2, r3, r4;
    int i, in, n;

    SelectSamples(st, candidate, &r1, &r2, &r3, &r4, NULL);
    n = rand_dim(st);

    VecCopy(st->nDim, st->trialSolution, Mat2Row(st->nDim, st->population, candidate));

    Mutate4(st, n, st->bestSolution, r1, r2, r3, r4);
    in = n;

    for (i = 1; (uniform_double(&(st->rng)) < st->probability) && (i < st->nDim); i++) {

        in = (n + i) % st->nDim;
        Mutate4(st, in, st->bestSolution, r1, r2, r3, r4);

    }

    return;
}

void Best2Bin(de_t *st, int candidate) {
    int r1, r2, r3, r4;
    int i, in, n;

    SelectSamples(st, candidate, &r1, &r2, &r3, &r4, NULL);
    n = rand_dim(st);

    VecCopy(st->nDim, st->trialSolution, Mat2Row(st->nDim, st->population, candidate));

    Mutate4(st, n, st->bestSolution, r1, r2, r3, r4);
    in = n;

    for (i = 1; i < st->nDim; i++) {

        in = (n + i) % st->nDim;
        if (uniform_double(&(st->rng)) < st->probability)
            Mutate4(st, in, st->bestSolution, r1, r2, r3, r4);

    }

    return;
}

void Rand2Exp(de_t *st, int candidate) {
    int r1, r2, r3, r4, r5;
    int i, in, n;

    SelectSamples(st, candidate, &r1, &r2, &r3, &r4, &r5);
    n = rand_dim(st);

    VecCopy(st->nDim, st->trialSolution, Mat2Row(st->nDim, st->population, candidate));

    Mutate4(st, n, Mat2Row(st->nDim, st->population, r1), r2, r3, r4, r5);
    in = n;

    for (i = 1; (uniform_double(&(st->rng)) < st->probability) && (i < st->nDim); i++) {

        in = (n + i) % st->nDim;
        Mutate4(st, in, Mat2Row(st->nDim, st->population, r1), r2, r3, r4, r5);

    }

    return;
}

void Rand2Bin(de_t *st, int candidate) {
    int r1, r2, r3, r4, r5;
    int i, in, n;

    SelectSamples(st, candidate, &r1, &r2, &r3, &r4, &r5);
    n = rand_dim(st);

    VecCopy(st->nDim, st->trialSolution, Mat2Row(st->nDim, st->population, candidate));

    Mutate4(st, n, Mat2Row(st->nDim, st->population, r1), r2, r3, r4, r5);
    in = n;

    for (i = 1; i < st->nDim; i++) {

        in = (n + i) % st->nDim;
        if (uniform_double(&(st->rng)) < st->probability)
            Mutate4(st, in, Mat2Row(st->nDim, st->population, r1), r2, r3, r4, r5);

    }

    return;
}
