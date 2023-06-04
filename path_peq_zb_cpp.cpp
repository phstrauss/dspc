/*  Audio Signal Processing routines in C/C++
    © Philippe Strauss, 2013  */


#include <cmath>
#include <dspc/path_peq_zb_cpp.hh>
#include <dspc/iir_utils.h>
#include <dspc/iir_zb.h>
#include <genc/exception.hh>
#include <dspc/path_iir_cpp.hh>
#include <dspc/path_vec16_cintr.h>


EXPORT template <typename float_T, direct_form_t form>
biquad_peq_zb_t<float_T, form>
::biquad_peq_zb_t(int nchan) : biquad_t<float_T, form>(nchan) {}

EXPORT template <typename float_T, direct_form_t form>
biquad_peq_zb_t<float_T, form>
::~biquad_peq_zb_t(void) {}


EXPORT template <typename float_T, direct_form_t form>
void biquad_peq_zb_t<float_T, form>
::set_zb_params(double _fc, double _fs, double _q, double _gdb) {
    fc = _fc; fs = _fs; q = _q; gdb = _gdb;

    double bw = bw_from_q(fc, q);
    double gp = pow(10.0, gdb / 20.0);

    hp = gp-1.0;
    double omega_c = omega_norm(fc, fs);
    double omega_w = omega_norm(bw, fs);
    double d = -cos(omega_c);

    double axd;
    if (gp > 1.0) {
        axd = peq_zb_ax(omega_w, 1.0);
    } else {
        axd = peq_zb_ax(omega_w, gp);
    }

    float_T ax = axd;
    float_T cmid = d * (1.0 - axd);
    /* b_num[0] = -ax ;
    b_num[1] = cmid ;
    b_num[2] = 1.0 ;
    a_denom[0] = 1.0 ; // trivial & unused
    a_denom[1] = -cmid ;
    a_denom[2] = ax ; */
    set_coeffs(-ax, cmid, 1.0, 1.0, -cmid, ax);
}

EXPORT template <typename float_T, direct_form_t form>
void biquad_peq_zb_t<float_T, form>
::set_zb_fs(double _fs) {
    fs = _fs;
    set_zb_params(fc, fs, q, gdb);
}

EXPORT template <typename float_T, direct_form_t form>
void biquad_peq_zb_t<float_T, form>
::set_zb_fc(double _fc) {
    fc = _fc;
    set_zb_params(fc, fs, q, gdb);
}

EXPORT template <typename float_T, direct_form_t form>
void biquad_peq_zb_t<float_T, form>
::set_zb_q(double _q) {
    q = _q;
    set_zb_params(fc, fs, q, gdb);
}

EXPORT template <typename float_T, direct_form_t form>
void biquad_peq_zb_t<float_T, form>
::set_zb_gdb(double _gdb) {
    gdb = _gdb;
    set_zb_params(fc, fs, q, gdb);
}


/*
INLINE void peq_zb_peak_process(biquad_coeffs_c_t *coeffs, float hp, float *z, float *in, float *out, int l) {
    int i;

    assert(l % 16 == 0);

    cintr_vec16_copy(in, out, l);
    c_iir2_df2_float(coeffs->b_num, coeffs->a_denom, z, out, out, l);
    for (i = 0; i < l; i += 16) {
        _cintr_vec16_vv1_sub(out+i, in+i);
        _cintr_vec16_cv_mul(out+i, 0.5 * hp);
        _cintr_vec16_vv2_add(in+i, out+i);
    }
}
*/

EXPORT template <typename float_T, direct_form_t form>
void biquad_peq_zb_t<float_T, form>
::process(struct biquad_coeffs<float_T> *pcoeffs, int chan, float *samples, int len) {
	throw exception_t("exception_t", "in-place samples unimplemented for ZB PEQ", __FILE__, "biquad_peq_zb_t<float_T, form>::process_inner", __LINE__);
}

EXPORT template <typename float_T, direct_form_t form>
void inline biquad_peq_zb_t<float_T, form>
::process(struct biquad_coeffs<float_T> *pcoeffs, int chan, /*const*/ float *in, float *out, int len) {
    int i;

    assert(len % 16 == 0);

    cintr_vec16_copy(in, out, len);
    if (form == direct_form_t::df2)
    	cpp_iir2_df2<float_T, float>(pcoeffs->b_num, pcoeffs->a_denom, z1[chan], out, out, len);
    else
 		cpp_iir2_df1<float_T, float>(pcoeffs->b_num, pcoeffs->a_denom, z1[chan], z2[chan], out, out, len);

    for (i = 0; i < len; i += 16) {
        _cintr_vec16_vv1_sub(out+i, in+i);
        _cintr_vec16_cv_mul(out+i, 0.5 * hp);
        _cintr_vec16_vv2_add(in+i, out+i);
    }
}
