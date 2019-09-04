#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>
#include <stdlib.h>

int hex_to_buffer(const uint8_t *hex, size_t hex_len, uint8_t *out,
                  size_t out_len);
int buffer_to_hex(const uint8_t *buffer, size_t buffer_len, uint8_t *out,
                  size_t out_len);
uint64_t hex2dec(char *source);

#endif