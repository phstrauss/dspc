/*  good quality random numbers, taken from numerical recipes, 3rd ed.,
    translated from C++ to ansi C.
    Philippe Strauss, 3.2012  */


#ifndef _SPS_RNG_H
#define _SPS_RNG_H


#ifdef __cplusplus
extern "C" {
#endif


#include <dspc/gnuinline.h>
#include <math.h>
#include <stdint.h>


/*  N.B.: GCC does not inline any functions when not optimizing unless you specify the `always_inline'
    attribute for the function, like this:

    inline void foo (const char) __attribute__((always_inline)); */

#define RAND_BSIZE 1024
#define ERR_OPEN -1
#define ERR_READ -2
#define ERR_DEVRANDOM -3

    struct rng_s {
        char random[RAND_BSIZE];    /* read from /dev/random or /dev/urandom */
        uint64_t u;
        uint64_t v;
        uint64_t w;
        double normal_stored;
    };

// rng.c
    int seed_from_devrandom(struct rng_s *st, uint64_t * seed);
    void init_rng(struct rng_s *st, uint64_t seed);
    int init_rng_from_devrandom(struct rng_s *st);
// end rng.c

    INLINE uint64_t uniform_int64(struct rng_s *st) {
        st->u = st->u * 2862933555777941757LL + 7046029254386353087LL;
        st->v ^= st->v >> 17;
        st->v ^= st->v << 31;
        st->v ^= st->v >> 8;
        st->w = 4294957665U * (st->w & 0xffffffff) + (st->w >> 32);
        uint64_t x = st->u ^ (st->u << 21);
         x ^= x >> 35;
         x ^= x << 4;

         return (x + st->v) ^ st->w;
    }

    INLINE uint32_t uniform_int32(struct rng_s * st) {
        return (uint32_t) uniform_int64(st);
    }

    INLINE double uniform_double(struct rng_s *st) {
        return 5.42101086242752217E-20 * uniform_int64(st);
    }

    INLINE double bounded_double(struct rng_s *st, double lower,
                                 double upper) {
        double span = upper - lower;
        return uniform_int64(st) * 5.42101086242752217E-20 * span + lower;
    }

/* 7.3.4 Normal Deviates by Transformation (Box-Muller) */
    INLINE double normal_double(struct rng_s *st, double mu, double sigma) {
        double v1, v2, rsq, fac;

        if (st->normal_stored == 0.0) {
            do {
                v1 = 2.0 * uniform_double(st) - 1.0;
                v2 = 2.0 * uniform_double(st) - 1.0;
                rsq = v1 * v1 + v2 * v2;
            } while (rsq >= 1.0 || rsq == 0.0);
            fac = sqrt(-2.0 * log(rsq) / rsq);
            st->normal_stored = v1 * fac;
            return mu + sigma * v2 * fac;
        } else {
            fac = st->normal_stored;
            st->normal_stored = 0.0;
            return mu + sigma * fac;
        }
    }


#ifdef __cplusplus
}                               /* extern "C" */
#endif


#endif
