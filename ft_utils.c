#include <dspc/ft_utils.h>
#include <stdbool.h>
#include <string.h> // memcpy


/* taken from pure-audio fftw.c */
bool fft_once_polar(const int n, const float_t *wave, float_t *mag, float_t *phase) {
    int i, n2 = n/2;
    float_t n_2 = ((float_t)n) / 2.0;
    float_t *x;
    fftwf_plan p;

    x = (float_t *) fftwf_malloc(n*sizeof(float_t));
    if (!x) return false;
    p = fftwf_plan_r2r_1d(n, x, x, FFTW_R2HC, FFTW_ESTIMATE);
    if (!p) {
        fftwf_free(x);
        return false;
    }

    memcpy(x, wave, n*sizeof(float_t));
    fftwf_execute(p);
    mag[0] = jabs(x[0], 0.0) / n;
    phase[0] = jarg(x[0], 0.0);
    for (i = 1; i < n2; i++) {
        mag[i] = jabs(x[i], x[n-i]) / n_2;
        phase[i] = jarg(x[i], x[n-i]);
    }
    if (n > 1) {
        if (n%2 == 0) {
            mag[n2] = jabs(x[n2], 0.0) / n;
            phase[n2] = jarg(x[n2], 0.0);
        } else {
            mag[n2] = jabs(x[n2], x[n-n2]) / n_2;
            phase[n2] = jarg(x[n2], x[n-n2]);
        }
    }
    fftwf_destroy_plan(p);
    fftwf_free(x);

    return true;
}

/* also taken from pure-audio fftw.c */
bool ifft_once_polar(const int n, const float_t *mag, const float_t *phase, float_t *wave) {
    int i, n2 = n/2;
    float_t *x;
    fftwf_plan p;

    x = (float_t *) fftwf_malloc(n*sizeof(float_t));
    if (!x) return false;
    p = fftwf_plan_r2r_1d(n, x, x, FFTW_HC2R, FFTW_ESTIMATE);
    if (!p) {
        fftwf_free(x);
        return false;
    }
    x[0] = mag[0] * cos(phase[0]);
    for (i = 1; i < n2; i++) {
        x[i] = mag[i] * cos(phase[i]) / 2.0;
        x[n-i] = mag[i] * sin(phase[i]) / 2.0;
    }
    x[n2] = mag[n2] * cos(phase[n2]);
    fftwf_execute(p);
    memcpy(wave, x, n*sizeof(float_t));
    fftwf_destroy_plan(p);
    fftwf_free(x);

    return true;
}
