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

uint8_t *rlp_decode(uint8_t *sequence, int len)
{
    if (len == 0)
        return NULL;
    seq_type type = NONE;
    uint8_t *read_ptr;
    int read_len = get_decode_length(sequence, len, read_ptr, &type);
    if (type == STRING)
    {
    }
    else if (type == LIST)
    {
        int res;
        printf("====");
        // memcpy(res_buffer, read_ptr, read_len);
        // read_len = rlp_decode(read_ptr,)
    }
}

int get_decode_length(uint8_t *seq, int seq_len, uint8_t *read_ptr, seq_type *type)
{
    uint8_t first_byte = *seq;

    int read_len = 0;

    if (first_byte <= 0x7f)
    {
        read_len = 1;
        type = STRING;
        read_ptr = seq+1;
        return read_len;
    }
    else if (first_byte <= 0xb7 && seq_len > (first_byte - 0x80)) // 第二个条件是为了防止以后读取数据时越界
    {
        read_len = first_byte - 0x80;
        type = STRING;
        read_ptr = seq+1;
        return read_len;
    }
    else if (first_byte <= 0xbf && seq_len > (first_byte - 0xb7))
    {
        /* code */
    }
    else if (first_byte <= 0xf7 && seq_len > (first_byte - 0xc0))
    {
        /* code */
    }
    else if (first_byte <= 0xff && seq_len > (first_byte - 0xf7))
    {
        read_len = first_byte - 0xf7;
        *type = LIST;
        read_ptr = seq+1;
        return read_len;
    }
}

static bool is_big_endian()
{
    int num = 65538;                // 0x00000000 00000001 00000000 00000010
    uint8_t *ptr = (uint8_t *)&num; // 第1个字节的内容是1则是小端字节序，为0则是大端字节序
    if (*ptr == 2 && *(ptr + 1) == 0 && *(ptr + 2) == 1 && *(ptr + 3) == 0)
        return false;
    else
        return true;
}

int hex2dec(uint8_t *seq, int len)
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
    uint8_t *strs[4];
    strs[0] = "wangkaixuan";
    strs[1] = "cgr";
    strs[2] = "lh";
    strs[3] = "wkj";
    uint8_t ch = strs[1][0];
    uint8_t *n = "‭EEE\0‬";
    int num = (int)n;
    // uint8_t *pa = ;
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

int main(int argc, uint8_t const *argv[])
{
    uint8_t seq[] = "f889008609184e72a00082271094000000000000000000000000000000000000000000a47f74657374320000000000000000000000000000000000000000000000000000006000571ca05e1d3a76fbf824220eafc8c79ad578ad2b67d01b0c2425eb1f1347e8f50882aba05bd428537f05f9830e93792f90ea6a3e2d1ee84952dd96edbae9f658f831ab13";
    uint8_t buffer[(sizeof(seq)) / 2] = {0};
    hex_to_buffer(seq, sizeof(seq) - 1, buffer, (sizeof(seq) - 1) / 2);

    uint8_t *res1 = rlp_decode(buffer, sizeof(buffer) / sizeof(buffer[0]));

    // printf("wangkaixaun");
    // some_test();
    // hex2dec();

    return 0;
}
