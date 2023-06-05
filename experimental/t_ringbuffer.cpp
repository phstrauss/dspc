/*  Audio Signal Processing routines in C++
    A ringbuffer for audio use, using Jack
    Criterion test file
    © Philippe Strauss, 2021  */


#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <dspc/rb.hh>


// Test(asserts, array) {
//     int arr1[] = { 1, 2, 3, 4 };
//     int arr2[] = { 4, 3, 2, 1 };

//     /* For primitive types we can compare their byte-to-byte representation */
//     struct cr_mem mem_arr1 = { .data = arr1, .size = 4 * sizeof (int) };
//     struct cr_mem mem_arr2 = { .data = arr2, .size = 4 * sizeof (int) };

//     cr_assert(eq(mem, mem_arr1, mem_arr1));
//     cr_assert(ne(mem, mem_arr1, mem_arr2));

//     /* Or we can use the tag[] notation */
//     cr_assert(eq(int[4], arr1, arr1));
//     cr_assert(ne(int[4], arr1, arr2));
// }

Test(ringbuffer_t, write) {
    float expected[] =
        {20.,21.,22.,23.,24.,5.,6.,7.,8.,9.,10.,11.,12.,13.,14.,15.,16.,17.,18.,19.};

    ringbuffer_t *rb = new ringbuffer_t(1, 20, 5);

    float arr1[] = {0.,1.,2.,3.,4.};
    rb->write(0, arr1);
    float arr2[] = {5.,6.,7.,8.,9.};
    rb->write(0, arr2);
    float arr3[] = {10.,11.,12.,13.,14.};
    rb->write(0, arr3);
    float arr4[] = {15.,16.,17.,18.,19.};
    rb->write(0, arr4);
    float arr5[] = {20.,21.,22.,23.,24.};
    rb->write(0, arr5);

    float **buf = rb->get_buffer_ptr();
    cr_assert(eq(flt[20], expected, buf[0]));
}

Test(ringbuffer_t, read_overlap) {
    float buffer[] =
        {0.,1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.,13.,14.,15.,16.,17.,18.,19.};

    ringbuffer_t *rb = new ringbuffer_t(1, 20, 5);
    // rb->set_overlap(2);

    /* read with overlap i.e. for STFT or FFT filtering */

    float to[5];

    // 3 writes
    float arr1[] = {0.,1.,2.,3.,4.};
    rb->write(0, arr1);
    float arr2[] = {5.,6.,7.,8.,9.};
    rb->write(0, arr2);
    float arr3[] = {10.,11.,12.,13.,14.};
    rb->write(0, arr3);

    // 3 overlaping reads
    float arr10[] = {0.,0.,0.,1.,2.};
    rb->read(0, to, 2);
    cr_assert(eq(flt[5], arr10, to));

    float arr11[] = {1.,2.,3.,4.,5.};
    rb->read(0, to, 2);
    cr_assert(eq(flt[5], arr11, to));

    float arr12[] = {4.,5.,6.,7.,8.};
    rb->read(0, to, 2);
    cr_assert(eq(flt[5], arr12, to));

    // last write
    float arr4[] = {15.,16.,17.,18.,19.};
    rb->write(0, arr4);

    // 3 overlaping reads
    float arr13[] = {7.,8.,9.,10.,11.};
    rb->read(0, to, 2);
    cr_assert(eq(flt[5], arr13, to));

    float arr14[] = {10.,11.,12.,13.,14.};
    rb->read(0, to, 2);
    cr_assert(eq(flt[5], arr14, to));

    float arr15[] = {13.,14.,15.,16.,17.};
    rb->read(0, to, 2);
    cr_assert(eq(flt[5], arr15, to));

    // can only be tested after last write
    float **buf = rb->get_buffer_ptr();
    cr_assert(eq(flt[20], buffer, buf[0]));

}