// see http://peter.kuscsik.com/drupal/

#include <stdio.h>
#include <stdint.h>
// inline mess require GCC and not clang to compile & link
// update 20211020: moved read_perfcnt from .h to this .c file. Builds now with clang
#include <dspc/gnuinline.h>
#include <dspc/cpuinfo.h>
// sysctl
#include <sys/types.h>
#if defined(__DARWIN__)
#    include <sys/sysctl.h>
#endif
// malloc
#include <stdlib.h>

// cpu MHz probing taken from ocaml and cpc distrib perfcount.c
#include <sys/times.h>
#include <unistd.h>
#include <math.h>
#include <dspc/perr.h>

// x86_threading
#include <string.h>
#ifdef __linux__
#include <strings.h>
#endif
#include <math.h>


#define NTIMES 30000000


/* This is the function that actually reads the performance counter. */
count_64_t read_perfcnt(void) {
    unsigned long pclow, pchigh;
    count_64_t lowhigh;

     GETCOUNTER(pclow, pchigh);

    // Put the 64-bit value together
     lowhigh = ((count_64_t) pclow) | ((count_64_t) pchigh << 32);

     return lowhigh;
}

bool mhz(double *ret) {
    struct tms t;
    clock_t start, finish, diff;
    count_64_t start_ticks, finish_ticks, diff_ticks;
    double cycles_per_usec;

    long clk_per_sec = sysconf(_SC_CLK_TCK);

    if (clk_per_sec <= 0) {
        eprintf("mhz(): cannot find clk_per_sec (got %ld)\n", clk_per_sec);
        return false;
    }

    times(&t);
    start = t.tms_utime;
    start_ticks = read_perfcnt();
    // Do something for a while
    {
        int i;
        double a = 5.678;
        for (i = 0; i < NTIMES; i++)
            a = (i & 1) ? (a * a) : (sqrt(a));
    }
    times(&t);
    finish = t.tms_utime;
    finish_ticks = read_perfcnt();
    diff = finish - start;
    diff_ticks = finish_ticks - start_ticks;
    if (diff == 0) {
        eprintf("mhz(): cannot use Unix.times\n");
        return false;
    }
    if (diff_ticks == 0) {
        eprintf("mhz(): invalid result from the peformance counters\n");
        return false;
    }
    diff_ticks /= 1000000;      // We care about cycles per microsecond

    cycles_per_usec =
        (((double) diff_ticks / (double) diff) * (double) clk_per_sec);

    /* Whatever value we print here will be used as the CYCLES_PER_USEC
     * below */
    dprintf("mhz(): cycles per usec: %.3lf\n", cycles_per_usec);
    *ret = cycles_per_usec;
    return true;
}

bool threading(x86_threading_t * ret) {
    uint32_t eax, ebx, ecx, edx;
    char vendor[12], cpu_vendor[13];
    uint32_t logical;
    bool wentok = false;

    bzero(vendor, 12);
    bzero(cpu_vendor, 13);
    bzero(ret->vendor_id, 13);

    CPUID(0, eax, ebx, ecx, edx);
    // using -O2 : attention : déréférencement du pointeur type-punned brisera
    // l'aliasing strict [-Wstrict-aliasing]
    ((uint32_t *) vendor)[0] = ebx;
    ((uint32_t *) vendor)[1] = edx;
    ((uint32_t *) vendor)[2] = ecx;
    strncat(cpu_vendor, vendor, 12);
    strncat(ret->vendor_id, vendor, 12);
    dprintf("threading(): CPU vendor ID: %s\n", cpu_vendor);

    CPUID(1, eax, ebx, ecx, edx);
    // nice ops precedences
    if (strncmp(cpu_vendor, "GenuineIntel", 12) == 0 && edx & (1 << 28)) {  /* HTT bit */
        logical = (ebx >> 16) & 0xff;   // EBX[23:16]
        //CPUID(4, eax, ebx, ecx, edx);
        // BUG : result not reliable it seems
        //cores = ((eax >> 26) & 0x3f) + 1; // EAX[31:26] + 1
        ret->logical = logical;
        dprintf("threading(): %u logical CPU(s)\n", logical);
        wentok = true;
    }
    return wentok;
}


void _features_init(cpu_info_t * feat) {
    // EDX 1
    feat->cmpxchg8b = false;
    feat->cmov = false;
    feat->clflush = false;
    feat->mmx = false;
    feat->fxsr = false;
    feat->sse = false;
    feat->sse2 = false;
    // ECX 1
    feat->sse3 = false;
    feat->pcmuldq = false;
    feat->ssse3 = false;
    feat->fmac = false;
    feat->cmpxchg16b = false;
    feat->sse41 = false;
    feat->sse42 = false;
    feat->xsave = false;
    feat->avx = false;
    // EDX ext
    feat->simd_3dnow = false;
    feat->simd_3dnowp = false;
    feat->longmode = false;
    feat->ncores = 0;
    feat->cpufrequency_measured = 0;
    feat->cpufrequency = 0;
    feat->cachelinesize = 0;
}

