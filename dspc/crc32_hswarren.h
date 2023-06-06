/* taken from http://www.hackersdelight.org/crc.pdf by Henry S. Warren
   Ph. Strauss: replaced the while loops by for w. explicit start and stop bounds */


#ifndef _SPS_CRC32_HSW_H
#define _SPS_CRC32_HSW_H

#ifdef __cplusplus
extern "C" {
#endif


    unsigned int crc32_once(unsigned char *message, int start, int stop);
    unsigned int crc32_lut(unsigned char *message, int start, int stop);


#ifdef __cplusplus
}                               /* extern "C" */
#endif
#endif
