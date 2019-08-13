#include <stdio.h>
#include <stdlib.h>
// typedef enum
// {
//     NONE,
//     STRING,
//     LIST
// } seq_type;

// char *rlp_decode(char *sequence, int len)
// {
//     if (len == 0)
//         return NULL;
//     int read_len = 0;
//     seq_type type = NONE;
//     get_decode_length(sequence, &read_len, &type);


// }

// // 返回偏移、长度、类型
// void get_decode_length(char *seq, int *read_len, seq_type *type)
// {
//     char first_byte = *seq;
//     if (first_byte <= 0x7f)
//     {
//         read_len = 1;
//         type = STRING;
//     }
// }

// char * get_char(){
//     char * str = malloc(11 * sizeof * str);
//     str = "wangkaixuan";
//     return str;
// }

int main(int argc, char const *argv[])
{
    char encode_sequence1[] = "80";
    // char *res1 = rlp_decode(encode_sequence1, sizeof(encode_sequence1) / sizeof(encode_sequence1[0]));
    // char * str =  get_char();
    printf("wangkaixaun");

    return 0;
}