void _cpuid_simd(cpu_info_t * feat) {
    uint32_t eax, ebx, ecx, edx;
    uint32_t ext_level;

    CPUID(1, eax, ebx, ecx, edx);
    // EDX
    feat->cmpxchg8b = edx & bit_CMPXCHG8B;
    feat->cmov = edx & bit_CMOV;
    feat->clflush = edx & bit_CLFLUSH;
    feat->mmx = edx & bit_MMX;
    feat->fxsr = edx & bit_FXSR;
    feat->sse = edx & bit_SSE;
    feat->sse2 = edx & bit_SSE2;
    // ECX
    feat->sse3 = ecx & bit_SSE3;
    feat->pcmuldq = ecx & bit_PCMULDQ;
    feat->ssse3 = ecx & bit_SSSE3;
    feat->fmac = ecx & bit_FMA;
    feat->cmpxchg16b = ecx & bit_CMPXCHG16B;
    feat->sse41 = ecx & bit_SSE41;
    feat->sse42 = ecx & bit_SSE42;
    feat->xsave = ecx & bit_XSAVE;
    feat->avx = ecx & bit_AVX;
    // EAX: Ext Fam, Ext Model, Proc. Type, Family, Model, Stepping
    // see p. 18 of intel app. note 485
    feat->signature = eax & 0x7ffffff;

    CPUID(0x80000000, eax, ebx, ecx, edx);
    ext_level = eax;
    if (ext_level >= 0x80000000) {
        CPUID(0x80000001, eax, ebx, ecx, edx);
        feat->simd_3dnow = edx & bit_3DNOW;
        feat->simd_3dnowp = edx & bit_3DNOWP;
        feat->longmode = edx & bit_LM;
    }
}

/* at least Darwin got some sysctl on the topic, freebsd prob. too */

#if defined(__DARWIN__)
int _my_sysctl_getint(const char *name, int64_t * ret) {
    int mib[6];
    size_t len;
    union {
        int32_t *pi32;
        int64_t *pi64;
    } uval;

    /* Fill out the first components of the mib */
    len = 6;
    // have a look at:
    // kern.usrstack64 on OSX 10.5
    // sysctl.h CTLTYPE_INT CTLTYPE_QUAD
    // => use int32_t or int64_t, seems always signed
    if (sysctlnametomib(name, mib, &len) == -1)
        return -1;
    if (sysctl(mib, 2, NULL, &len, NULL, 0) == -1)
        return -2;
    switch (len) {
    case 4:
        uval.pi32 = malloc(4);
        if (sysctl(mib, 2, uval.pi32, &len, NULL, 0) == -1)
            return -3;
        *ret = *(uval.pi32);
        free(uval.pi32);
        break;
    case 8:
        uval.pi64 = malloc(8);
        if (sysctl(mib, 2, uval.pi64, &len, NULL, 0) == -1)
            return -3;
        *ret = *(uval.pi64);
        free(uval.pi64);
        break;
    }
    return 0;
}

void _my_sysctl_simd(cpu_info_t * feat) {
    int64_t sysctl_ret;

    if (_my_sysctl_getint("hw.ncpu", &sysctl_ret) == 0)
        ;
    feat->ncores = sysctl_ret;

    if (_my_sysctl_getint("hw.cpufrequency", &sysctl_ret) == 0)
        ;
    feat->cpufrequency = sysctl_ret;

    if (_my_sysctl_getint("hw.cachelinesize", &sysctl_ret) == 0)
        ;
    feat->cachelinesize = sysctl_ret;
}
#endif

int cpuinfo(cpu_info_t * feat) {
    double freq = 0.0;

    _features_init(feat);
    _cpuid_simd(feat);
#if defined(__DARWIN__)
    _my_sysctl_simd(feat);
#endif
    mhz(&freq);
    feat->cpufrequency_measured = freq;
    threading(&(feat->thread));
    return 0;
}

void pprint_cpuinfo(cpu_info_t * feat) {
    printf("CPU features :\n\
----------------\n\
Signature27:0... : 0x%X\n\
MMX ............ : %d\n\
3D Now ......... : %d\n\
3D NowP ........ : %d\n\
SSE ............ : %d\n\
SSE 2 .......... : %d\n\
SSE 3 .......... : %d\n\
SSSE 3 ......... : %d\n\
SSE 4.1 ........ : %d\n\
SSE 4.2 ........ : %d\n\
AVX ............ : %d\n\
FMA ............ : %d\n\
clflush ........ : %d\n\
cmov ........... : %d\n\
fxsr ........... : %d\n\
xsave .......... : %d\n\
pcmuldq ........ : %d\n\
longmode ....... : %d\n\
cmpxchg8b ...... : %d\n\
cmpxchg16b ..... : %d\n\
Logical CPUs ... : %d\n\
# of cores ..... : %d\n\
CPU freq ....... : %.4E Hz\n\
CPU freq probed. : %.1f MHz\n\
Cacheline size.. : %lld\n\
Vendor ID ...... : %s\n", feat->signature, feat->mmx, feat->simd_3dnow, feat->simd_3dnowp,
feat->sse, feat->sse2, feat->sse3, feat->ssse3, feat->sse41, feat->sse42, feat->avx,
feat->fmac, feat->clflush, feat->cmov, feat->fxsr, feat->xsave, feat->pcmuldq,
feat->longmode, feat->cmpxchg8b, feat->cmpxchg16b, feat->thread.logical, feat->ncores,
feat->cpufrequency, feat->cpufrequency_measured, (long long) feat->cachelinesize, feat->thread.vendor_id);
}
