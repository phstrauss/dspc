/* libFLAC - Free Lossless Audio Codec library
 * Copyright (C) 2006,2007  Josh Coalson
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of the Xiph.org Foundation nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <math.h>

#ifndef M_PI
/* math.h in VC++ doesn't seem to have this (how Microsoft is that?) */
#define M_PI 3.14159265358979323846
#endif


void bartlett(float_t *window, const int L)
{
	const int N = L - 1;
	int n;

	if (L & 1) {
		for (n = 0; n <= N/2; n++)
			window[n] = 2.0f * n / (float)N;
		for (; n <= N; n++)
			window[n] = 2.0f - 2.0f * n / (float)N;
	}
	else {
		for (n = 0; n <= L/2-1; n++)
			window[n] = 2.0f * n / (float)N;
		for (; n <= N; n++)
			window[n] = 2.0f - 2.0f * (N-n) / (float)N;
	}
}

void bartlett_hann(float_t *window, const int L)
{
	const int N = L - 1;
	int n;

	for (n = 0; n < L; n++)
		window[n] = (float_t)(0.62f - 0.48f * fabs((float)n/(float)N+0.5f) + 0.38f * cos(2.0f * M_PI * ((float)n/(float)N+0.5f)));
}

void blackman(float_t *window, const int L)
{
	const int N = L - 1;
	int n;

	for (n = 0; n < L; n++)
		window[n] = (float_t)(0.42f - 0.5f * cos(2.0f * M_PI * n / N) + 0.08f * cos(4.0f * M_PI * n / N));
}

/* 4-term -92dB side-lobe */
void blackman_harris_4term_92db_sidelobe(float_t *window, const int L)
{
	const int N = L - 1;
	int n;

	for (n = 0; n <= N; n++)
		window[n] = (float_t)(0.35875f - 0.48829f * cos(2.0f * M_PI * n / N) + 0.14128f * cos(4.0f * M_PI * n / N) - 0.01168f * cos(6.0f * M_PI * n / N));
}

void connes(float_t *window, const int L)
{
	const int N = L - 1;
	const double N2 = (double)N / 2.;
	int n;

	for (n = 0; n <= N; n++) {
		double k = ((double)n - N2) / N2;
		k = 1.0f - k * k;
		window[n] = (float_t)(k * k);
	}
}

void flattop(float_t *window, const int L)
{
	const int N = L - 1;
	int n;

	for (n = 0; n < L; n++)
		window[n] = (float_t)(1.0f - 1.93f * cos(2.0f * M_PI * n / N) + 1.29f * cos(4.0f * M_PI * n / N) - 0.388f * cos(6.0f * M_PI * n / N) + 0.0322f * cos(8.0f * M_PI * n / N));
}

void gauss(float_t *window, const int L, const float_t stddev)
{
	const int N = L - 1;
	const double N2 = (double)N / 2.;
	int n;

	for (n = 0; n <= N; n++) {
		const double k = ((double)n - N2) / (stddev * N2);
		window[n] = (float_t)exp(-0.5f * k * k);
	}
}

void hamming(float_t *window, const int L)
{
	const int N = L - 1;
	int n;

	for (n = 0; n < L; n++)
		window[n] = (float_t)(0.54f - 0.46f * cos(2.0f * M_PI * n / N));
}

void hann(float_t *window, const int L)
{
	const int N = L - 1;
	int n;

	for (n = 0; n < L; n++)
		window[n] = (float_t)(0.5f - 0.5f * cos(2.0f * M_PI * n / N));
}

void kaiser_bessel(float_t *window, const int L)
{
	const int N = L - 1;
	int n;

	for (n = 0; n < L; n++)
		window[n] = (float_t)(0.402f - 0.498f * cos(2.0f * M_PI * n / N) + 0.098f * cos(4.0f * M_PI * n / N) - 0.001f * cos(6.0f * M_PI * n / N));
}

void nuttall(float_t *window, const int L)
{
	const int N = L - 1;
	int n;

	for (n = 0; n < L; n++)
		window[n] = (float_t)(0.3635819f - 0.4891775f*cos(2.0f*M_PI*n/N) + 0.1365995f*cos(4.0f*M_PI*n/N) - 0.0106411f*cos(6.0f*M_PI*n/N));
}

void rectangle(float_t *window, const int L)
{
	int n;

	for (n = 0; n < L; n++)
		window[n] = 1.0f;
}

void triangle(float_t *window, const int L)
{
	int n;

	if (L & 1) {
		for (n = 1; n <= L+1/2; n++)
			window[n-1] = 2.0f * n / ((float)L + 1.0f);
		for (; n <= L; n++)
			window[n-1] = - (float)(2 * (L - n + 1)) / ((float)L + 1.0f);
	}
	else {
		for (n = 1; n <= L/2; n++)
			window[n-1] = 2.0f * n / (float)L;
		for (; n <= L; n++)
			window[n-1] = ((float)(2 * (L - n)) + 1.0f) / (float)L;
	}
}

void tukey(float_t *window, const int L, const float_t p)
{
	if (p <= 0.0)
		rectangle(window, L);
	else if (p >= 1.0)
		hann(window, L);
	else {
		const int Np = (int)(p / 2.0f * L) - 1;
		int n;
		/* start with rectangle... */
		rectangle(window, L);
		/* ...replace ends with hann */
		if (Np > 0) {
			for (n = 0; n <= Np; n++) {
				window[n] = (float_t)(0.5f - 0.5f * cos(M_PI * n / Np));
				window[L-Np-1+n] = (float_t)(0.5f - 0.5f * cos(M_PI * (n+Np) / Np));
			}
		}
	}
}

void welch(float_t *window, const int L)
{
	const int N = L - 1;
	const double N2 = (double)N / 2.;
	int n;

	for (n = 0; n <= N; n++) {
		const double k = ((double)n - N2) / N2;
		window[n] = (float_t)(1.0f - k * k);
	}
}

