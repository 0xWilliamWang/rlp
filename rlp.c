#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#define RES_BUFFER_LEN 100
typedef enum
{
    NONE,
    STRING,
    LIST
} seq_type;

char *rlp_decode(char *sequence, int len)
{
    if (len == 0)
        return NULL;
    seq_type type = NONE;
    char *read_ptr;
    int read_len = get_decode_length(sequence, &read_ptr, &type);
    if (type == STRING)
    {
    }
    else if (type == LIST)
    {

        // memcpy(res_buffer, read_ptr, read_len);
    }
}

int get_decode_length(char *seq, char *read_ptr, seq_type *type)
{
    char first_byte[2] = {seq[0], seq[1]};

    int read_len = 0;
    if (first_byte <= 0x7f)
    {
        read_len = 1;
        type = STRING;
        read_ptr = seq++;
        return read_len;
    }
    else if (first_byte <= 0xb7 && sizeof(seq) > first_byte - 0x80) // 第二个条件是为了防止以后读取数据时越界
    {
        read_len = first_byte - 0x80;
        type = STRING;
        read_ptr = seq++;
        return read_len;
    }
    else if (first_byte <= 0xbf && sizeof(seq) > first_byte - 0xb7)
    {
        /* code */
    }
}

static bool is_big_endian()
{
    int num = 65538;          // 0x00000000 00000001 00000000 00000010
    char *ptr = (char *)&num; // 第1个字节的内容是1则是小端字节序，为0则是大端字节序
    if (*ptr == 2 && *(ptr + 1) == 0 && *(ptr + 2) == 1 && *(ptr + 3) == 0)
        return false;
    else
        return true;
}

int hex2dec(char *seq, int len)
{
    int dec_val = 0;
    int index = 0;
    int tmp = 0;
    bool big_endian = is_big_endian();
    if (big_endian)
    {
        for (size_t i = len - 1; i >= 0; i--)
        {
            tmp = (int)(seq[i]);
            dec_val += pow(16, index) * tmp;
            index++;
        }
    }
    else
    {
        for (size_t i = 0; i <= len - 1; i++)
        {
            tmp = (int)(seq[i]);
            dec_val += pow(16, index) * tmp;
            index++;
        }
    }
}

void some_test()
{
    char *strs[4];
    strs[0] = "wangkaixuan";
    strs[1] = "cgr";
    strs[2] = "lh";
    strs[3] = "wkj";
    char ch = strs[1][0];
    char *n = "‭EEE\0‬";
    int num = (int)n;
    // char *pa = ;
    // int res = hex2dec(pa, 3);
    // printf("some:%d",num);
}

static uint8_t convert_hex_to_digital(uint8_t c)
{
    if (c >= '0' && c <= '9')
    {
        c -= '0';
    }
    else if (c >= 'A' && c <= 'F')
    {
        c = c - 'A' + 10;
    }
    else if (c >= 'a' && c <= 'f')
    {
        c = c - 'a' + 10;
    }
    else
    {
        return 0xff;
    }

    return c;
}

int hex_to_buffer(const uint8_t *hex, size_t hex_len, uint8_t *out, size_t out_len)
{
    int padding = hex_len % 2;
    uint8_t last_digital = 0;
    int i;
    for (i = 0; i < hex_len && i < out_len * 2; i++)
    {
        uint8_t d = convert_hex_to_digital(hex[i]);
        if (d > 0x0f)
        {
            break;
        }

        if ((i + padding) % 2)
        {
            out[(i + padding) / 2] = ((last_digital << 4) & 0xf0) | (d & 0x0f);
        }
        else
        {
            last_digital = d;
        }
    }

    return (i + padding) / 2;
}

int main(int argc, char const *argv[])
{
    char seq[] = "f889008609184e72a00082271094000000000000000000000000000000000000000000a47f74657374320000000000000000000000000000000000000000000000000000006000571ca05e1d3a76fbf824220eafc8c79ad578ad2b67d01b0c2425eb1f1347e8f50882aba05bd428537f05f9830e93792f90ea6a3e2d1ee84952dd96edbae9f658f831ab13";
    char buffer[(sizeof(seq)) / 2] = {0};
    hex_to_buffer(seq, sizeof(seq) - 1, buffer, (sizeof(seq) - 1) / 2);

    // char *res1 = rlp_decode(encode_sequence1, sizeof(encode_sequence1) / sizeof(encode_sequence1[0]));
    // printf("wangkaixaun");
    // some_test();
    // hex2dec();

    return 0;
}
