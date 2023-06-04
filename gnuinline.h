#ifndef _GNUINLINE_H
#define _GNUINLINE_H

//#ifdef __cplusplus
//extern "C" {
//#endif

/* Taken from GSL_inline.h :

   In recent versiions of GCC, the inline keyword has two different
   forms: GNU and C99.

   In GNU mode we can use 'extern inline' to make inline functions
   work like macros.  The function is only inlined--it is never output
   as a definition in an object file.

   In the new C99 mode 'extern inline' has a different meaning--it
   causes the definition of the function to be output in each object
   file where it is used.  This will result in multiple-definition
   errors on linking.  The 'inline' keyword on its own (without
   extern) has the same behavior as the original GNU 'extern inline'.

   The C99 style is the default with -std=c99 in GCC 4.3.  

   This header file allows either form of inline to be used by
   redefining the macros INLINE_DECL and INLINE_FUN.  These are used
   in the public header files as

        INLINE_DECL double gsl_foo (double x);
        #ifdef HAVE_INLINE
        INLINE_FUN double gsl_foo (double x) { return x+1.0; } ;
        #endif
   
*/

#ifndef INLINE
# if __GNUC__ && !__GNUC_STDC_INLINE__
#  define INLINE extern inline __attribute__ ((__gnu_inline__))
#  define _INLINE extern inline __attribute__ ((__gnu_inline__))
# else
#  define INLINE inline
#  define _INLINE inline
# endif
#endif

//#ifdef __cplusplus
//} /* extern "C" */
//#endif

#endif
