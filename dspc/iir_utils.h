/*  Audio signal processing - filter (design) helpers
    comments about resulting code generated: using gcc-4.6.1, itself hand compiled to be sure to have
    graphite loop optimizer, requiring parma polyhedra lib and cloog-ppl
    © Philippe Strauss, 2011  */


#ifndef _SPS_IIR_UTILS_H
#define _SPS_IIR_UTILS_H


#ifdef __cplusplus
extern "C" {
#endif


#include <dspc/gnuinline.h>
#include <math.h>
#include <dspc/dspc.h>
#include <assert.h>

// A C++ file including a plain ansii C .h itself refering to <complex.h>
// often triggers a "template with C linkage" error
/*#ifdef __GXX_EXPERIMENTAL_CXX0X__
#undef __GXX_EXPERIMENTAL_CXX0X__
#include <complex.h>
#define __GXX_EXPERIMENTAL_CXX0X__
#else
#include <complex.h>
#endif*/


// epsilons suitable for audio
#define EPS_FIELD 1e-7
#define EPS_POWER 1e-14
// a minima for sampling freq
#define FS_MIN 1000
#define Q_MIN 0.01
#define FBW_MIN 0.2


// here c stand for ansii C
struct biquad_coeffs_c {
    float_t b_num[3];
    float_t a_denom[3];
};
typedef struct biquad_coeffs_c biquad_coeffs_c_t;

// d & f: double & float
struct sps_complex_d {double re; double im;};
struct sps_complex_f {float re; float im;};
typedef struct sps_complex_d sps_complex_d_t;
typedef struct sps_complex_f sps_complex_f_t;


INLINE double bw_from_q(double fc, double q);
INLINE double q_from_bw(double fc, double fbw);
INLINE double f_norm(double f, double fs);
INLINE double omega_norm(double f, double fs);
INLINE void biquad_a_sign_flip(biquad_coeffs_c_t *);
// iir_utils.c : bilinear xform
double omega_warp(double f, double fs);
double omega_dt2ct(double f, double fs);
void lp_bilin_biquad(double wc, double kn, double ctd, sps_complex_d_t pole, double *b_num, double *a_denom);
void hp_bilin_biquad(double wc, double kn, double ctd, sps_complex_d_t pole, double *b_num, double *a_denom);
// iir_utils.c : polynomial expansion
void poly2_from_conjugate(double r, double theta, double *poly2);
void poly3_from_poly2(const double *poly2, const double *poly1, double *poly3);
void poly4_from_poly2(const double *poly2a, const double *poly2b, double *poly4);
void poly6_from_poly2(const double *poly2a, const double *poly2b, const double *poly2c, double *poly6);


INLINE double bw_from_q(double fc, double q) {
    assert(q > Q_MIN);
    return fc / q;
}

INLINE double q_from_bw(double fc, double fbw) {
    assert(fbw > FBW_MIN);
    return fc / fbw;
}

INLINE double f_norm(double f, double fs) {
    assert(fs > FS_MIN);
    return (f / fs);    
}

INLINE double omega_norm(double f, double fs) {
    return 2 * M_PIl * f_norm(f, fs);
}

INLINE void biquad_a_sign_flip(biquad_coeffs_c_t *coeffs) {
    int i;

    for (i = 0; i < 3; ++i)
        coeffs->a_denom[i] = -coeffs->a_denom[i];
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
