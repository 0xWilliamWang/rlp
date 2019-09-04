#include "utils.h"

static uint8_t convert_hex_to_digital(uint8_t c) {
  if (c >= '0' && c <= '9') {
    c -= '0';
  } else if (c >= 'A' && c <= 'F') {
    c = c - 'A' + 10;
  } else if (c >= 'a' && c <= 'f') {
    c = c - 'a' + 10;
  } else {
    return 0xff;
  }

  return c;
}

int hex_to_buffer(const uint8_t *hex, size_t hex_len, uint8_t *out,
                  size_t out_len) {
  int padding = hex_len % 2;
  uint8_t last_digital = 0;
  int i;
  for (i = 0; i < hex_len && i < out_len * 2; i++) {
    uint8_t d = convert_hex_to_digital(hex[i]);
    if (d > 0x0f) {
      break;
    }

    if ((i + padding) % 2) {
      out[(i + padding) / 2] = ((last_digital << 4) & 0xf0) | (d & 0x0f);
    } else {
      last_digital = d;
    }
  }

  return (i + padding) / 2;
}

int buffer_to_hex(const uint8_t *buffer, size_t buffer_len, uint8_t *out,
                  size_t out_len) {
  uint8_t map[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                   '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  int i;
  for (i = 0; i < buffer_len && i < out_len / 2; i++) {
    uint8_t b = (buffer[i] >> 4) & 0x0f;
    uint8_t l = buffer[i] & 0x0f;
    out[i * 2] = map[b];
    out[i * 2 + 1] = map[l];
  }

  return i * 2;
}

/* 十六进制数转换为十进制数 */
uint64_t hex2dec(char *source) {
  uint64_t sum = 0;
  uint64_t t = 1;
  int i, len = 0;

  len = strlen(source);
  for (i = len - 1; i >= 0; i--) {
    uint64_t j = get_index_of_signs(*(source + i));
    sum += (t * j);
    t *= 16;
  }

  return sum;
}