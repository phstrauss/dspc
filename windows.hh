/*  Audio Signal Processing routines in C++
    © Philippe Strauss, philippe@strauss-acoustics.ch, 2013  */


#ifndef _SPS_WINDOWS_HH
#define _SPS_WINDOWS_HH


#include <dspc/dspc.h>
#include <dspc/exception.hh>


int fact(long int);


enum class win_common {hann, hann_ampc_gs, hamming, blackman, blackman_harris_92db,
    bartlett, bartlett_hann, connes, flattop, kaiser_bessel, nuttall, triangle, welch};
typedef enum win_common win_common_t;


class windows_t {

public:
    windows_t(int len);
    ~windows_t(void);
    void init(win_common_t fct);
    void init_kaiser(const float_t beta, int bessel_iter);
    void init_gaussian(const float_t sigma);
    void init_tukey(const float_t p);
    void init_sinc(int k);
    int get_len(void);
    void process(float_t *wave);
    void process(const float_t *in, float_t *out);

private:
    int len;
    float_t N, N2; // const, use (initialize) constructor syntax
    float_t *win;

};


#endif
