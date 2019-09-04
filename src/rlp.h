#ifndef __RLP_H__
#define __RLP_H__

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DECODE_RESULT_LEN 100

typedef enum { NONE, STRING, LIST } seq_type;

typedef struct {
  uint8_t **data;
  uint8_t capacity;
  uint8_t size;
  // decode_result * next; // TODO: 这个字段可以用来实现，保存嵌套的list
} decode_result;

uint64_t get_decode_length(uint8_t *seq, int seq_len, int *decoded_len,
                           seq_type *type);
int rlp_decode(decode_result *my_result, uint8_t *seq, int seq_len);
#endif
