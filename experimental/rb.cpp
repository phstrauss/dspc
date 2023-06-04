/*  Audio Signal Processing routines in C++
    A ringbuffer for audio use, using Jack
    © Philippe Strauss, 2013, 2021  */


#include <dspc/dspc.h>
#include <dspc/rb.hh>
#include <cstdio>
#include <cstring> // memcpy
#include <cassert>
#include <dspc/exception.hh>
#include <dspc/mem.hh>
#include <string.h>
#include <dspc/perr.h>


ringbuffer_t::ringbuffer_t(int _nchan, int _depth, int _len_frame): nchan(_nchan), depth(_depth), len_frame(_len_frame)
{
    assert(depth % len_frame == 0);

    writeptr = 0;
    readptr  = 0;
    frames_read = 0;

    buffer = (float_t **) CALLOC_EXN(nchan, sizeof(float_t *));
    for (int i = 0; i < nchan; ++i)
        buffer[i] = (float_t *) CALLOC_EXN(depth, sizeof(float_t));
}

float_t **ringbuffer_t::get_buffer_ptr() {
    return buffer;
}

ringbuffer_t::~ringbuffer_t(void) {
    for (int i = 0; i < nchan; ++i)
        free(buffer[i]);
    free(buffer);
}

void ringbuffer_t::write(int chanid, float_t *from) {
    if(writeptr == depth) { // wraparound
        writeptr = 0;
        vdprintf("write: writeptr == depth, wrapping.\n");
    } // WW: else if writeptr > depth, exn ?
    memcpy(buffer[chanid]+writeptr, from, len_frame * sizeof(float_t));
    writeptr = (writeptr + len_frame) % depth;
    frames_written += 1; // WW: one such var per channel ?
}

void ringbuffer_t::get_ptr(pointers_t *ptrs) {
    ptrs->readptr = readptr;
    ptrs->writeptr = writeptr;
}

//
// see spreadsheet "overlap.ods": write > read:
//
// .initialy:   readlag = writeptr - overlap (! len_frame must be cst)
// .readstep  = len_frame - overlap
// .readptr+1 = (readptr + readstep) % depth ; if ... < len_frame
// writestep  = len_frame
// .writetail = writeptr - len_frame
// .headroom/available  = writetail - readptr (goal is to keep headroom ~cst)
//
int ringbuffer_t::read(int chanid, float_t *to, int overlap) {
    int readstep  = len_frame - overlap;
    int readtail  = readptr   - len_frame; // WW: at read wrap-around tail is negative !

    if (readtail < 0)
        readtail += depth;

    dprintf("read TOP: writeptr=%d; readptr=%d; readtail=%d; frames_written=%d; frames_read=%d\n",
            writeptr, readptr, readtail, frames_written, frames_read);

    if (frames_read < 1) {
        dprintf("branch frames_read < 1: readstep=%d\n", readstep);
        if (overlap > 0)
            memset(to, 0, overlap * sizeof(float_t)); // initial zero padding
        memcpy(to + overlap, buffer[chanid], (len_frame-overlap) * sizeof(float_t));
        readptr = len_frame - overlap;
    } else {
        dprintf("branch A: frames_read >= 1\n");
        if (readptr >= writeptr + len_frame) { // read > write: writewrap
            dprintf("branch B: readptr >= writeptr + len_frame\n");
            if (readptr < readtail) { // readwrap, 2 segments to copy
                dprintf("branch C: readptr < readtail\n");
                // high addresses
                memcpy(to, buffer[chanid] + readtail, (depth - readtail) * sizeof(float_t));
                // low addresses
                memcpy(to + (depth - readtail), buffer[chanid], (readptr - 0) * sizeof(float_t));
            } else { // 1 segment
                dprintf("branch D: readptr >= readtail\n");
                memcpy(to, buffer[chanid] + readtail, len_frame * sizeof(float_t));
            }
        } else if (writeptr >= readptr + len_frame)  { // write > read
            dprintf("branch E: writeptr >= readptr + len_frame: readtail=%d\n", readtail);
            memcpy(to, buffer[chanid] + readtail, len_frame * sizeof(float_t));
        } else { // right during an overwrite
            dprintf("branch F\n");
            // memset (to + offset, 0, len);
        }
    } // WW: output (fill with) 0.0's instead of over/underrun ?
    frames_read += 1;
    readptr = (readptr + readstep) % depth;
    int available = writeptr - len_frame - readptr; // if negative: we are in trouble, write underrun
    dprintf("read BOTTOM: readptr=%d; frames_written=%d; frames_read=%d; available=%d\n",
        readptr, frames_written, frames_read, available);
    return available;
}

void ringbuffer_t::reset(void) {
    writeptr = 0;
    readptr  = 0;
    blank();    
}

// private
void ringbuffer_t::blank(void) {
    for (int i = 0; i < nchan; ++i)
        for (int j = 0; j < depth; ++j)
            buffer[i][j] = 0.;
}

// TODO:
//
// read RETURN VALUE !
// overlap=0 !
// throw exn on overwrite ?
// read start/len ?
// callback for read sync ? on_new_frame()
// read frame length integer multiple of write frame length ?
// time measurement of write interval ?
// rather, registration of fs in the RB (fixed bandwidth) ?
// no channel id; act on all channels equaly ?
