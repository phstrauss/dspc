/*  Audio signal processing - filter design helpers
    Butterwoth and linkwitz-riley max flat magnitude functions
    © Philippe Strauss, 2011  */


#ifndef _SPS_BUTTERWORTH_LR_H
#define _SPS_BUTTERWORTH_LR_H


#ifdef __cplusplus
extern "C" {
#endif


#include <math.h>
#include <assert.h>
#include <dspc/iir_utils.h>
#include <dspc/gnuinline.h>


INLINE double denom_butterworth_mag(double f, double fc, double order);
INLINE double butterworth_mag_lp(double f, double fc, double order);
INLINE double butterworth_mag_hp(double f, double fc, double order);
INLINE double lr_mag_lp(double f, double fc, double order);
INLINE double lr_mag_hp(double f, double fc, double order);


INLINE double denom_butterworth_mag(double f, double fc, double order) {
    return sqrt(pow(1 + f_norm(f, fc), (2 * order)));
}

INLINE double butterworth_mag_lp(double f, double fc, double order) {
    return (1 / denom_butterworth(f, fc, order));
}

INLINE double butterworth_mag_hp(double f, double fc, double order) {
    assert(order > 1.0);
    return (pow(f_norm(f, fc), order) / denom_butterworth(f, fc, order));
}

INLINE double lr_mag_lp(double f, double fc, double order) {
    return pow(butterworth_mag_lp(f, fc, order/2), 2);
}

INLINE double lr_mag_hp(double f, double fc, double order) {
    return pow(butterworth_mag_hp(f, fc, order/2), 2);
}


/* (*  see [OppWill] p. 704, eq. 9.147 - TESTED *)
let butterworth_pole_arc order k =
    assert(2*k < order) ;
    pi *. (2. *. (foi k) +. 1.) /. (2. *. (foi order)) +. (pi /. 2.) */

/* (*  polar to ortho w. omega_cutoff scaling
    see [OppWill] p. 704, eq. 9.148 - TESTED *)
let butterworth_pole ?kwc:(kw=1.0) k n =
    Complex.mul {re=kw; im=0.0} (Complex.exp {re=0.0; im=(butterworth_pole_arc k n)}) */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
