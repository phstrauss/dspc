/*  perr.c
    Error handling helpers
    (c) Philippe Strauss, spring 2010  */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>             // exit
#include <stdarg.h>
#include <sys/errno.h>

#include <dspc/perr.h>


void eprintf(const char *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    vfprintf(stderr, fmt, argp);
    va_end(argp);
    fflush(stderr);
}

void perr_str(const char *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    vfprintf(stderr, fmt, argp);
    va_end(argp);
    fprintf(stderr, ", errno=%d, %s\n", errno, strerror(errno));
    fflush(stderr);
}

void perr_exit(int exitcode, const char *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    vfprintf(stderr, fmt, argp);
    va_end(argp);
    fprintf(stderr, ", errno=%d, %s, exiting!\n", errno, strerror(errno));
    fflush(stderr);
    exit(exitcode);
}
