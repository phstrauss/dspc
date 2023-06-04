/*  Audio signal processing - filter (design) helpers
    © Philippe Strauss, 2011 */


#ifndef _SPS_IIR_ZB_H
#define _SPS_IIR_ZB_H


#ifdef __cplusplus
extern "C" {
#endif


struct zb_params {
    double hp; // gp-1, Vo-1
    double omega_c;
    double omega_w;
    double d;
    double ax;
};
typedef struct zb_params zb_params_t;


// zb_params.c
double peq_zb_ax(double ww, double gp);
void peq_zb_params(double fc, double fbw, double fs, double gp, zb_params_t *params);

void peq_zb_coeffs(double ax, double hp, double d, biquad_coeffs_c_t *coeffs);
void peq_zb_coeffs_f(double ax, double hp, double d, float *b_num, float *a_denom);
void peq_zb_coeffs_d(double ax, double hp, double d, double *b_num, double *a_denom);

void peq_zb_peak(double fc, double fs, double q, double gdb, zb_params_t *params, biquad_coeffs_c_t *coeffs);
void peq_zb_peak_f(double fc, double fs, double q, double gdb, zb_params_t *params, float *b_num, float *a_denom);
void peq_zb_peak_d(double fc, double fs, double q, double gdb, zb_params_t *params, double *b_num, double *a_denom);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
