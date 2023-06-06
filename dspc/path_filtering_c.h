/*  block based 1D/vector signal processing, targetting real-time, efficient CPU usage.
    © Philippe Strauss, 2011

    comments about resulting code generated: using gcc-4.6.1, itself hand compiled to be sure to have
    graphite loop optimizer, requiring parma polyhedra lib and cloog-ppl */


#ifndef _SPS_PATH_FILTERING_C_H
#define _SPS_PATH_FILTERING_C_H

#ifdef __cplusplus
extern "C" {
#endif


#include <assert.h>
#include <dspc/gnuinline.h>



/* provide both float and double precision
   WW : Feature : choose also I/O floating point accuracy */

#define FIR_FLOAT_T float
#define IIR_FLOAT_T float

#include <dspc/genfiltering_c_preproc.h>
#include <dspc/iir_c_preproc.h>

#undef IIR_FLOAT_T
#undef FIR_FLOAT_T

#undef _SPS_IIR_C_PREPROC_H
#undef _SPS_GENFILTERING_C_PREPROC_H

#define FIR_FLOAT_T double
#define IIR_FLOAT_T double

#include <dspc/genfiltering_c_preproc.h>
#include <dspc/iir_c_preproc.h>

#undef FIR_FLOAT_T
#undef IIR_FLOAT_T


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _SPS_PATH_FILTERING_C_H */
