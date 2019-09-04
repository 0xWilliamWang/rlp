#ifndef __RLP_H__
#define __RLP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>


#define DECODE_RESULT_LEN 100

typedef enum
{
    NONE,
    STRING,
    LIST
} seq_type;

typedef struct
{
    uint8_t **data;
    uint8_t capacity;
    uint8_t used_index;
    // decode_result * next; // TODO: 这个字段可以用来实现，保存嵌套的list
} decode_result;

uint64_t get_decode_length(uint8_t *seq, int seq_len, int *decoded_len, seq_type *type);
void test_get_decode_length();
int rlp_decode(decode_result *my_result, uint8_t *seq, int seq_len);
uint64_t hex2dec(char *source);
uint64_t get_index_of_signs(char ch);
static uint8_t convert_hex_to_digital(uint8_t c);
int hex_to_buffer(const uint8_t *hex, size_t hex_len, uint8_t *out, size_t out_len);
int buffer_to_hex(const uint8_t *buffer, size_t buffer_len, uint8_t *out, size_t out_len);
void test_rlp_decode();
void test_rlp_decode1();
void test_rlp_decode2();
void test_hex2dec();
#endif
