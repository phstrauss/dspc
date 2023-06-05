/* nice resource found afterward: oprofile's ./libop/op_hw_specific.h */

#ifndef _SPS_CPUINFO_H
#define _SPS_CPUINFO_H

#ifdef __cplusplus
extern "C" {
#endif


#include <sys/types.h>
#include <stdint.h>             // int64_t
#include <stdbool.h>
#include <dspc/gnuinline.h>


// from ocaml perfcount.c, return int rather than double
#if defined(__GNUC__)
    typedef uint64_t count_64_t;
    // RDTSC puts the result in EAX and EDX. We tell gcc to use those registers
    // for "low" and "high"
#if defined(__i386__) || defined(__x86_64__)
#define GETCOUNTER(low, high) \
            __asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
#else
#define GETCOUNTER(low, high) \
            printf ("Reading of performance counters is supported only on Intel x86\n"); \
            exit(1);
#endif
#else
    // Microsoft Visual Studio
    //#define count_64_t __int64
    typedef __uint64 count_64_t;
#define inline   __inline
#define GETCOUNTER(low,high) __asm { \
    __asm rdtsc \
    __asm mov low, eax \
    __asm mov high, edx };
#endif


/* ***** was in cpuid.h ***** */

#if defined(__i386__) && defined(__PIC__)

/* %ebx may be the PIC register.  */
#define CPUID(level, a, b, c, d) \
__asm__ ("xchg %%ebx, %1 ; " \
    "cpuid ; " \
    "xchg %%ebx, %1 ; " \
    : "=a" (a), "=r" (b), "=c" (c), "=d" (d) \
    : "0" (level))

#else

#define CPUID(level, a, b, c, d) \
__asm__ ("cpuid" \
    : "=a" (a), "=b" (b), "=c" (c), "=d" (d) \
    : "0" (level))

#endif

// EDX fct 1
#define bit_CMPXCHG8B (1 << 8)
#define bit_CMOV (1 << 15)
#define bit_CLFLUSH (1 << 19)
#define bit_MMX (1 << 23)
#define bit_FXSR (1 << 24)
#define bit_SSE (1 << 25)
#define bit_SSE2 (1 << 26)
// ECX fct 1
#define bit_SSE3 (1 << 0)
#define bit_PCMULDQ (1 << 1)
#define bit_SSSE3 (1 << 9)
#define bit_FMA (1 << 12)
#define bit_CMPXCHG16B (1 << 13)
#define bit_SSE41 (1 << 19)
#define bit_SSE42 (1 << 20)
#define bit_XSAVE (1 << 26)
#define bit_AVX (1 << 28)
// EDX ext
#define bit_3DNOW (1 << 31)
#define bit_3DNOWP (1 << 30)
#define bit_LM (1 << 29)

/* ***** END was in cpuid.h ***** */

/* ***** was in perfcount.c ***** */

// fairly ocaml specific :
    INLINE unsigned long sample_perfcnt_20(void);
    INLINE unsigned long sample_perfcnt_10(void);

/*
 * A module that allows the reading of performance counters on Pentium.
 *
 * This file contains both code that uses the performance counters to 
 * compute the number of cycles per second (to be used during ./configure) 
 * and also code to read the performance counters from Ocaml.
 *
 * Author: George Necula (necula@cs.berkeley.edu)
 */

/* Philippe Strauss: took perfcount.c from CPC/ocaml and stripped / adapted it for this small lib */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* sm: I want a version that is as fast as possible, dropping
 * bits that aren't very important to achieve it. *
 *
 * This version drops the low 20 bits and the high 14 bits so the
 * result is 30 bits (always a positive Ocaml int); this yields
 * megacycles, which for GHz machines will be something like
 * milliseconds. */
    INLINE unsigned long sample_perfcnt_20(void) {
        unsigned long pclow, pchigh;

        GETCOUNTER(pclow, pchigh);

        return ((pclow >> 20) | (pchigh << 12)) & 0x3FFFFFFF;
    }

/* This version drops the low 10 bits, yielding something like
 * microseconds. */
    INLINE unsigned long sample_perfcnt_10(void) {
        unsigned long pclow, pchigh;

        GETCOUNTER(pclow, pchigh);

        return ((pclow >> 10) | (pchigh << 22)) & 0x3FFFFFFF;
    }

/* ***** END was in perfcount.c ***** */


    struct x86_threading {
        char vendor_id[13];
        int logical;
        bool hyper_threading;
    };
    typedef struct x86_threading x86_threading_t;

    struct cpu_info {
        // EDX 1
        bool cmpxchg8b;
        bool cmov;
        bool clflush;
        bool mmx;
        bool fxsr;
        bool sse;
        bool sse2;
        // ECX 1
        bool sse3;
        bool pcmuldq;
        bool ssse3;
        bool fmac;
        bool cmpxchg16b;
        bool sse41;
        bool sse42;
        bool xsave;
        bool avx;
        // EDX ext
        bool simd_3dnow;
        bool simd_3dnowp;
        bool longmode;
        // EAX 1
        uint32_t signature;
        x86_threading_t thread;
        int ncores;             // __DARWIN__ sysctl
        double cpufrequency;    // __DARWIN__ sysctl
        double cpufrequency_measured;
        int64_t cachelinesize;  // __DARWIN__ sysctl
    };
    typedef struct cpu_info cpu_info_t;


// cpuninfo.c
    bool mhz(double *);
    bool threading(x86_threading_t *);
    int cpuinfo(cpu_info_t *);
    void pprint_cpuinfo(cpu_info_t *);


#ifdef __cplusplus
}                               /* extern "C" */
#endif

#endif
