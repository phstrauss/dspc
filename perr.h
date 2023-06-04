/*  perr.h
    Error handling helpers
    (c) Philippe Strauss, spring 2010  */

#ifndef _SPS_PERR_H
#define _SPS_PERR_H

#ifdef __cplusplus
extern "C" {
#endif


    void eprintf(const char *, ...);
    void perr_str(const char *, ...);
    void perr_exit(int, const char *, ...);

// some ideas borrowed from jack.*
#ifdef DEBUG
#define dprintf(...) eprintf(__VA_ARGS__)
#else
#define dprintf(...)
#endif

#ifdef VERBOSE_DEBUG
#define vdprintf(...) eprintf(__VA_ARGS__)
#else
#define vdprintf(...)
#endif

#ifdef VERY_VERBOSE_DEBUG
#define vvdprintf(...) eprintf(__VA_ARGS__)
#else
#define vvdprintf(...)
#endif

#define FAIL exit(-1)

#define PFAIL(...) perr_exit(-1, __VA_ARGS__)


#ifdef __cplusplus
}                               /* extern "C" */
#endif
#endif
