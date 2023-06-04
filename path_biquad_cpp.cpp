/*  Audio Signal Processing routines in C/C++
    © Philippe Strauss, 2013  */


#include <dspc/path_biquad_cpp.hh>
#include <dspc/path_iir_cpp.hh>
#include <dspc/mem.hh>
#include <dspc/exception.hh>
#include <unistd.h> // usleep
#include <dspc/perr.h>
#include <pthread.h>


EXPORT template <typename float_T, direct_form_t form>
biquad_t<float_T, form>
::biquad_t(int _nchan) {
	nchan = _nchan;
	int ret;

	z1 = (float_T **) ALLOC_SIMPLE_MATRIX(float_T, nchan, 3);
	if (form == direct_form_t::df1)
		z2 = (float_T **) ALLOC_SIMPLE_MATRIX(float_T, nchan, 3);
	else
		z2 = NULL;

	if ( (ret = pthread_mutex_init(&mx1, NULL)) ) {
		snprintf(err, ERR_LEN, "pthread_mutex_init mx1 failure pthread err=%d", ret);
		throw exception_t("exception_t", err, __FILE__, "biquad_t::biquad_t", __LINE__);
	}
    if ( (ret = pthread_mutex_init(&mx2, NULL)) ) {
		snprintf(err, ERR_LEN, "pthread_mutex_init mx2 failure pthread err=%d", ret);
		throw exception_t("exception_t", err, __FILE__, "biquad_t::biquad_t", __LINE__);
    }
}

EXPORT template <typename float_T, direct_form_t form>
biquad_t<float_T, form>
::~biquad_t() {
	int ret;

	FREE_SIMPLE_MATRIX(float_T, z1, nchan);
	if (form == direct_form_t::df1)
		FREE_SIMPLE_MATRIX(float_T, z2, nchan);

	if ( (ret = pthread_mutex_destroy(&mx1)) ) {
		snprintf(err, ERR_LEN, "pthread_mutex_destroy mx1 failure pthread err=%d", ret);
		throw exception_t("exception_t", err, __FILE__, "biquad_t::~biquad_t", __LINE__);
	}
    if ( (ret = pthread_mutex_destroy(&mx2)) ) {
		snprintf(err, ERR_LEN, "pthread_mutex_destroy mx2 failure pthread err=%d", ret);
		throw exception_t("exception_t", err, __FILE__, "biquad_t::~biquad_t", __LINE__);
    }
}

EXPORT template <typename float_T, direct_form_t form>
void biquad_t<float_T, form>
::zzero(float_T **z) {
	int i;

	for(i = 0; i < nchan; ++i) {
		z[i][0] = 0.0; z[i][1] = 0.0; z[i][2] = 0.0;
	}
}

EXPORT template <typename float_T, direct_form_t form>
void biquad_t<float_T, form>
::set_coeffs(float_T b0, float_T b1, float_T b2, float_T a0, float_T a1, float_T a2) {
    int ret1, ret2;
    // FIX: z delay line locking

    /* backup coeffs */
    while ( (ret1 = pthread_mutex_trylock(&mx2)) == EBUSY ) {
        usleep(250); dprintf("biquad_t::set_coeffs (backup): spinning...\n");
    }
    if (ret1 == 0) {
        coeffs2.b_num[0] = b0; coeffs2.b_num[1] = b1; coeffs2.b_num[2] = b2;
        coeffs2.a_denom[0] = a0; coeffs2.a_denom[1] = a1; coeffs2.a_denom[2] = a2;
        zzero(z1);
        if (form == direct_form_t::df1)
        	zzero(z2);
        if ( (ret2 = pthread_mutex_unlock(&mx2)) != 0 ) {
        	snprintf(err, ERR_LEN, "pthread_mutex_unlock failed, errcode=%d\n", ret2);
        	throw exception_t("exception_t", err, __FILE__, "biquad_t::set_coeffs", __LINE__);
        }
    } else if (ret1 == EINVAL) {
        // FIX: free ressources
        throw exception_t("exception_t", "pthread_mutex_trylock failed, returned EINVAL", __FILE__, "biquad_t::set_coeffs", __LINE__);
    }

    /* primary coeffs */
    while ( (ret1 = pthread_mutex_trylock(&mx1)) == EBUSY ) {
        usleep(250); dprintf("biquad_t::set_coeffs (primary): spinning...\n");
    }
    if (ret1 == 0) {
        coeffs1.b_num[0] = b0; coeffs1.b_num[1] = b1; coeffs1.b_num[2] = b2;
        coeffs1.a_denom[0] = a0; coeffs1.a_denom[1] = a1; coeffs1.a_denom[2] = a2;
        // zeroed twice - but locking ok at least
        zzero(z1);
        if (form == direct_form_t::df1)
        	zzero(z2);
        if ( (ret2 = pthread_mutex_unlock(&mx1)) != 0 ) {
        	snprintf(err, ERR_LEN, "pthread_mutex_unlock failed, errcode=%d\n", ret2);
        	throw exception_t("exception_t", err, __FILE__, "biquad_t::set_coeffs", __LINE__);
        }
    } else if (ret1 == EINVAL) {
        // FIX: free ressources
        throw exception_t("exception_t", "pthread_mutex_trylock failed, returned EINVAL", __FILE__, "biquad_t::set_coeffs", __LINE__);
    }
}

