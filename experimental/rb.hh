/*  Audio Signal Processing routines in C++
    An advanced ringbuffer for audio use w. Jack audio.
    © Philippe Strauss, 2013, 2021  */


#ifndef _SPS_RB_HH
#define _SPS_RB_HH


#include <dspc/dspc.h>


typedef struct {
    int readptr;
    int writeptr;
} pointers_t;


class ringbuffer_t {

public:
    ringbuffer_t(int nchan, int depth, int len_frame);
    ~ringbuffer_t(void);

    float_t **get_buffer_ptr(void);
    // get read and write "pointers"
    void get_ptr(pointers_t *ptrs);

    // size fixed to len_frame and depth MUST BE a multiple of len_frame
    void write(int chanid, float_t *from);

    /* return number of frame(s) left (readable) in
       full frame mode; number of samples in overlap mode */
    //int available(int chanid);

    /* read first frame available in RB.
       return number of frame(s) left (readable) in
       full frame mode; number of samples in overlap mode */
    int read(int chanid, float_t *to, int overlap);

    void reset(void);

private:
    int nchan, depth, len_frame;
    int writeptr, readptr;
    unsigned long long int frames_read;
    unsigned long long int frames_written;
    float_t **buffer; // float_t : jack audio in mind

    void blank(void);
};


#endif
