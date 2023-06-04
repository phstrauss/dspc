/*  Audio Signal Processing routines in C++
    © Philippe Strauss, 2013  */


#include <math.h>
#include <dspc/dspc_misc.h>
#include <dspc/windows.hh>
#include <dspc/dspc.h>
#include <cassert>
#include <cstdio>
#include <dspc/exception.hh>
#include <dspc/mem.hh>


/* seems fine, negatives does not return NAN due to a long int and not a float return */
int fact(long int x) {
    int i;
    long int acc = x;

    if (x > 0) {
        for (i = x - 1; i > 0; --i)
            acc *= i;
    } else if (x < 0) {
        EXN_MSG_THROW("else if (x < 0)", "negative argument");
    } else {
        acc = 1;
    }

    return acc;
}

/* Ref: SASP
   GSL has gsl_sf_bessel_I0 (double X); investigate accuracy */
float_t bessel_I0(float_t x, int accuracy) {
    int i;
    float_t acc = 0.0, x_2 = x / 2.0;

    for (i = 0; i < accuracy; ++i)
        acc += pow ((pow(x_2, (float_t) i) / (float_t) fact(i)), 2.0);

    return acc;
}


/*  vorbis power complementary transition (ref : celt_tasl.pdf)
    len : transition len in samples, n sample index in the transition  */
float_t vorbis_power_complementary(int n, int len) {
    return sin(M_PI * pow(sin(M_PI * (n+1 / 2.0) / 2.0*len), 2.0) / 2.0);
}

float_t gauss(const float_t sigma, int n, int len) {
    return exp(-0.5 * (pow((n-(len-1.0) / 2.0) / (sigma * (len-1.0) / 2.0), 2.0)));
}

void hann_window(float_t *win, int len) {
    int i;
    float_t N = len - 1.0;
    for (i=0; i<len; ++i)
        win[i] = 0.5 * (1.0 - cos(2.0 * M_PI * i / N));
}

void rect_window(float_t *win, int len) {
    int i;
    for (i=0; i<len; ++i)
        win[i] = 1.0;
}


windows_t::windows_t(int _len): len(_len)
{
    N = (float_t) _len - 1.0;
    N2 = ((float_t) _len - 1.0) / 2.0;
    win = (float_t *) CALLOC_EXN(len, sizeof(float_t));
}

windows_t::~windows_t(void) {
    free(win);
}

/*  TODO: dolph-chebychev;
    Ref.: T. Painter and A. Spanias, "Perceptual coding of digital audio",
    Proceedings of the IEEE, vol. 88, pp. 451-513, Apr. 2000  */

