/*  Audio Signal Processing routines in C++
    © Philippe Strauss, 2013  */


#ifndef _SPS_MISC_H
#define _SPS_MISC_H


#ifdef __cplusplus
extern "C" {
#endif


#include <dspc/dspc.h>
#include <math.h>


/* a floor for float, using an epsilon */
INLINE int floor_epsilon(const float_t x, const float_t epsilon) {
    double up = x + epsilon;

    return (int) floor(up);
}

INLINE int ceil_epsilon(const float_t x, const float_t epsilon) {
    double down = x - epsilon;

    return (int) ceil(down);
}


enum class rounding {up, down};
typedef enum rounding rounding_t;

INLINE int half(rounding_t round, int n) {
    if (n % 2 == 0)
        return n/2;
    else if (round == rounding_t::up)
        return (n+1)/2;
    else if (round == rounding_t::down)
        return (n-1)/2;
    // make compiler happy:
    return n/2;
}

INLINE void zeros(float_t *buffer, int from, int to) {
    int i;
    for (i = from; i <= to; ++i)
        buffer[i] = 0.0;
}

// unsafe
INLINE void vec_blit(float_t *dst, float_t *src, int offset_dst, int offset_src, int len) {
    int i;
    for (i = 0; i < len; ++i)
        dst[offset_dst + i] = src[offset_src + i];
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
