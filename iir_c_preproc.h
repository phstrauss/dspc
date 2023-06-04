/*  block based 1D/vector signal processing, targetting real-time, efficient CPU usage.
    © Philippe Strauss, 2011

    comments about resulting code generated: using gcc-4.6.1, itself hand compiled to be sure to have
    graphite loop optimizer, requiring parma polyhedra lib and cloog-ppl */


#ifndef _SPS_IIR_C_PREPROC_H
#define _SPS_IIR_C_PREPROC_H

#ifdef __cplusplus
extern "C" {
#endif


#include <assert.h>
#include <dspc/gnuinline.h>
#include <dspc/funcname.h>


/*  IIR filtering, __ (hand) unrolled inner loop __.

    *** BIG FAT WARNING ***
    *** YOU NEED TO FLIP THE SIGN OF A/DENOM COEFFS BEFORE USING ***
    *** see i.e. Proakis & Manolakis p. 492 ***  */


/*  Direct Form 2 IIR processing loop  */

INLINE void FUNCNAME(c_iir1_df2_,IIR_FLOAT_T)(const IIR_FLOAT_T a, const IIR_FLOAT_T *b,
    IIR_FLOAT_T *z, float *in, float *out, int l) {

    int i;

    for (i = 0; i < l; ++i) {
        z[1] = z[0];

        z[0]   = in[i] + a * z[1];
        out[i] = b[0] * z[0] + b[1] * z[1];
    }
}

INLINE void FUNCNAME(c_iir2_df2_,IIR_FLOAT_T)(const IIR_FLOAT_T *b, const IIR_FLOAT_T *a,
    IIR_FLOAT_T *z, float *in, float *out, int l) {

    int i;

    for (i = 0; i < l; ++i) {
        /* delay z-1 */
        z[2] = z[1]; z[1] = z[0];

        /*  a[0] is assumed to be eq. to 1.0, even if never accessed.  */
        z[0]        = in[i] + a[1] * z[1] + a[2] * z[2];
        out[i]  = b[0] * z[0] + b[1] * z[1] + b[2] * z[2];
    }
}

INLINE void FUNCNAME(c_iir3_df2_,IIR_FLOAT_T)(const IIR_FLOAT_T *a, const IIR_FLOAT_T *b,
    IIR_FLOAT_T *z, float *in, float *out, int l) {

    int i;

    for (i = 0; i < l; ++i) {
        z[3] = z[2]; z[2] = z[1]; z[1] = z[0];

        z[0] = in[i] + a[1] * z[1] + a[2] * z[2] + a[3] * z[3];
        out[i] = b[0] * z[0] + b[1] * z[1] + b[2] * z[2] + b[3] * z[3];
    }
}

INLINE void FUNCNAME(c_iir4_df2_,IIR_FLOAT_T)(const IIR_FLOAT_T *a, const IIR_FLOAT_T *b,
    IIR_FLOAT_T *z, float *in, float *out, int l) {

    int i;

    for (i = 0; i < l; ++i) {
        z[4] = z[3]; z[3] = z[2]; z[2] = z[1]; z[1] = z[0];

        z[0] = in[i] + a[1] * z[1] + a[2] * z[2] + a[3] * z[3] + a[4] * z[4];
        out[i] = b[0] * z[0] + b[1] * z[1] + b[2] * z[2] + b[3] * z[3] + b[4] * z[4];
    }
}

INLINE void FUNCNAME(c_iir6_df2_,IIR_FLOAT_T)(const IIR_FLOAT_T *a, const IIR_FLOAT_T *b,
    IIR_FLOAT_T *z, float *in, float *out, int l) {

    int i;

    for (i = 0; i < l; ++i) {
        z[6] = z[5]; z[5] = z[4]; z[4] = z[3]; z[3] = z[2]; z[2] = z[1]; z[1] = z[0];

        // RB : a[1] * z[0] ... + a[6] * z[5]; z[0] is now z[6]
        // 6 + (idx-- % 6) ; (ofs + n) % 6
        z[0] = in[i] + a[1] * z[1] + a[2] * z[2] + a[3] * z[3] + a[4] * z[4] + a[5] * z[5] + a[6] * z[6];
        out[i] = b[0] * z[0] + b[1] * z[1] + b[2] * z[2] + b[3] * z[3] + b[4] * z[4] + b[5] * z[5] + b[6] * z[6];
    }
}


/*  Direct Form 1  */

INLINE void FUNCNAME(c_iir1_df1_,IIR_FLOAT_T)(const IIR_FLOAT_T *b, const IIR_FLOAT_T *a,
    IIR_FLOAT_T *z1, IIR_FLOAT_T *z2, float *in, float *out, int l) {

    int i;

    for (i = 0; i < l; ++i) {
        z1[1] = z1[0]; z1[0] = in[i];
        z2[1] = z2[0];

        out[i]  = b[0] * z1[0] + b[1] * z1[1] +
            a[1] * z2[1];

        z2[0] = out[i];
    }
}

INLINE void FUNCNAME(c_iir2_df1_,IIR_FLOAT_T)(const IIR_FLOAT_T *b, const IIR_FLOAT_T *a,
    IIR_FLOAT_T *z1, IIR_FLOAT_T *z2, float *in, float *out, int l) {

    int i;

    for (i = 0; i < l; ++i) {
        z1[2] = z1[1]; z1[1] = z1[0]; z1[0] = in[i];
        z2[2] = z2[1]; z2[1] = z2[0];

        out[i]  = b[0] * z1[0] + b[1] * z1[1] + b[2] * z1[2] +
            a[1] * z2[1] + a[2] * z2[2];

        z2[0] = out[i];
    }
}

INLINE void FUNCNAME(c_iir3_df1_,IIR_FLOAT_T)(const IIR_FLOAT_T *b, const IIR_FLOAT_T *a,
    IIR_FLOAT_T *z1, IIR_FLOAT_T *z2, float *in, float *out, int l) {

    int i;

    for (i = 0; i < l; ++i) {
        z1[3] = z1[2]; z1[2] = z1[1]; z1[1] = z1[0]; z1[0] = in[i];
        z2[3] = z2[2]; z2[2] = z2[1]; z2[1] = z2[0];

        out[i]  = b[0] * z1[0] + b[1] * z1[1] + b[2] * z1[2] + b[3] * z1[3] +
            a[1] * z2[1] + a[2] * z2[2] + a[3] * z2[3];

        z2[0] = out[i];
    }
}

INLINE void FUNCNAME(c_iir4_df1_,IIR_FLOAT_T)(const IIR_FLOAT_T *b, const IIR_FLOAT_T *a,
    IIR_FLOAT_T *z1, IIR_FLOAT_T *z2, float *in, float *out, int l) {

    int i;

    for (i = 0; i < l; ++i) {
        z1[4] = z1[3]; z1[3] = z1[2]; z1[2] = z1[1]; z1[1] = z1[0]; z1[0] = in[i];
        z2[4] = z2[3]; z2[3] = z2[2]; z2[2] = z2[1]; z2[1] = z2[0];

        out[i]  = b[0] * z1[0] + b[1] * z1[1] + b[2] * z1[2] + b[3] * z1[3] + b[4] * z1[4] +
            a[1] * z2[1] + a[2] * z2[2] + a[3] * z2[3] + a[4] * z2[4];

        z2[0] = out[i];
    }
}

INLINE void FUNCNAME(c_iir6_df1_,IIR_FLOAT_T)(const IIR_FLOAT_T *b, const IIR_FLOAT_T *a,
    IIR_FLOAT_T *z1, IIR_FLOAT_T *z2, float *in, float *out, int l) {

    int i;

    for (i = 0; i < l; ++i) {
        z1[6] = z1[5]; z1[5] = z1[4]; z1[4] = z1[3]; z1[3] = z1[2]; z1[2] = z1[1]; z1[1] = z1[0]; z1[0] = in[i];
        z2[6] = z2[5]; z2[5] = z2[4]; z2[4] = z2[3]; z2[3] = z2[2]; z2[2] = z2[1]; z2[1] = z2[0];

        out[i]  = b[0] * z1[0] + b[1] * z1[1] + b[2] * z1[2] + b[3] * z1[3] + b[4] * z1[4] + b[5] * z1[5] + b[6] * z1[6] +
            a[1] * z2[1] + a[2] * z2[2] + a[3] * z2[3] + a[4] * z2[4] + a[5] * z2[5] + a[6] * z2[6];

        z2[0] = out[i];
    }
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _IIR_PREPROC_H */
