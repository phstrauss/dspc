/* taken from http://www.hackersdelight.org/crc.pdf by Henry S. Warren
   Ph. Strauss: replaced the while loops by for w. explicit start and stop bounds */


#include <dspc/crc32_hswarren.h>


unsigned int crc32_once(unsigned char *message, int start, int stop) {
    int i, j;
    unsigned int byte, crc, mask;

    i = 0;
    crc = 0xFFFFFFFF;

    // while (message[i] != 0) {
    for (i = start; i < stop; ++i) {
        byte = message[i];      // Get next byte.
        crc = crc ^ byte;
        for (j = 7; j >= 0; j--) {  // Do eight times.
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & mask);
        }
        // i = i + 1;
    }
    return ~crc;
}

unsigned int crc32_lut(unsigned char *message, int start, int stop) {
    int i, j;
    unsigned int byte, crc, mask;
    static unsigned int table[256];

    /* Set up the table, if necessary. */
    if (table[1] == 0) {
        for (byte = 0; byte <= 255; byte++) {
            crc = byte;
            for (j = 7; j >= 0; j--) {  // Do eight times.
                mask = -(crc & 1);
                crc = (crc >> 1) ^ (0xEDB88320 & mask);
            }
            table[byte] = crc;
        }
    }

    /* Through with table setup, now calculate the CRC. */
    i = 0;
    crc = 0xFFFFFFFF;
    // while ((byte = message[i]) != 0) {
    for (i = start; i < stop; ++i) {
        byte = message[i];
        crc = (crc >> 8) ^ table[(crc ^ byte) & 0xFF];
        // i = i + 1;
    }
    return ~crc;
}
