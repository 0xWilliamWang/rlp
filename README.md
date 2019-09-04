# rlp
A pure c implementation of Recursive Length Prefix encoding (RLP). You can find the specification of the standard in the [Ethereum wiki](https://github.com/ethereum/wiki/wiki/RLP#rlp-decoding).

# notice
 Nested data parsing is not implemented and only support decode.

# usage
1. set the RLP encoded sequence
1. convert to buffer data
1. define a decode_result variable to hold the results
1. call `rlp_decode` to decode
1. get the data through `size`

demo:
```c
  uint8_t seq[] =
      "f889008609184e72a00082271094000000000000000000000000000000000000000000a4"
      "7f7465737432000000000000000000000000000000000000000000000000000000600057"
      "1ca05e1d3a76fbf824220eafc8c79ad578ad2b67d01b0c2425eb1f1347e8f50882aba05b"
      "d428537f05f9830e93792f90ea6a3e2d1ee84952dd96edbae9f658f831ab13";
  uint8_t buffer[(sizeof(seq)) / 2] = {0};
  hex_to_buffer(seq, sizeof(seq) - 1, buffer, (sizeof(seq) - 1) / 2);

  decode_result my_resut;
  my_resut.data = malloc(sizeof(char) * DECODE_RESULT_LEN);
  my_resut.capacity = DECODE_RESULT_LEN;
  my_resut.size = 0;

  rlp_decode(&my_resut, buffer, sizeof(buffer) / sizeof(buffer[0]));

  assert(strcmp(my_resut.data[0], "00") == 0);
  assert(strcmp(my_resut.data[1], "09184e72a000") == 0);
  assert(strcmp(my_resut.data[2], "2710") == 0);
  assert(strcmp(my_resut.data[3], "0000000000000000000000000000000000000000") == 0);
  assert(strcmp(my_resut.data[4], "00") == 0);
  assert(strcmp(my_resut.data[5], "7f7465737432000000000000000000000000000000000000000000000000000000600057") == 0);
  assert(strcmp(my_resut.data[6], "1c") == 0);
```

# test
see [test](./test/main.c)