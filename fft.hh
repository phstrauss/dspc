/*  Audio Signal Processing routines in C++
    © Philippe Strauss, 2013  */


#ifndef _SPS_FFT_HH
#define _SPS_FFT_HH


#include <dspc/dspc.h>
#include <fftw3.h>
#include <cmath>
#include <cstring>
#include <dspc/templating.hh>


/* from FFTW manual :

§ 2.5.1 The Halfcomplex-format DFT
An r2r kind of FFTW_R2HC (r2hc) corresponds to an r2c DFT (see Section 2.3 [One-Dimensional DFTs of
Real Data], page 6) but with “halfcomplex” format output, and may sometimes be faster and/or more
convenient than the latter. The inverse hc2r transform is of kind FFTW_HC2R. This consists of the
non-redundant half of the complex output for a 1d real-input DFT of size n, stored as a sequence
of n real numbers (double) in the format:

r0, r1, r2, ..., rn/2,  i(n+1)/2−1, ..., i2, i1

Here, rk is the real part of the kth output, and ik is the imaginary part.
(Division by 2 is rounded down.) For a halfcomplex array hc[n], the kth component thus has its
real part in hc[k] and its imaginary part in hc[n-k], with the exception of k == 0 or n/2
(the latter only if n is even)—in these two cases, the imaginary part is zero due to symmetries
of the real-input DFT, and is not stored. Thus, the r2hc transform of n real values is a
halfcomplex array of length n, and vice versa for hc2r.

...

As a result of this symmetry, half of the output Y is redundant (being the complex conjugate
of the other half), and so the 1d r2c transforms only output elements 0. . . n/2 of Y (n/2 + 1
complex numbers), where the division by 2 is rounded down.
Moreover, the Hermitian symmetry implies that Y 0 and, if n is even, the Y n/2 element, are
purely real. So, for the R2HC r2r transform, these elements are not stored in the halfcomplex
output format.

The c2r and H2RC r2r transforms compute the backward DFT of the complex array X
with Hermitian symmetry, stored in the r2c/R2HC output formats, respectively, where the
backward transform is defined exactly as for the complex case.

(formula)

The outputs Y of this transform can easily be seen to be purely real, and are stored as an
array of real numbers.

FFTW computes an unnormalized transform, in that there is no coefficient in front of
the summation in the DFT.
In other words, applying the forward and then the backward transform will multiply
the input by n. */


enum class ft_mag {field, power, db};
typedef enum ft_mag ft_mag_t;

enum class ft_fill {internal, external};
typedef enum ft_fill ft_fill_t;


EXPORT template <ft_fill_t fill_kind, ft_mag_t mag_kind>
class fft_real_t {

public:
    fft_real_t(int len);
    ~fft_real_t(void);
    int get_len(void);
    int get_half_len(void);
    float_t *get_buffer_ptr(void);
    /* cartesian (complex) out */
    void inline process_forward(const float_t *wave, fftwf_complex *out);
    /* polar, magnitude part : ft_mag_t : field, power, db */
    void inline process_forward(const float_t *wave, float_t *magx, float_t *phase);
    /* magnitude only, using ft_mag_t */
    void inline process_forward(const float_t *wave, float_t *magx);
    /* cartesian */
    void inline process_backward(const fftwf_complex *in, float_t *wave);
    /* polar */
    void inline process_backward(const float_t *mag_field, const float_t *phase, float_t *wave);

private:
    int len, n2;
    float_t n_2;
    float_t *x;
    fftwf_plan plan_forward;
    fftwf_plan plan_backward;
};


#include <dspc/exception.hh>
#include <cassert>
#include <dspc/ft_utils.h>


EXPORT template <ft_fill_t fill_kind, ft_mag_t mag_kind>
fft_real_t<fill_kind, mag_kind>
::fft_real_t(int n): len(n) {
    assert(len % 16 == 0);

    n2 = n/2; /* PST : WW : changed 2014-10-17, 2015-03-07, watch out for off by ones */
    n_2 = (float_t) n2;

    x = (float_t *) fftwf_malloc(len * sizeof(float_t));
    if (!x)
        EXN_MSG_THROW("fftwf_malloc()", "failed");
    plan_forward = fftwf_plan_r2r_1d(len, x, x, FFTW_R2HC, FFTW_ESTIMATE);
    if (!plan_forward) {
        fftwf_free(x);
        EXN_MSG_THROW("fftwf_plan_r2r_1d(FFTW_R2HC)", "failed");
    }
    plan_backward = fftwf_plan_r2r_1d(len, x, x, FFTW_HC2R, FFTW_ESTIMATE);
    if (!plan_backward) {
        fftwf_free(x);
        EXN_MSG_THROW("fftwf_plan_r2r_1d(FFTW_HC2R)", "failed");
    }
}

