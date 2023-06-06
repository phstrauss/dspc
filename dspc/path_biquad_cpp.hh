/*  Audio Signal Processing routines in C/C++
    © Philippe Strauss, 2013  */


#ifndef _SPS_PATH_BIQUAD_CPP_HH
#define _SPS_PATH_BIQUAD_CPP_HH


#include <dspc/templating.hh>
#include <cmath>
#include <pthread.h>


#define ERR_LEN 256


/* mantissa : float : 23 bits, double : 52 bits
   see OppSch 2nd ed. p. 413 */
template <typename float_T>
struct biquad_coeffs {
    float_T b_num[3];
    float_T a_denom[3];
};

/* we lose ability to typedef
   typedef struct biquad_coeffs biquad_coeffs_t; */

enum class direct_form {df1, df2};
typedef enum direct_form direct_form_t;


EXPORT template <typename float_T, direct_form_t form>
class biquad_t {

public:

	biquad_t(int nchan);
	~biquad_t();
	void set_coeffs(float_T b0, float_T b1, float_T b2, float_T a0, float_T a1, float_T a2);
	virtual void inline process_inner(struct biquad_coeffs<float_T> *pcoeffs, int chan, float *samples, int len);
	virtual void inline process_inner(struct biquad_coeffs<float_T> *pcoeffs, int chan, const float *in, float *out, int len);
	void process(int chan, float *samples, int len);
	void process(int chan, const float *in, float *out, int len);

private:

	int nchan;
	// xx1, xx2: mthread : using coeffs while being updated :
	// we use two sets of coeffs, primary and backup
	pthread_mutex_t mx1, mx2;
	struct biquad_coeffs<float_T> coeffs1;
	struct biquad_coeffs<float_T> coeffs2;
	// float_T *z1 = nullptr, *z2 = nullptr;
	float_T *z1, *z2;
	char err[ERR_LEN];

	void zzero(float_T **z);

};


#ifdef NEED_TEMPLATE_DEFINITIONS
    #include <dspc/path_biquad_cpp.cpp>
#endif

#endif