EXPORT template <typename float_T, direct_form_t form>
void inline biquad_t<float_T, form>
::process_inner(struct biquad_coeffs<float_T> *pcoeffs, int chan, float *samples, int len) {
    if (form == direct_form_t::df2)
        cpp_iir2_df2<float_T, float>(pcoeffs->b_num, pcoeffs->a_denom, z1[chan], samples, samples, len);
    else
        cpp_iir2_df1<float_T, float>(pcoeffs->b_num, pcoeffs->a_denom, z1[chan], z2[chan], samples, samples, len);
}

EXPORT template <typename float_T, direct_form_t form>
void inline biquad_t<float_T, form>
::process_inner(struct biquad_coeffs<float_T> *pcoeffs, int chan, const float *in, float *out, int len) {
    if (form == direct_form_t::df2)
        cpp_iir2_df2<float_T, float>(pcoeffs->b_num, pcoeffs->a_denom, z1[chan], in, out, len);
    else
        cpp_iir2_df1<float_T, float>(pcoeffs->b_num, pcoeffs->a_denom, z1[chan], z2[chan], in, out, len);
}

EXPORT template <typename float_T, direct_form_t form>
void biquad_t<float_T, form>
::process(int chan, float *samples, int len) {
	struct biquad_coeffs<float_T> *pcoeffs;
	int ret, ret2, retu;

	try {

    if((ret = pthread_mutex_trylock(&mx1)) == EBUSY) {
        dprintf("biquad_t::process: mutex_trylock: reverting to backup IIR coeffs\n");
        if ((ret2 = pthread_mutex_trylock(&mx2)) == 0)
            pcoeffs = &coeffs2;
    } else if (ret == 0) {
        pcoeffs = &coeffs1;
    }
    if (ret == 0 || (ret == EBUSY && ret2 == 0)) {
        /* ***** PROCESS_INNER here ***** */
        process_inner(pcoeffs, chan, samples, len);			
    } else {
        dprintf("biquad_t::process: out of luck, both primary and backup IIR coeffs locked, this frame left unfiltered\n");
        // NB: inplace: do nothing, in/out: copy
    }

    // unlock mutex
    if (ret == 0) {
        if( (retu = pthread_mutex_unlock(&mx1)) ) {
        	snprintf(err, ERR_LEN, "mutex_unlock mx1 error: %d\n", retu);
            throw exception_t("exception_t", err, __FILE__, "biquad_t::process", __LINE__);
            // FIX: mute/blank output
        }
    } else if (ret == EBUSY && ret2 == 0) {
        if( (retu = pthread_mutex_unlock(&mx2)) ) {
        	snprintf(err, ERR_LEN, "mutex_unlock mx2 error: %d\n", retu);
            throw exception_t("exception_t", err, __FILE__, "biquad_t::process", __LINE__);
        }
    }

	} catch (exception_t &e) {
		// TODO: blank samples
		e.display();
	}
}

EXPORT template <typename float_T, direct_form_t form>
void biquad_t<float_T, form>
::process(int chan, const float *in, float *out, int len) {
    struct biquad_coeffs<float_T> *pcoeffs;
    int ret, ret2, retu;

    try {

    if( (ret = pthread_mutex_trylock(&mx1)) == EBUSY ) {
        dprintf("biquad_t::process: mutex_trylock: reverting to backup IIR coeffs\n");
        if ((ret2 = pthread_mutex_trylock(&mx2)) == 0)
            pcoeffs = &coeffs2;
    } else if (ret == 0) {
        pcoeffs = &coeffs1;
    }
    if ( ret == 0 || (ret == EBUSY && ret2 == 0) ) {
        /* ***** PROCESS_INNER here ***** */
        process_inner(pcoeffs, chan, in, out, len);         
    } else {
        dprintf("biquad_t::process: out of luck, both primary and backup IIR coeffs locked, this frame left unfiltered\n");
        // NB: inplace: do nothing, in/out: copy
    }

    // unlock mutex
    if (ret == 0) {
        if( (retu = pthread_mutex_unlock(&mx1)) ) {
            snprintf(err, ERR_LEN, "mutex_unlock mx1 error: %d\n", retu);
            throw exception_t("exception_t", err, __FILE__, "biquad_t::process", __LINE__);
            // FIX: mute/blank output
        }
    } else if (ret == EBUSY && ret2 == 0) {
        if( (retu = pthread_mutex_unlock(&mx2)) ) {
            snprintf(err, ERR_LEN, "mutex_unlock mx2 error: %d\n", retu);
            throw exception_t("exception_t", err, __FILE__, "biquad_t::process", __LINE__);
        }
    }

    } catch (exception_t &e) {
        // TODO: blank samples
        e.display();
    }
}
