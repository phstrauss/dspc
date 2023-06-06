/*  Audio Signal Processing routines in C++
    © Philippe Strauss, 2013  */


#ifndef _SPS_SFIO_HH
#define _SPS_SFIO_HH


#include <sndfile.h>
#include <dspc/dspc.h>
#include <dspc/gnuinline.h>
#include <dspc/exception.hh>
#include <assert.h>


INLINE void deinterleave(float_t *wav, float_t **demux, int totframes, int nchan) {
    int i;

    assert(totframes % nchan == 0);

    for (i = 0; i < totframes; ++i)
        demux[i % nchan][i / nchan] = wav[i];
}

INLINE void interleave(float_t **demux, float_t *wav, int nframes, int nchan) {
    int i;

    for (i = 0; i < (nchan * nframes); ++i)
        wav[i] = demux[i % nchan][i / nchan];
}


// maybe needs to wrap combined read / seek for overlap-save, plus
// interleave / deinterleave

class sfio_t {

public:

    sfio_t(void);
    ~sfio_t(void);
    void open_read(const char *path, int _readlen, int _overlap, int _demux_len);
    SF_FORMAT_INFO *get_format(void);
    int get_channels(void);
    int get_samplerate(void);
    sf_count_t get_samples(void);
    void open_write(const char *path, int _format, int _channels, int _samplerate, int _demux_len);
    sf_count_t seek(long seek);
    sf_count_t read(float_t *wav, sf_count_t nsamp);
    sf_count_t read(float_t **demux, sf_count_t *offset);
    sf_count_t write(float_t *wav, sf_count_t nsamp);
    sf_count_t write(float_t **demux, sf_count_t nsamp);
    void close(void);

private:

    int readlen;
    int overlap;
    long nread = 0;
    int demux_len;
    float_t *buf;
    SNDFILE *sfd;

    /* struct SF_INFO
    {       sf_count_t      frames ;
            int                     samplerate ;
            int                     channels ;
            int                     format ;
            int                     sections ;
            int                     seekable ;
    } ; */
    struct SF_INFO sf_info;

    /* int sf_command(SNDFILE *sndfile, int command, void *data, int datasize) ;

    command : SFC_GET_FORMAT_INFO

    data :

        typedef struct
    {       int                     format ;
            const char      *name ;
            const char      *extension ;
    } SF_FORMAT_INFO ; */
    SF_FORMAT_INFO fmt;
    void init_buffer(int depth);

};


/*  automatically add Libsndfile error reporting strings to exns  */
class exception_sndfile_t : public exception_t {

public:

    exception_sndfile_t(SNDFILE *_sfd, string _filename, string _funcname, int _linenumber);
    ~exception_sndfile_t(void);
    void add_sf_msg(void);

private:

    SNDFILE *sfd;

};


#define EXN_SNDFILE_THROW(sfd)  exception_sndfile_t(sfd, __FILE__, __PRETTY_FUNCTION__, __LINE__)


#endif
