#include "../src/rlp.h"

void test_rlp_decode() {
  uint8_t seq[] =
      "f889008609184e72a00082271094000000000000000000000000000000000000000000a4"
      "7f7465737432000000000000000000000000000000000000000000000000000000600057"
      "1ca05e1d3a76fbf824220eafc8c79ad578ad2b67d01b0c2425eb1f1347e8f50882aba05b"
      "d428537f05f9830e93792f90ea6a3e2d1ee84952dd96edbae9f658f831ab13";
  uint8_t buffer[(sizeof(seq)) / 2] = {0};
  hex_to_buffer(seq, sizeof(seq) - 1, buffer, (sizeof(seq) - 1) / 2);

  decode_result my_resut;
  char no_use;
  my_resut.data = malloc(sizeof(&no_use) * DECODE_RESULT_LEN);
  my_resut.capacity = DECODE_RESULT_LEN;
  my_resut.used_index = 0;

  rlp_decode(&my_resut, buffer, sizeof(buffer) / sizeof(buffer[0]));

  assert(strcmp(my_resut.data[0], "00") == 0);
  assert(strcmp(my_resut.data[1], "09184e72a000") == 0);
  assert(strcmp(my_resut.data[2], "2710") == 0);
  assert(strcmp(my_resut.data[3], "0000000000000000000000000000000000000000") ==
         0);
  assert(strcmp(my_resut.data[4], "00") == 0);
  assert(strcmp(my_resut.data[5], "7f746573743200000000000000000000000000000000"
                                  "0000000000000000000000600057") == 0);
  assert(strcmp(my_resut.data[6], "1c") == 0);
  assert(
      strcmp(
          my_resut.data[7],
          "5e1d3a76fbf824220eafc8c79ad578ad2b67d01b0c2425eb1f1347e8f50882ab") ==
      0);
  assert(
      strcmp(
          my_resut.data[8],
          "5bd428537f05f9830e93792f90ea6a3e2d1ee84952dd96edbae9f658f831ab13") ==
      0);
}

void test_rlp_decode1() {
  uint8_t seq[] =
      "f8aa018504e3b2920083030d409486fa049857e0209aa7d9e616f7eb3b3b78ecfdb080b8"
      "44a9059cbb0000000000000000000000001febdb3539341a3005f3c5851854db7720a037"
      "fe00000000000000000000000000000000000000000000000002c68af0bb1400001ba0fe"
      "13bc4be8dc46e804f5b7eb6182d1f6feecdc2574eafface0ecc7f7be3516f4a042586cf9"
      "62a3896e607214eede1a5cde727e13b62f86678efb980d9047a1017e";
  uint8_t buffer[(sizeof(seq)) / 2] = {0};
  hex_to_buffer(seq, sizeof(seq) - 1, buffer, (sizeof(seq) - 1) / 2);

  decode_result my_resut;
  char no_use;
  my_resut.data = malloc(sizeof(&no_use) * DECODE_RESULT_LEN);
  my_resut.capacity = DECODE_RESULT_LEN;
  my_resut.used_index = 0;

  rlp_decode(&my_resut, buffer, sizeof(buffer) / sizeof(buffer[0]));

  for (size_t i = 0; i < my_resut.used_index; i++) {
    printf("index:%d,data:%s\n", i, my_resut.data[i]);
  }
}

void test_rlp_decode2() {
  uint8_t seq[] =
      "f8a903850a2fa8e8e2825dc09486fa049857e0209aa7d9e616f7eb3b3b78ecfdb080b844"
      "a9059cbb0000000000000000000000001febdb3539341a3005f3c5851854db7720a037fe"
      "000000000000000000000000000000000000000000000000016345785d8a00001ca023a0"
      "f5e57bfa3b518919078e1aa98c854a641322b1e2c7c6272c27d81e380d99a00c6102a9c0"
      "114772f9c64cc9c7aeccb7296640db079e35781276c66a0927b7ba";
  uint8_t buffer[(sizeof(seq)) / 2] = {0};
  hex_to_buffer(seq, sizeof(seq) - 1, buffer, (sizeof(seq) - 1) / 2);

  decode_result my_resut;
  char no_use;
  my_resut.data = malloc(sizeof(&no_use) * DECODE_RESULT_LEN);
  my_resut.capacity = DECODE_RESULT_LEN;
  my_resut.used_index = 0;

  rlp_decode(&my_resut, buffer, sizeof(buffer) / sizeof(buffer[0]));

  for (size_t i = 0; i < my_resut.used_index; i++) {
    printf("index:%d,data:%s\n", i, my_resut.data[i]);
  }
}

void test_hex2dec() {
  uint64_t len = hex2dec("11");
  assert(len == 17);

  len = hex2dec("fd");
  assert(len == 253);

  len = hex2dec("f3d");
  assert(len == 3901);

  len = hex2dec("f3d3");
  assert(len == 62419);

  len = hex2dec("f3d3e");
  assert(len == 998718);

  len = hex2dec("f3d3ed");
  assert(len == 15979501);

  len = hex2dec("f3d3ed2");
  assert(len == 255672018);

  len = hex2dec("f3d3ed22");
  assert(len == 4090752290);
}

void test_get_decode_length() {
  uint8_t buf;
  int decoded_len = 0;
  seq_type type = NONE;
  uint64_t read_len = 0;

  hex_to_buffer("00", 2, &buf, 1);
  read_len = get_decode_length(&buf, 1, &decoded_len, &type);
  assert(decoded_len == 0 && type == STRING && read_len == 1);

  hex_to_buffer("88", 2, &buf, 1);
  read_len = get_decode_length(&buf, 9, &decoded_len, &type); // 假设长度大于8
  assert(decoded_len == 1 && type == STRING && read_len == 8);

  hex_to_buffer("c9", 2, &buf, 1);
  read_len = get_decode_length(&buf, 10, &decoded_len, &type);
  assert(decoded_len == 1 && type == LIST && read_len == 9);

  uint8_t buf2[2];
  hex_to_buffer("f889", 4, &buf2, 2);
  read_len = get_decode_length(buf2, 200, &decoded_len, &type);
  assert(decoded_len == 2 && type == LIST && read_len == 137);

  hex_to_buffer("b811", 4, &buf2, 2);
  read_len = get_decode_length(buf2, 20, &decoded_len, &type);
  assert(decoded_len == 2 && type == STRING && read_len == 17);

  uint8_t buf3[3];
  hex_to_buffer("f9df23", 6, &buf3, 3);
  read_len = get_decode_length(buf3, 20, &decoded_len, &type);
  assert(decoded_len == 2 && type == LIST && read_len == 57123);
}
int main(int argc, char const *argv[]) {
  test_hex2dec();
  test_get_decode_length();
  test_rlp_decode();
  test_rlp_decode1();
  test_rlp_decode2();
  return 0;
}
