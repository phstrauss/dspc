#ifndef _SPS_H
#define _SPS_H


#ifdef __cplusplus
extern "C" {
#endif


#include <dspc/gnuinline.h>
#include <math.h>

#undef M_PIl
#define M_PIl       3.141592653589793238462643383279502884L

/*  the true reason of the existence of this C header file
        recap : mantissa : float : 23 bits, double : 52 bits
    see OppSch 2nd ed. p. 413  */
typedef float float_t;


INLINE unsigned int ipow(const unsigned int base, const unsigned int exp) {
    unsigned int ret = base;

    if (exp == 0)
        return 1;
    for (unsigned int i = 1 ; i < exp; ++i)
        ret *= base;
    return ret;
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
