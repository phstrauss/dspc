/*  Audio Signal Processing routines in C++
    © Philippe Strauss, 2013  */


#include <dspc/exception.hh>
#include <dspc/mem.hh>


void *malloc_exn(size_t sz, const char *filename, const char *funcname, int line) {
    void *mem = malloc(sz);

    if (mem == NULL)
        throw exception_t("mem == NULL", "could not allocate memory", filename, funcname, line);

    return mem;
}

void *calloc_exn(size_t count, size_t elt_sz, const char *filename, const char *funcname, int line) {
    void *mem = calloc(count, elt_sz);

    if (mem == NULL)
        throw exception_t("mem == NULL", "could not allocate memory", filename, funcname, line);

    return mem;
}


/* darwin align everything on 16 bytes "vs." posix_memalign :

int status = posix_memalign(&ret, getpagesize(), size);
if (status) {
    errno = status;
    return NULL;
}

*/

#if defined(__DARWIN__) && !defined(HAVE_POSIX_MEMALIGN)
#define ALIGNED_ALLOC(alignment, size)  malloc(size)
// PST 20211025: seems recent Linux (ubuntu) don't define HAVE_POSIX_MEMALIGN
//#elif defined(HAVE_POSIX_MEMALIGN)
#else

void *aligned_alloc(size_t alignment, size_t size) {
    void *ret;
    int status = posix_memalign(&ret, alignment, size);
    if (status) {
        errno = status;
        return NULL;
    } else
        return ret;
}

#define ALIGNED_ALLOC(alignment, size)  aligned_alloc(alignment, size)
#endif


void *aligned_exn(size_t alignment, size_t sz, const char *filename, const char *funcname, int line) {
    void *mem = ALIGNED_ALLOC(alignment, sz);

    if (mem == NULL)
        throw exception_t("mem == NULL", "could not allocate memory", filename, funcname, line);

    return mem;
}

void *caligned_exn(size_t count, size_t elt_sz, const char *filename, const char *funcname, int line) {
    void *mem = ALIGNED_ALLOC(SPS_ALIGNMENT, count * elt_sz);

    if (mem == NULL)
        throw exception_t("mem == NULL", "could not allocate memory", filename, funcname, line);

    return mem;
}

