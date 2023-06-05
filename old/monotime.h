/*  monotime.h
    monotonic increasing time routine for usec accuracy function timing.
    found on the net, originally by Thomas Habets <thomas@habets.pp.se> 2010.
    Philippe Strauss, spring 2012  */


#ifndef _SPS_MONOTIME_H
#define _SPS_MONOTIME_H

#ifdef __cplusplus
extern "C" {
#endif


    double monotime_fallback(void);
    int clock_is_monotonic(void);
    double monotime(void);


#ifdef __cplusplus
}                               /* extern "C" */
#endif
#endif
