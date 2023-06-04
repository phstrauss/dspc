/*  Audio Signal Processing routines in C++
    © Philippe Strauss, 2013  */


#include <dspc/dspc.h>
#include <dspc/exception.hh>
#include <cassert>
#include <sndfile.h>
#include <dspc/perr.h>
#include <dspc/mem.hh>
#include <dspc/sfio.hh>


sfio_t::sfio_t(void) {}

void sfio_t::close(void) {
    if (sfd != NULL)
        if (sf_close(sfd) != 0)
            EXN_SNDFILE_THROW(sfd);
    free(buf);
}

sfio_t::~sfio_t(void) { close(); }

void sfio_t::init_buffer(int depth) {
    buf = (float_t *) MALLOC_EXN(depth * sizeof(float_t));
}

void sfio_t::open_read(const char *path, int _readlen, int _overlap, int _demux_len) {
    sfd = sf_open(path, SFM_READ, &sf_info);
    if (sfd == NULL)
        EXN_SNDFILE_THROW(sfd);
    readlen = _readlen;
    overlap = _overlap;
    demux_len = _demux_len;
    nread = 0;
    if (demux_len > 0) init_buffer(demux_len * sf_info.channels);
}

void sfio_t::open_write(const char *path, int _format, int _channels, int _samplerate, int _demux_len) {
    demux_len = _demux_len;
    sf_info.format = _format;
    sf_info.channels = _channels;
    sf_info.samplerate = _samplerate;
    sfd = sf_open(path, SFM_WRITE, &sf_info);
    if (sfd == NULL) EXN_SNDFILE_THROW(sfd);
    if (demux_len > 0) init_buffer(demux_len * sf_info.channels);
}

SF_FORMAT_INFO *sfio_t::get_format(void) {
    sf_command(sfd, SFC_GET_FORMAT_INFO, (void *) &fmt, sizeof(SF_FORMAT_INFO));
    return &fmt;
}

int sfio_t::get_channels(void) {
    return sf_info.channels;
}

int sfio_t::get_samplerate(void) {
    return sf_info.samplerate;
}

sf_count_t sfio_t::get_samples(void) {
    /* TOTAL NUMBER OF __SAMPLES__, NOT FRAMES IN FILE,
       COULD assert(get_frames() % get_channels() == 0) */
    return sf_info.frames;
}

/* frames are samples * #channels */
sf_count_t sfio_t::read(float_t *wav, sf_count_t nframes) {
    sf_count_t cnt = sf_readf_float(sfd, wav, nframes);
    if(cnt < 0)
        EXN_SNDFILE_THROW(sfd);

    return cnt;
}

sf_count_t sfio_t::seek(long seek) {
    return sf_seek(sfd, seek, SEEK_SET);
}

sf_count_t sfio_t::read(float_t **demux, sf_count_t *offset) {
    /* let seek = if !times > 0 then !times * (* finfo.nchan * *) (bsize - overlap) else 0 in
    let offset = Sf.seek finfo.fd (Int64.of_int seek) Sf.SEEK_SET in */
    long seek = nread++ * (readlen - overlap);
    *offset = sfio_t::seek(seek);
    /* WW : GAAAAASP fucking stuck your f'ckin glasses on your dumbass godam fucking nose
       you 'cking bastard readf, reeeeaaaaddddddd èèèèèèèèèèèèffffff
       read/return the number of __frames__, not elements or samples.
       one more godam mothefucker bildeberg commendited bug which was actually mine */
    sf_count_t cnt = sf_readf_float(sfd, buf, readlen);
    if(cnt < 0)
        EXN_SNDFILE_THROW(sfd);

    deinterleave(buf, demux, cnt*sf_info.channels, sf_info.channels);

    return cnt;
}

sf_count_t sfio_t::write(float_t *wav, sf_count_t nsamp) {
    sf_count_t cnt = sf_write_float(sfd, wav, nsamp);
    if (cnt < 0)
        EXN_SNDFILE_THROW(sfd);

    return cnt;
}

sf_count_t sfio_t::write(float_t **demux, sf_count_t nsamp) {

    assert(nsamp <= demux_len);

    interleave(demux, buf, nsamp, sf_info.channels);

    sf_count_t cnt = sf_writef_float(sfd, buf, nsamp);
    if (cnt < 0)
        EXN_SNDFILE_THROW(sfd);

    return cnt;
}


exception_sndfile_t::exception_sndfile_t(SNDFILE *_sfd, string _filename, string _funcname,
                                         int _linenumber)
    : exception_t(_filename, _funcname, _linenumber), sfd(_sfd) {

    sfd = _sfd;

    add_sf_msg();
}

exception_sndfile_t::~exception_sndfile_t(void) {};

void exception_sndfile_t::add_sf_msg(void) {
    const char *sferrstr = sf_strerror(sfd);
    add_line("Libsndfile", sferrstr);
}
