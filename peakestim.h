/*  Audio Signal Processing routines in C++
    © Philippe Strauss, philippe@strauss-acoustics.ch, 2013  */


#ifndef _SPS_PEAKESTIM_HH
#define _SPS_PEAKESTIM_HH


#ifdef __cplusplus
extern "C" {
#endif


#include <genc/genc.h>
#include <genc/gnuinline.h>
#include <assert.h>


typedef struct {float_t a; float_t p; float_t mag;} quad_estim_t;


INLINE float_t quad_a(float_t ya, float_t yb, float_t yc);
INLINE float_t freq_bin(float_t a, float_t ya, float_t yc);
INLINE float_t peak_mag(float_t ya, float_t yb, float_t yc, float_t p);
INLINE void quad_estimate(float_t ya, float_t yb, float_t yc, quad_estim_t *res);


/*  Ref.: SASP
    ya, yb, yc : magnitude of first, peak, second point (rel. freq. bin of maxima) on parabola
    ***  twice this value is the curvature  ***/
INLINE float_t quad_a(float_t ya, float_t yb, float_t yc) {
    return 0.5 * (ya - 2.0 * yb + yc);
}

/*  a : result of the previous  */
INLINE float_t freq_bin(float_t a, float_t ya, float_t yc) {
    float_t p = a * (ya - yc);
    assert(-0.5 <= p || p <= 0.5);
    return p;
}

/*  p : return of the previous, computed center freq. bin  */
INLINE float_t peak_mag(float_t ya, float_t yb, float_t yc, float_t p) {
    return yb - 0.25 * (ya - yc) * p;
}

/*  compute all params at once  */
INLINE void quad_estimate(float_t ya, float_t yb, float_t yc, quad_estim_t *res) {
    res->a = quad_a(ya, yb, yc);
    res->p = freq_bin(res->a, ya, yc);
    res->mag = peak_mag(ya, yb, yc, res->p);
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
