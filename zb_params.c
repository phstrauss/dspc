/*  Audio signal processing - filter (design) helpers
    Zölzer-Boltze (ZB) peak/notch PEQ parameters computation 
    © Philippe Strauss, 2011  */


#include <math.h>
#include <dspc/iir_utils.h>
#include <dspc/iir_zb.h>


// use with gp = 1 for boost case
// ax: ZB paper use ab for boost, ac for cut, abc for both cases, hence ax
// gp: Vo in ZB paper
// omw: omega of bandWidth
double peq_zb_ax(double ww, double gp) {
    double tom = tan(ww/2.0);
    return (tom-gp) / (tom+gp);
}

void peq_zb_params(double fc, double fbw, double fs, double gp, zb_params_t *params) {
    params->hp = gp-1.0;
    params->omega_c = omega_norm(fc, fs);
    params->omega_w = omega_norm(fbw, fs);
    params->d = -cos(params->omega_c);
    if (gp > 1.0) {
        params->ax = peq_zb_ax(params->omega_w, 1.0);
    } else {
        params->ax = peq_zb_ax(params->omega_w, gp);
    }
}

void peq_zb_coeffs(double ax, double hp, double d, biquad_coeffs_c_t *coeffs) {
    double cmid = d * (1.0 - ax);
    coeffs->b_num[0] = -ax ;
    coeffs->b_num[1] = cmid ;
    coeffs->b_num[2] = 1.0 ;
    // denominator
    coeffs->a_denom[0] = 1.0 ; // trivial & unused
    coeffs->a_denom[1] = -cmid ;
    coeffs->a_denom[2] = ax ;
}

void peq_zb_coeffs_f(double ax, double hp, double d, float *b_num, float *a_denom) {
    double cmid = d * (1.0 - ax);
    b_num[0] = -ax ;
    b_num[1] = cmid ;
    b_num[2] = 1.0 ;
    // denominator
    a_denom[0] = 1.0 ; // trivial & unused
    a_denom[1] = -cmid ;
    a_denom[2] = ax ;
}

void peq_zb_coeffs_d(double ax, double hp, double d, double *b_num, double *a_denom) {
    double cmid = d * (1.0 - ax);
    b_num[0] = -ax ;
    b_num[1] = cmid ;
    b_num[2] = 1.0 ;
    // denominator
    a_denom[0] = 1.0 ; // trivial & unused
    a_denom[1] = -cmid ;
    a_denom[2] = ax ;
}

// do it all, could be only public funcs
void peq_zb_peak(double fc, double fs, double q, double gdb, zb_params_t *params, biquad_coeffs_c_t *coeffs) {
    double bw = bw_from_q(fc, q);
    double gp = pow(10.0, gdb / 20.0);
    peq_zb_params(fc, bw, fs, gp, params);
    peq_zb_coeffs(params->ax, params->hp, params->d, coeffs);
}

void peq_zb_peak_f(double fc, double fs, double q, double gdb, zb_params_t *params, float *b_num, float *a_denom) {
    double bw = bw_from_q(fc, q);
    double gp = pow(10.0, gdb / 20.0);
    peq_zb_params(fc, bw, fs, gp, params);
    peq_zb_coeffs_f(params->ax, params->hp, params->d, b_num, a_denom);
}

void peq_zb_peak_d(double fc, double fs, double q, double gdb, zb_params_t *params, double *b_num, double *a_denom) {
    double bw = bw_from_q(fc, q);
    double gp = pow(10.0, gdb / 20.0);
    peq_zb_params(fc, bw, fs, gp, params);
    peq_zb_coeffs_d(params->ax, params->hp, params->d, b_num, a_denom);
}
