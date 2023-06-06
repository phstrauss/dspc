/*  Audio Signal Processing routines in C++
    © Philippe Strauss, 2013  */


#ifndef _SPS_MEM_CPP_HH
#define _SPS_MEM_CPP_HH


#include <dspc/templating.hh>
#include <cstdlib>
#include <dspc/dspc.h>

#ifndef SPS_ALIGNMENT
#define SPS_ALIGNMENT 16
#endif


void *malloc_exn(size_t sz, const char *filename,
				 const char *funcname, int line_nr);
void *calloc_exn(size_t count, size_t elt_sz, const char *filename,
	             const char *funcname, int line_nr);
void *aligned_exn(size_t alignment, size_t sz, const char *filename,
	             const char *funcname, int line_nr);
void *caligned_exn(size_t count, size_t elt_sz, const char *filename,
	             const char *funcname, int line_nr);

EXPORT template <typename float_T>
float_T **alloc_simple_matrix(unsigned int lines, unsigned int columns,
							  const char *filename, const char *funcname, int line_nr);

EXPORT template <typename float_T>
void free_simple_matrix(float_T **mem, int lines, const char *filename,
	                    const char *funcname, int line_nr);


#define MALLOC_EXN(sz) 						malloc_exn(sz, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define CALLOC_EXN(count, elt_sz) 				calloc_exn(count, elt_sz, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define ALIGNED_EXN(alignment, sz) 				aligned_exn(alignent, sz, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define CALIGNED_EXN(count, elt_sz) 				caligned_exn(count, elt_sz, __FILE__, __PRETTY_FUNCTION__, __LINE__)

#define ALLOC_SIMPLE_MATRIX(float_type, lines, columns)		alloc_simple_matrix<float_type>(lines, columns, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define FREE_SIMPLE_MATRIX(float_type, mem, lines)			free_simple_matrix<float_type>(mem, lines, __FILE__, __PRETTY_FUNCTION__, __LINE__)


#include <dspc/exception.hh>


EXPORT template <typename float_T>
float_T **alloc_simple_matrix(unsigned int lines, unsigned int columns, const char *filename, const char *funcname, int line) {
    unsigned int i, j;

    float_t **mem = (float_T **) calloc_exn(lines, sizeof(float_T *), filename, funcname, line);

    if (mem == NULL)
        throw exception_t("mem == NULL", "could not allocate memory", filename, funcname, line);

    for (i = 0; i < lines; ++i) {
        mem[i] = (float_t *) caligned_exn(columns, sizeof(float_T), filename, funcname, line);
        if (mem[i] == NULL) {
            for (j = i-1; j > 0; --j)
                free(mem[j]);
            throw exception_t("mem[i] == NULL", "could not allocate memory", filename, funcname, line);
        }

    }
    return mem;
}

EXPORT template <typename float_T>
void free_simple_matrix(float_T **mem, int lines, const char *filename, int line) {
    int i;

    for (i = 0; i < lines; ++i)
        free(mem[i]);
    free(mem);
}

#endif