EXPORT template <ft_fill_t fill_kind, ft_mag_t mag_kind>
fft_real_t<fill_kind, mag_kind>
::~fft_real_t(void) {
    fftwf_destroy_plan(plan_forward);
    fftwf_destroy_plan(plan_backward);
    fftwf_free(x);
}

EXPORT template <ft_fill_t fill_kind, ft_mag_t mag_kind>
int fft_real_t<fill_kind, mag_kind>
::get_len(void) { return len; }

EXPORT template <ft_fill_t fill_kind, ft_mag_t mag_kind>
int fft_real_t<fill_kind, mag_kind>
::get_half_len(void) { return n2; }

EXPORT template <ft_fill_t fill_kind, ft_mag_t mag_kind>
float_t *fft_real_t<fill_kind, mag_kind>
::get_buffer_ptr(void) { return x; }


/* ***** PROCESS methods ***** */

/* cartesian out */
EXPORT template <ft_fill_t fill_kind, ft_mag_t mag_kind>
void inline fft_real_t<fill_kind, mag_kind>
::process_forward(const float_t *wave, fftwf_complex *out) {
    if (fill_kind == ft_fill_t::internal)
        memcpy(x, wave, (len*sizeof(float_t)));
    fftwf_execute(plan_forward);
    hc2complex(x, out, len, n2, n_2);
}

/* polar out */
EXPORT template <ft_fill_t fill_kind, ft_mag_t mag_kind>
void inline fft_real_t<fill_kind, mag_kind>
::process_forward(const float_t *wave, float_t *magx, float_t *phase) {
    if (fill_kind == ft_fill_t::internal)
        memcpy(x, wave, (len*sizeof(float_t)));
    fftwf_execute(plan_forward);
    hc2polar(x, magx, phase, len, n2, n_2);
    if (mag_kind ==ft_mag_t::field) return;
    if ((mag_kind == ft_mag_t::power) || (mag_kind == ft_mag_t::db)) {
        for (int i = 0; i < n2; ++i) magx[i] *= magx[i];
        if (mag_kind == ft_mag_t::power) return;        
        db_from_power(magx, n2);
        return;
    }
}

/* magnitude only out : WW : only power and dB implemented */
EXPORT template <ft_fill_t fill_kind, ft_mag_t mag_kind>
void inline fft_real_t<fill_kind, mag_kind>
::process_forward(const float_t *wave, float_t *magx) {
    if (mag_kind == ft_mag_t::field) EXN_MSG_THROW("mag_kind == ft_mag_t::field", "unimplemented");
    if (fill_kind == ft_fill_t::internal)
        memcpy(x, wave, (len * sizeof(float_t)));
    fftwf_execute(plan_forward);
    hc2power(x, magx, len, n2, n_2);
    if (mag_kind == ft_mag_t::power) return;
    db_from_power(magx, n2); return; /* PST : WW : crash here */
}

/* backward : cartesian */
EXPORT template <ft_fill_t fill_kind, ft_mag_t mag_kind>
void inline fft_real_t<fill_kind, mag_kind>
::process_backward(const fftwf_complex *in, float_t *wave) {
    complex2hc(in, x, len, n2, n_2);
    fftwf_execute(plan_backward);
    if (fill_kind == ft_fill_t::internal)
        memcpy(wave, x, (len*sizeof(float_t)));
}

/* backward : polar, magnitude : field value */
EXPORT template <ft_fill_t fill_kind, ft_mag_t mag_kind>
void inline fft_real_t<fill_kind, mag_kind>
::process_backward(const float_t *mag, const float_t *phase, float_t *wave) {
    polar2hc(mag, phase, x, len, n2, n_2);
    fftwf_execute(plan_backward);
    if (fill_kind == ft_fill_t::internal)
        memcpy(wave, x, (len*sizeof(float_t)));    
}


#endif
