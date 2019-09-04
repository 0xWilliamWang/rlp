#include "rlp.h"

/* 返回ch字符在sign数组中的序号 */
uint64_t get_index_of_signs(char ch) {
  if (ch >= '0' && ch <= '9') {
    return ch - '0';
  }
  if (ch >= 'A' && ch <= 'F') {
    return ch - 'A' + 10;
  }
  if (ch >= 'a' && ch <= 'f') {
    return ch - 'a' + 10;
  }
  return -1;
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

uint64_t get_decode_length(uint8_t *seq, int seq_len, int *decoded_len,
                           seq_type *type) {
  uint8_t first_byte = *seq;
  uint64_t item_bytes_len = 0;
  if (first_byte <= 0x7f) {
    item_bytes_len = 1;
    *type = STRING;
    *decoded_len = 0;
  } else if (first_byte <= 0xb7 &&
             seq_len >
                 (first_byte - 0x80)) // 第二个条件是为了防止以后读取数据时越界
  {
    item_bytes_len = first_byte - 0x80;
    *type = STRING;
    *decoded_len = 1;
  } else if (first_byte <= 0xbf && seq_len > (first_byte - 0xb7)) {
    uint8_t len = first_byte - 0xb7;
    uint8_t buffer_len[len];
    uint8_t hex_len[len * 2 + 1];
    hex_len[len * 2] = '\0';
    *decoded_len = 1;
    memcpy(buffer_len, seq + *decoded_len, len);
    *decoded_len += 1;
    buffer_to_hex(buffer_len, len, hex_len, len * 2);
    item_bytes_len = hex2dec(hex_len);
    *type = STRING;
  } else if (first_byte <= 0xf7 && seq_len > (first_byte - 0xc0)) {
    item_bytes_len = first_byte - 0xc0;
    *type = LIST;
    *decoded_len = 1;
  } else if (first_byte <= 0xff && seq_len > (first_byte - 0xf7)) {
    uint8_t len = first_byte - 0xf7;
    uint8_t buffer_len[len];
    uint8_t hex_len[len * 2 + 1];
    hex_len[len * 2] = '\0';

    *decoded_len = 1;
    memcpy(buffer_len, seq + *decoded_len, len);
    *decoded_len += 1;
    buffer_to_hex(buffer_len, len, hex_len, len * 2);
    item_bytes_len = hex2dec(hex_len);
    *type = LIST;
  } else {
    perror("sequence don't conform RLP encoding form");
  }

  return item_bytes_len;
}

// NOTE: 目前只能解码没有嵌套list，且str和list的字节数量都没有超过55字节的情况
int rlp_decode(decode_result *my_result, uint8_t *seq, int seq_len) {
  if (seq_len == 0)
    return 0;
  seq_type type = NONE; // 保存此次将要处理的数据的类型
  int decoded_len;      // 保存此次解码过的长度
  uint64_t item_num = get_decode_length(seq, seq_len, &decoded_len,
                                        &type); // 保存需要解码的长度
  uint8_t *start_ptr = seq + decoded_len;       // 解码的起始地址
  int need_decode_len = seq_len - decoded_len;

  if (type == STRING) {
    if (my_result->capacity < my_result->used_index) {
      // TODO:申请更大的内存空间，并拷贝旧数据，释放旧空间
    }
    char tmp[item_num];
    memcpy(tmp, start_ptr, item_num);
    uint8_t *buf1 = malloc(sizeof(char) * item_num * 2 + 1);
    buffer_to_hex(tmp, item_num, buf1, item_num * 2);
    buf1[item_num * 2] = '\0';
    my_result->data[my_result->used_index++] = buf1;
    rlp_decode(my_result, start_ptr + item_num, need_decode_len - item_num);
  } else if (type == LIST) {
    rlp_decode(my_result, start_ptr, need_decode_len);
  }
}

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
