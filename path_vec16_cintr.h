/*  block based 1D/vector signal processing, targetting real-time, efficient CPU usage.
    Philippe Strauss, 2011

    comments about resulting code generated: using gcc-4.5.3 or gcc-4.6.1, itself hand compiled to be sure to have
    graphite loop optimizer, requiring parma polyhedra lib and cloog-ppl */


#ifndef _SIGPATH_VEC16_CINTR_H
#define _SIGPATH_VEC16_CINTR_H

#ifdef __cplusplus
extern "C" {
#endif


#include <assert.h>
#include <x86intrin.h>
#include <dspc/gnuinline.h>


INLINE  void cin_copy(float *a, float *b, int l);
_INLINE void _cintr_vec4_copy(float *a, float *b);
_INLINE void _cintr_vec16_copy(float *a, float *b);
INLINE  void cintr_vec16_copy(float *a, float *b, int l);

_INLINE void _cintr_vec4_vv2_mul(float *a, float *b);
_INLINE void _cintr_vec4_vv2_add(float *a, float *b);

_INLINE void _cintr_vec16_vv2_mul(float *a, float *b);
_INLINE void _cintr_vec16_vv2_add(float *a, float *b);
_INLINE void _cintr_vec16_vv1_sub(float *a, float *b);
_INLINE void _cintr_vec16_vv2_sub(float *a, float *b);

INLINE  void cintr_vec16_vv2_mul(float *a, float *b, int l);
INLINE  void cintr_vec16_vv2_add(float *a, float *b, int l);

_INLINE void _cintr_vec16_vsq(float *a);
_INLINE void _cintr_vec16_cv_mul(float *b, const float acst);
_INLINE void _cintr_vec16_cv_add(float *b, const float acst);

INLINE  void cintr_vec16_vsq(float *a, int l);
INLINE  void cintr_vec16_cv_mul(float *b, const float acst, int l);
INLINE  void cintr_vec16_cv_add(float *b, const float acst, int l);


INLINE void cin_copy(float *a, float *b, int l) {
    int i;
    for (i = 0; i < l; ++i) {
        b[i] = a[i];
    }
}

_INLINE void _cintr_vec4_copy(float *a, float *b) {
    __m128 tmp;
    tmp = _mm_load_ps(a);
    _mm_store_ps(b, tmp);
}

_INLINE void _cintr_vec16_copy(float *a, float *b) {
    /* gcc emit perfect asm w this (also for stride 4 variants) */
    __m128 tmp0, tmp1, tmp2, tmp3;

    tmp0 = _mm_load_ps(a);
    tmp1 = _mm_load_ps(a+4);
    tmp2 = _mm_load_ps(a+8);
    tmp3 = _mm_load_ps(a+12);
    _mm_store_ps(b, tmp0);
    _mm_store_ps(b+4, tmp1);
    _mm_store_ps(b+8, tmp2);
    _mm_store_ps(b+12, tmp3);
}

INLINE void cintr_vec16_copy(float *a, float *b, int l) {
    int i;
    assert(l % 16 == 0);
    for (i = 0; i < l; i += 16) {
        _cintr_vec16_copy(a+i, b+i);
    }
}

_INLINE void _cintr_vec4_vv2_mul(float *a, float *b) {
    __m128 prod;
    __m128 tmpb = _mm_load_ps(b);
    prod = _mm_mul_ps(tmpb, _mm_load_ps(a));
    _mm_store_ps(b, prod);
}

_INLINE void _cintr_vec4_vv2_add(float *a, float *b) {
    __m128 prod;
    __m128 tmpb = _mm_load_ps(b);
    prod = _mm_add_ps(tmpb, _mm_load_ps(a));
    _mm_store_ps(b, prod);
}

_INLINE void _cintr_vec16_vv2_mul(float *a, float *b) {
    /* gcc emit perfect asm w this (also for stride 4 variants) */
    __m128 prod0, prod1, prod2, prod3;

    __m128 tmpb0 = _mm_load_ps(b);
    __m128 tmpb1 = _mm_load_ps(b+4);
    __m128 tmpb2 = _mm_load_ps(b+8);
    __m128 tmpb3 = _mm_load_ps(b+12);
    /* when using a temp for a elements, gcc do not merge loading and mulps
       when writing _load_ps without temp as an _mul_ps arg, it does merge loading w mulps */
    prod0 = _mm_mul_ps(tmpb0, _mm_load_ps(a));
    prod1 = _mm_mul_ps(tmpb1, _mm_load_ps(a+4));
    prod2 = _mm_mul_ps(tmpb2, _mm_load_ps(a+8));
    prod3 = _mm_mul_ps(tmpb3, _mm_load_ps(a+12));
    _mm_store_ps(b, prod0);
    _mm_store_ps(b+4, prod1);
    _mm_store_ps(b+8, prod2);
    _mm_store_ps(b+12, prod3);
}

_INLINE void _cintr_vec16_vv2_add(float *a, float *b) {
    __m128 sum0, sum1, sum2, sum3;

    __m128 tmpb0 = _mm_load_ps(b);
    __m128 tmpb1 = _mm_load_ps(b+4);
    __m128 tmpb2 = _mm_load_ps(b+8);
    __m128 tmpb3 = _mm_load_ps(b+12);
    sum0 = _mm_add_ps(tmpb0, _mm_load_ps(a));
    sum1 = _mm_add_ps(tmpb1, _mm_load_ps(a+4));
    sum2 = _mm_add_ps(tmpb2, _mm_load_ps(a+8));
    sum3 = _mm_add_ps(tmpb3, _mm_load_ps(a+12));
    _mm_store_ps(b, sum0);
    _mm_store_ps(b+4, sum1);
    _mm_store_ps(b+8, sum2);
    _mm_store_ps(b+12, sum3);
}

/* second (b) - first (a); store in first (a) */
_INLINE void _cintr_vec16_vv1_sub(float *a, float *b) {
    __m128 diff0, diff1, diff2, diff3;

    __m128 tmpb0 = _mm_load_ps(b);
    __m128 tmpb1 = _mm_load_ps(b+4);
    __m128 tmpb2 = _mm_load_ps(b+8);
    __m128 tmpb3 = _mm_load_ps(b+12);
    /* beware intel vs. att order */
    diff0 = _mm_sub_ps(tmpb0, _mm_load_ps(a));
    diff1 = _mm_sub_ps(tmpb1, _mm_load_ps(a+4));
    diff2 = _mm_sub_ps(tmpb2, _mm_load_ps(a+8));
    diff3 = _mm_sub_ps(tmpb3, _mm_load_ps(a+12));
    _mm_store_ps(a, diff0);
    _mm_store_ps(a+4, diff1);
    _mm_store_ps(a+8, diff2);
    _mm_store_ps(a+12, diff3);
}

/* second (b) - first (a) store in second (b) */
_INLINE void _cintr_vec16_vv2_sub(float *a, float *b) {
    __m128 diff0, diff1, diff2, diff3;

    __m128 tmpb0 = _mm_load_ps(b);
    __m128 tmpb1 = _mm_load_ps(b+4);
    __m128 tmpb2 = _mm_load_ps(b+8);
    __m128 tmpb3 = _mm_load_ps(b+12);
    /* beware intel vs. att order */
    diff0 = _mm_sub_ps(tmpb0, _mm_load_ps(a));
    diff1 = _mm_sub_ps(tmpb1, _mm_load_ps(a+4));
    diff2 = _mm_sub_ps(tmpb2, _mm_load_ps(a+8));
    diff3 = _mm_sub_ps(tmpb3, _mm_load_ps(a+12));
    _mm_store_ps(b, diff0);
    _mm_store_ps(b+4, diff1);
    _mm_store_ps(b+8, diff2);
    _mm_store_ps(b+12, diff3);
}

INLINE void cintr_vec16_vv2_mul(float *a, float *b, int l) {
    int i;
    assert(l % 16 == 0);
    for (i = 0; i < l; i += 16) {
        _cintr_vec16_vv2_mul(a+i, b+i);
    }
}

INLINE void cintr_vec16_vv2_add(float *a, float *b, int l) {
    int i;
    assert(l % 16 == 0);
    for (i = 0; i < l; i += 16) {
        _cintr_vec16_vv2_add(a+i, b+i);
    }
}

_INLINE void _cintr_vec16_vsq(float *a) {
    __m128 prod0, prod1, prod2, prod3;

    __m128 tmpa0 = _mm_load_ps(a);
    __m128 tmpa1 = _mm_load_ps(a+4);
    __m128 tmpa2 = _mm_load_ps(a+8);
    __m128 tmpa3 = _mm_load_ps(a+12);
    prod0 = _mm_mul_ps(tmpa0, tmpa0);
    prod1 = _mm_mul_ps(tmpa1, tmpa1);
    prod2 = _mm_mul_ps(tmpa2, tmpa2);
    prod3 = _mm_mul_ps(tmpa3, tmpa3);
    _mm_store_ps(a, prod0);
    _mm_store_ps(a+4, prod1);
    _mm_store_ps(a+8, prod2);
    _mm_store_ps(a+12, prod3);
}

_INLINE void _cintr_vec16_cv_mul(float *b, const float acst) {
    __m128 prod0, prod1, prod2, prod3;

    __m128 cst = _mm_set1_ps(acst);
    /* could be written with temps for _load_ps with no changes in generated asm */
    prod0 = _mm_mul_ps(cst, _mm_load_ps(b));
    prod1 = _mm_mul_ps(cst, _mm_load_ps(b+4));
    prod2 = _mm_mul_ps(cst, _mm_load_ps(b+8));
    prod3 = _mm_mul_ps(cst, _mm_load_ps(b+12));
    _mm_store_ps(b, prod0);
    _mm_store_ps(b+4, prod1);
    _mm_store_ps(b+8, prod2);
    _mm_store_ps(b+12, prod3);
}

/* may be usefull for D.C. removal, offseting */
_INLINE void _cintr_vec16_cv_add(float *b, const float acst) {
    __m128 sum0, sum1, sum2, sum3;

    __m128 cst = _mm_set1_ps(acst);
    /* could be written with temps for _load_ps with no changes in generated asm */
    sum0 = _mm_add_ps(cst, _mm_load_ps(b));
    sum1 = _mm_add_ps(cst, _mm_load_ps(b+4));
    sum2 = _mm_add_ps(cst, _mm_load_ps(b+8));
    sum3 = _mm_add_ps(cst, _mm_load_ps(b+12));
    _mm_store_ps(b, sum0);
    _mm_store_ps(b+4, sum1);
    _mm_store_ps(b+8, sum2);
    _mm_store_ps(b+12, sum3);
}

INLINE void cintr_vec16_vsq(float *a, int l) {
    int i;
    assert(l % 16 == 0);
    for (i = 0; i < l; i += 16) {
        _cintr_vec16_vsq(a+i);
    }
}

INLINE void cintr_vec16_cv_mul(float *b, const float acst, int l) {
    int i;
    __m128 prod0, prod1, prod2, prod3;

    __m128 cst = _mm_set1_ps(acst);
    assert(l % 16 == 0);
    for (i = 0; i < l; i += 16) {
        prod0 = _mm_mul_ps(cst, _mm_load_ps(b+i));
        prod1 = _mm_mul_ps(cst, _mm_load_ps(b+i+4));
        prod2 = _mm_mul_ps(cst, _mm_load_ps(b+i+8));
        prod3 = _mm_mul_ps(cst, _mm_load_ps(b+i+12));
        _mm_store_ps(b+i, prod0);
        _mm_store_ps(b+i+4, prod1);
        _mm_store_ps(b+i+8, prod2);
        _mm_store_ps(b+i+12, prod3);
    }
}

INLINE void cintr_vec16_cv_add(float *b, const float acst, int l) {
    int i;
    __m128 sum0, sum1, sum2, sum3;

    __m128 cst = _mm_set1_ps(acst);
    assert(l % 16 == 0);
    for (i = 0; i < l; i += 16) {
        sum0 = _mm_add_ps(cst, _mm_load_ps(b+i));
        sum1 = _mm_add_ps(cst, _mm_load_ps(b+i+4));
        sum2 = _mm_add_ps(cst, _mm_load_ps(b+i+8));
        sum3 = _mm_add_ps(cst, _mm_load_ps(b+i+12));
        _mm_store_ps(b+i, sum0);
        _mm_store_ps(b+i+4, sum1);
        _mm_store_ps(b+i+8, sum2);
        _mm_store_ps(b+i+12, sum3);
    }
}

/* plain SSE, SSE1 downto here */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
