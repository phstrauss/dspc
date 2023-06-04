/*  good quality random numbers, taken from numerical recipes, 3rd ed.,
    translated from C++ to ansi C.
    Philippe Strauss, 3.2012  */


#include <dspc/rng.h>
/* open */
#include <fcntl.h>
/* read */
#include <sys/types.h>
/* #define _POSIX_C_SOURCE 200809L */
#ifdef __linux__
#include <sys/stat.h>
#endif
#include <sys/uio.h>
#include <unistd.h>
/* perr_str */
#include <dspc/perr.h>


int seed_from_devrandom(struct rng_s *st, uint64_t * seed) {
    int fd;
    uint64_t *array = (uint64_t *) st->random;
    static int seek;

    if (seek == 0 || seek >= (RAND_BSIZE / (sizeof(uint64_t)) - 1)) {
        fd = open("/dev/random", O_RDONLY /*|O_NOFOLLOW */ );
        if (fd == -1) {
            perr_str("ERR_OPEN on opening /dev/random");
            return ERR_OPEN;
        }
        if (read(fd, st->random, RAND_BSIZE) < RAND_BSIZE) {
            perr_str("ERR_READ on reading /dev/random");
            return ERR_READ;
        }
        close(fd);
        seek = 1;
    }
    *seed = array[seek++];
    return 0;
}

void init_rng(struct rng_s *st, uint64_t seed) {
    st->v = 4101842887655102017LL;
    st->w = 1;
    st->u = seed ^ st->v;
    uniform_int64(st);
    st->v = st->u;
    uniform_int64(st);
    st->w = st->v;
    uniform_int64(st);
}

int init_rng_from_devrandom(struct rng_s *st) {
    uint64_t seed;

    if (seed_from_devrandom(st, &seed) != 0) {
        eprintf
            ("ERR_DEVRANDOM in init_rng_from_devrandom, falling back on hard-coded seed\n");
        init_rng(st, 9845847364529872484UL);
        return ERR_DEVRANDOM;
    }
    init_rng(st, seed);
    return 0;
}
