/*  monotime.c
    monotonic increasing time routine for usec accuracy function timing.
    found on the net, originally by Thomas Habets <thomas@habets.pp.se> 2010.
    Philippe Strauss, 03.2012  */

#include <time.h>
#include <sys/time.h>
#include <dspc/monotime.h>
#include <dspc/perr.h>


double monotime_fallback(void) {
#if HAVE_GETTIMEOFDAY
    struct timeval tv;

    eprintf("Warning: falling back to gettimeofday()\n");
    if (0 == gettimeofday(&tv, NULL))
        return tv.tv_sec + tv.tv_usec / 1000000.0;
#endif
    eprintf("Warning: falling back to time()\n");
    return (double) time(0);
}


#undef __linux__
#if defined(__linux__)
/* untested as of 25.3.2012 */

int clock_is_monotonic(void) {
    struct timespec ts;

    if (clock_gettime(CLOCK_MONOTONIC, &ts))
        return 0;
    return 1;
}

double monotime(void) {
    struct timespec ts;

    /* try clock_gettime() */
    if (0 == clock_gettime(CLOCK_MONOTONIC, &ts))
        return ts.tv_sec + ts.tv_nsec / 1000000000.0;

    eprintf("Warning: clock_gettime() returned non-zero, falling back\n");
    return monotime_fallback();
}

#endif

#if defined(__DARWIN__)

#include <mach/mach_time.h>

int clock_is_monotonic(void) {
    mach_timebase_info_data_t info;

    if (mach_timebase_info(&info))
        return 0;

    return 1;
}

double monotime(void) {
    uint64_t time = mach_absolute_time();
    static double scaling_factor = 0;

    if (scaling_factor == 0) {
        mach_timebase_info_data_t info;
        kern_return_t ret = mach_timebase_info(&info);
        if (ret != 0) {
            eprintf
                ("Warning: mach_timebase_info() failed: %d, falling back\n",
                 ret);

            return monotime_fallback();
        }
        scaling_factor = (double) info.numer / (double) info.denom;
    }
    return (double) time *scaling_factor / 1000000000.0;
}

#endif
