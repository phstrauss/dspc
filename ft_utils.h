#ifndef _SPS_FTUTILS_H
#define _SPS_FTUTILS_H


#ifdef __cplusplus
extern "C" {
#endif


#include <fftw3.h>
#include <dspc/dspc.h>
#include <dspc/gnuinline.h>


INLINE float_t sqr(const float_t x) {
    return x * x;
}

INLINE float_t jnorm2(const float_t x, const float_t y) {
    return (sqr(x) + sqr(y));
}

INLINE float_t jabs(const float_t x, const float_t y) {
    return sqrt(jnorm2(x, y));
}

INLINE float_t jarg(const float_t x, const float_t y) {
    return atan2(y, x);
}


INLINE void db_from_power(float_t *power, const int len) {
    float_t epsilon = 1e-14; // power, not field value

    for(int i=0; i<len; ++i)
        if (power[i]>epsilon)
            power[i] = 10.0 * log10(power[i]);
        else
            power[i] = -140.0;
}

INLINE void energy_from_field(const float_t *sig, int len) {
    float_t sum = 0.0;
    for (int i = 0; i < len; ++i)
        sum += pow(sig[i], 2.0);
    printf("energy_from_field : %f\n", sum);
}

INLINE void energy_from_power(const float_t *sig, int len) {
    float_t sum = 0.0;
    for (int i = 0; i < len; ++i)
        sum += sig[i];
    printf("energy_from_power : %f\n", sum);
}

INLINE void energy_from_db(const float_t *sig, int len) {
    float_t sum = 0.0;
    for (int i = 0; i < len; ++i)
        sum += pow(10.0, sig[i]/10.0);
    printf("energy_from_db : %f\n", sum);
}


INLINE void hc2complex(const float_t *hc, fftwf_complex *cplx, const int len, const int n2, const float_t n_2) {
    /*  let len = arlen complex in
        let len2 = balen halfcomplex in
        assert (len2 = 2*len) ;
        for k = 1 to len-1 do
            complex.(k) <- { re = halfcomplex.{k} ; im = halfcomplex.{len2-k} } 
        done ;
        (* hermitian sym. and periodicity prop. *)
        complex.(0) <- { re = halfcomplex.{0} +. halfcomplex.{len} ; im = 0.0 }  */

    int i;
    for (i = 1; i < n2; ++i) {
        cplx[i][0] = hc[i] / n_2; cplx[i][1] = hc[len-i] / n_2;
    }
    /* PST : WW : index = n2 */
    cplx[0][0] = hc[0] + hc[n2] / len; cplx[0][i] = 0.0; 
}

INLINE void hc2polar(const float_t *hc, float_t *mag, float_t *phase, const int len, const int n2, const float_t n_2) {
    mag[0] = jabs(hc[0], 0.0) / len;
    phase[0] = jarg(hc[0], 0.0);

    for (int i = 1; i < n2; i++) {
        mag[i] = jabs(hc[i], hc[len-i]) / n_2; phase[i] = jarg(hc[i], hc[len-i]);
    }

    if (len > 0 && (len % 2 == 0)) {
        mag[n2] = jabs(hc[n2], 0.0) / len; phase[n2] = jarg(hc[n2], 0.0);
    } else {
        mag[n2] = jabs(hc[n2], hc[len-n2]) / n_2;
        phase[n2] = jarg(hc[n2], hc[len-n2]);
    }
}

INLINE void hc2power(const float_t *hc, float_t *power, const int len, const int n2, float_t n_2) {
    /* normalization */
    float_t sq_n_2 = n_2 * n_2;
    float_t sq_len = (float_t) len * len;

    power[0] = jnorm2(hc[0], 0.0) / sq_len;

    for (int i = 1; i < n2; i++)
        power[i] = jnorm2(hc[i], hc[len-i]) / sq_n_2;

    /* PST : indices & offsets triple checked 2015-03-07 */
    if (len > 0 && (len % 2 == 0))
        power[n2] = jnorm2(hc[n2], 0.0) / sq_len; /* storage space: n2+1 */
    else /* PST : WW : untested; WW : 20211012 this file has not yet been tested with other FFT length than powers of 2 (!!!) */
        power[n2] = jnorm2(hc[n2], hc[len-n2]) / sq_n_2;
}


/* for inverse FT */

INLINE void complex2hc(const fftwf_complex *in, float_t *out, const int len, const int n2, const float_t n_2) {
    /*  let len = arlen complex in
        let len2 = balen halfcomplex in
        assert (len2 = 2*len) ;
        for k = 1 to len-1 do
            halfcomplex.{k} <- complex.(k).re ;
            halfcomplex.{len2-k} <- complex.(k).im ;
        done ;
        let halfdc = complex.(0).re /. 2. in
        halfcomplex.{0} <- halfdc ;
        halfcomplex.{len} <- halfdc  */
    for (int i = 1; i < n2; ++i) {
        out[i] = in[i][0];
        out[len-i] = in[i][1];
    }
    float_t halfdc = in[0][0] / 2.0;
    out[0] = halfdc;
    out[n2] = halfdc;
}

INLINE void polar2hc(const float_t *mag, const float_t *phase, float_t *out, const int len, const int n2, const float_t n_2) {
    out[0] = mag[0] * cos(phase[0]);
    for (int i = 1; i < n2; i++) {
        out[i] = mag[i] * cos(phase[i]) / 2.0;
        out[len-i] = mag[i] * sin(phase[i]) / 2.0;
    }
    out[n2] = mag[n2] * cos(phase[n2]);
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _SPS_FTUTILS_H */
