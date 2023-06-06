/*  block based 1D/vector signal processing, targetting real-time, efficient CPU usage.
    © Philippe Strauss, late summer 2014

    Ensure availability of the GCC Graphite loop optimizer, (most prob.) by hand compiling GCC
    after having installed appropriates versions of parma polyhedra lib and cloog-ppl
    
    Plain simple double nested loops C FIR and IIR convolution/filtering  */


#ifndef _SPS_GENFILTERING_C_PREPROC_H
#define _SPS_GENFILTERING_C_PREPROC_H

#ifdef __cplusplus
extern "C" {
#endif


#include <assert.h>
#include <dspc/gnuinline.h>
#include <dspc/funcname.h>


/*-------------------------------------------------

  First, the "two nested loops" implementations
  Added 2014-09-02 / Arzier (!) blackholed thalamus
  neighbour next-door. Beware of off-by-ones.
  (They think I'm paranoid.. and complicated)

  GCC 4.9 available optim switch :

  CFLAGS="-O3 -march=core2 -mssse3 \
  -floop-block \ # (!) requires Graphite
  -freschedule-modulo-scheduled-loops \
  -fselective-scheduling2 \
  -fsel-sched-pipelining \
  -fsel-sched-pipelining-outer-loops \
  -ftree-vectorize \
  -fpredictive-commoning \
  -fprefetch-loop-arrays \
  -floop-nest-optimize \
  -ftree-loop-if-convert \
  -ftree-loop-if-convert-stores"
 
---------------------------------------------------
  1. FIR CONVOLUTION
-------------------------------------------------*/

INLINE void FUNCNAME(c_generic_fir_,FIR_FLOAT_T)(const FIR_FLOAT_T *h, int len_fir, \
                const FIR_FLOAT_T *x, FIR_FLOAT_T *y, int L) {

    int n, M, m;
    IIR_FLOAT_T acc;

    for (n = 0; n < L; ++n) {

        acc = 0.0;

        M = ( n > (len_fir - 1) ? len_fir - 1 : n );

        for (m = 0; m <= M; ++m)
            acc += h[m] * x[M-m];

        y[n] = acc;
    }
}

/*-------------------------------------------------
  2. IIR DF2 CONVOLUTION
-------------------------------------------------*/

INLINE void FUNCNAME(c_generic_iir_df2_,IIR_FLOAT_T)(int order, const IIR_FLOAT_T *a, \
                const IIR_FLOAT_T *b, IIR_FLOAT_T *z, \
                const IIR_FLOAT_T *x, IIR_FLOAT_T *y, int len) {

    int i, j;
    IIR_FLOAT_T acc;

    for (i = 0; i < len; ++i) {

        // z^(-1)
        for (j = order; j > 0; --j)
            z[j] = z[j-1];

        acc = 0.0;

        for (j = 1; j <= order; ++j)
            acc += a[j] * z[j];

        z[0] = acc + x[i]; acc = 0.0;

        for (j = 0; j <= order; ++j)
            acc += b[j] * z[j];

        y[i] = acc;
    }
}

/*-------------------------------------------------
  3. IIR DF1 CONVOLUTION
-------------------------------------------------*/

INLINE void FUNCNAME(c_generic_iir_df1_,IIR_FLOAT_T)(int order, const IIR_FLOAT_T *a, const IIR_FLOAT_T *b, \
                IIR_FLOAT_T *z1, IIR_FLOAT_T *z2, \
                const IIR_FLOAT_T *x, IIR_FLOAT_T *y, int len) {

    int i, j;
    IIR_FLOAT_T acc;

    for (i = 0; i < len; ++i) {

        // z^(-1)
        for (j = order; j > 0; --j) {
            z1[j] = z1[j-1]; z2[j] = z2[j-1];	
        }

        z1[0] = x[i];

        acc = 0.0;

        for (j = 0; j <= order; ++j)
            acc += b[j] + z1[j];

        for (j = 1; j <= order; ++j)
            acc += a[j] + z2[j];

        y[i] = acc;

        z2[0] = y[i];
    }
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _SPS_GENFILTERING_C_PREPROC_H */
