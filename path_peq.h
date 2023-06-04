/*  block based 1D/vector signal processing, targetting real-time, efficient CPU usage.
    © Philippe Strauss, 2011
    comments about resulting code generated: using gcc-4.5.3 or gcc-4.6.1, itself hand compiled to be sure to have
    graphite loop optimizer, requiring parma polyhedra lib and cloog-ppl  */


#ifndef _SPS_PATH_PEQ_PROCESS_H
#define _SPS_PATH_PEQ_PROCESS_H


#include <assert.h>


#ifdef __cplusplus
extern "C" {
#endif


#include <dspc/path_vec16_cintr.h>
#include <dspc/path_iir_c.h>
#include <dspc/gnuinline.h>


/*
    block based IIR processing of peak/notch zolbol PEQ
*/
INLINE void peq_zb_peak_process(biquad_coeffs_c_t *coeffs, float hp, float *z, float *in, float *out, int l) {
    int i;

    assert(l % 16 == 0);

    cintr_vec16_copy(in, out, l);
    c_iir2_df2_float(coeffs->b_num, coeffs->a_denom, z, out, out, l);
    for (i = 0; i < l; i += 16) {
        _cintr_vec16_vv1_sub(out+i, in+i);
        _cintr_vec16_cv_mul(out+i, 0.5 * hp);
        _cintr_vec16_vv2_add(in+i, out+i);
    }
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