/* WW : thread unsafe for now */
void windows_t::init(win_common_t fct) {
    int i;
    int hlen = half(rounding_t::down, len);

    switch (fct) {
        case win_common_t::hann:
            for (i = 0; i < len; ++i)
                win[i] = 0.5 * (1.0 - cos(2.0 * M_PI * i / N));
            break;
        case win_common_t::hann_ampc_gs: // amplitude complementary; synthesis in G&S
            for (i = 0; i < len; ++i)
                win[i] = pow(cos((i-hlen) * M_PI / (2.0 * len)), 2.0);
            break;
        case win_common_t::hamming: // tested
            for (i = 0; i < len; ++i)
                win[i] = 0.54 - 0.46 * cos(2.0 * M_PI * i / N);
            break;
        case win_common_t::blackman: // tested
            for (i = 0; i < len; ++i)
                win[i] = 0.42 - 0.50 * cos ((2 * M_PI * i) / N) + 0.08 * cos ((4 * M_PI * i) / N);
            break;
        // Flac
        case win_common_t::blackman_harris_92db:
            for (i = 0; i < len; ++i) {
                win[i] = (float_t)(0.35875f - 0.48829f * cos(2.0f * M_PI * i / N) + 0.14128f
                         * cos(4.0f * M_PI * i / N) - 0.01168f * cos(6.0f * M_PI * i / N));
            }
            break;
        // Flac
        case win_common_t::bartlett:
        	if (len & 1) {
        		for (i = 0; i <= (len-1)/2; i++)
        			win[i] = 2.0 * i / N;
        		for (; i < len; i++)
        			win[i] = 2.0 - 2.0 * i / N;
            } else {
        		for (i = 0; i <= len/2-1; i++)
        			win[i] = 2.0 * i / N;
        		for (; i < len; i++)
        			win[i] = 2.0 - 2.0 * (N-i) / N;
            }
            break;
        // Flac
        case win_common_t::bartlett_hann:
            for (i = 0; i < len; ++i) {
                win[i] = (float_t)(0.62f - 0.48f * fabs((float)i/N+0.5f) + 0.38f
                         * cos(2.0f * M_PI * ((float)i/N+0.5f)));
            }
            break;
        // Flac
        case win_common_t::connes:
        	for (i = 0; i < len; i++) {
        		double k = ((double)i - N2) / N2;
        		k = 1.0 - k * k;
        		win[i] = (float_t)(k * k);
        	}
            break;
        // Flac
        case win_common_t::flattop:
            for (i = 0; i < len; ++i) {
                win[i] = (float_t)(1.0f - 1.93f * cos(2.0f * M_PI * i / N) + 1.29f
                         * cos(4.0f * M_PI * i / N) - 0.388f * cos(6.0f * M_PI * i / N) + 0.0322f
                         * cos(8.0f * M_PI * i / N));
            }
            break;
        // Flac
        case win_common_t::kaiser_bessel:
            for (i = 0; i < len; ++i) {
                win[i] = (float_t)(0.402f - 0.498f * cos(2.0f * M_PI * i / N) + 0.098f
                         * cos(4.0f * M_PI * i / N) - 0.001f * cos(6.0f * M_PI * i / N));
            }
            break;
        // Flac
        case win_common_t::nuttall:
            for (i = 0; i < len; ++i) {
                win[i] = (float_t)(0.3635819f - 0.4891775f * cos(2.0f*M_PI*i/N)
                         + 0.1365995f*cos(4.0f*M_PI*i/N) - 0.0106411f*cos(6.0f*M_PI*i/N));
            }
            break;
        // Flac
        case win_common_t::welch:
        	for (i = 0; i <= (len-1); i++) {
        		const double k = ((double)i - N2) / N2;
        		win[i] = (float_t)(1.0f - k * k);
        	}
            break;
        // Flac    
        case win_common_t::triangle:
        	if (len & 1) {
        		for (i = 1; i <= len+1/2; i++)
        			win[i-1] = 2.0f * i / ((float)len + 1.0f);
        		for (; i <= len; i++)
        			win[i-1] = - (float)(2 * (len - i + 1)) / ((float)len + 1.0f);
        	}
        	else {
        		for (i = 1; i <= len/2; i++)
        			win[i-1] = 2.0f * i / (float)len;
        		for (; i <= len; i++)
        			win[i-1] = ((float)(2 * (len - i)) + 1.0f) / (float)len;
        	}
            break;
        default:
                EXN_MSG_THROW("windows_t::init", "unreachable code reached");
            break;
    }
}

void windows_t::init_kaiser(const float_t beta, int bessel_iter) { // tested
    int i;
    float_t wn;

    for (i = 0; i < len; ++i) {
        wn = bessel_I0 (
            beta * sqrt (1.0 - pow(2.0 * i / (float_t) len - 1.0, 2.0)), bessel_iter
            ) / bessel_I0 (beta, bessel_iter);

        win[i] = wn;
    }
}

void windows_t::init_gaussian(const float_t sigma) { // tested
    int i;
    float_t wn;

    for (i = 0; i < len; ++i) {
        wn = gauss(sigma, i, len);

        win[i] = wn;

    }
}

// Flac
void windows_t::init_tukey(const float_t p) {
	if (p <= 0.0)
		rect_window(win, len);
	else if (p >= 1.0)
		hann_window(win, len);
	else {
		const int Np = (int)(p / 2.0f * len) - 1;
		int i;
		/* start with rectangle... */
		rect_window(win, len);
		/* ...replace ends with hann */
		if (Np > 0) {
			for (i = 0; i <= Np; i++) {
				win[i] = (float_t)(0.5f - 0.5f * cos(M_PI * i / Np));
				win[len-Np-1+i] = (float_t)(0.5f - 0.5f * cos(M_PI * (i+Np) / Np));
			}
		}
	}    
}

void windows_t::init_sinc(int k) {
    int i;
    float_t wn, x, hlen = len / 2.0;

    for (i = 0; i < len; ++i) {
        x = 2.0 * M_PI * ((float_t) i - hlen) * k / len;
        if (x != 0.0)
            wn = sin(x) / x;
        else
            wn = 1.0;
        win[i] = wn;
    }
}

int windows_t::get_len(void) {
    return len;
}

void windows_t::process(float_t *wave) {
    int i;

    for(i = 0; i < len; ++i)
        wave[i] *= win[i];
}

void windows_t::process(const float_t *in, float_t *out) {
    int i;

    for(i = 0; i < len; ++i)
        out[i] = in[i] * win[i];
}

