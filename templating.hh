/*  Audio Signal Processing routines in C/C++
    © Philippe Strauss, philippe@strauss-acoustics.ch, 2013  */


#ifndef _SPS_TEMPLATING_HH
#define _SPS_TEMPLATING_HH


/*  set accordingly your compiler features about templating,
    or prefer to set this in your build system  */

#undef HAS_EXPORT
//#define NEED_TEMPLATE_DEFINITIONS 1


#ifdef HAS_EXPORT
    #define EXPORT export
#else
    #define EXPORT
#endif


#endif