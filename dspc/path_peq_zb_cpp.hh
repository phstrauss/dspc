/*  Audio Signal Processing routines in C/C++
    © Philippe Strauss, 2013  */


#ifndef _SPS_PATH_PEQ_ZB_CPP_HH
#define _SPS_PATH_PEQ_ZB_CPP_HH


#include <dspc/path_biquad_cpp.hh>


EXPORT template <typename float_T, direct_form_t form>
class biquad_peq_zb_t : public biquad_t<float_T, form> {

public:

    biquad_peq_zb_t(int nchan);
    ~biquad_peq_zb_t();
    void set_zb_params(double fc, double fs, double q, double gdb);
    void set_zb_fs(double fs);
    void set_zb_fc(double fc);
    void set_zb_q(double q);
    void set_zb_gdb(double gdb);
	void inline process(struct biquad_coeffs<float_T> *pcoeffs, int chan, float *samples, int len); // throws
	void inline process(struct biquad_coeffs<float_T> *pcoeffs, int chan, /*const*/ float *in, float *out, int len);

private:

	double fc, fs, q, gdb;
    double hp;
	using biquad_t<float_T, form>::z1;
	using biquad_t<float_T, form>::z2;

};


#ifdef NEED_TEMPLATE_DEFINITIONS
    #include <dspc/path_peq_zb_cpp.cpp>
#endif

#endif
