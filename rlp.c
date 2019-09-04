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

int get_decode_length(uint8_t *seq, int seq_len, int *decoded_len, seq_type *type)
{
    uint8_t first_byte = *seq;
    int item_bytes_len = 0;
    if (first_byte <= 0x7f)
    {
        item_bytes_len = 1;
        *type = STRING;
        *decoded_len = 0;
    }
    else if (first_byte <= 0xb7 && seq_len > (first_byte - 0x80)) // 第二个条件是为了防止以后读取数据时越界
    {
        item_bytes_len = first_byte - 0x80;
        *type = STRING;
        *decoded_len = 1;
    }
    else if (first_byte <= 0xbf && seq_len > (first_byte - 0xb7))
    {
        item_bytes_len = *(seq + 1);
        *type = STRING;
        *decoded_len = 2; // TODO:需要计算表示后续序列长度的数量
    }
    else if (first_byte <= 0xf7 && seq_len > (first_byte - 0xc0))
    {
        item_bytes_len = first_byte - 0xc0;
        *type = LIST;
        *decoded_len = 1;
    }
    else if (first_byte <= 0xff && seq_len > (first_byte - 0xf7))
    {
        item_bytes_len = *(seq + 1);
        *type = LIST;
        *decoded_len = 2; // TODO:需要计算表示后续序列长度的数量
    }
    else
    {
        perror("sequence don't conform RLP encoding form");
    }

    return item_bytes_len;
}

void test_get_decode_length()
{
    uint8_t buf;
    int decoded_len = 0;
    seq_type type = NONE;
    int read_len = 0;

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

    hex_to_buffer("b911", 4, &buf2, 2);
    read_len = get_decode_length(buf2, 20, &decoded_len, &type);
    assert(decoded_len == 2 && type == STRING && read_len == 17);
}
// NOTE: 目前只能解码没有嵌套list，且str和list的字节数量都没有超过55字节的情况
int rlp_decode(decode_result *my_result, uint8_t *seq, int seq_len)
{
    if (seq_len == 0)
        return 0;
    seq_type type = NONE;                                                // 保存此次将要处理的数据的类型
    int decoded_len;                                                     // 保存此次解码过的长度
    int item_num = get_decode_length(seq, seq_len, &decoded_len, &type); // 保存需要解码的长度
    uint8_t *start_ptr = seq + decoded_len;                              // 解码的起始地址
    int need_decode_len = seq_len - decoded_len;

    if (type == STRING)
    {
        if (my_result->capacity < my_result->used_index)
        {
            // TODO:申请更大的内存空间，并拷贝旧数据，释放旧空间
        }
        char tmp[item_num];
        memcpy(tmp, start_ptr, item_num);
        uint8_t *buf1 = malloc(sizeof(char) * item_num * 2 + 1);
        buffer_to_hex(tmp, item_num, buf1, item_num * 2);
        buf1[item_num * 2] = '\0';
        my_result->data[my_result->used_index++] = buf1;
        rlp_decode(my_result, start_ptr + item_num, need_decode_len - item_num);
    }
    else if (type == LIST)
    {
        rlp_decode(my_result, start_ptr, need_decode_len);
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

int hex2dec(uint8_t *seq, int seq_len)
{
    int dec_val = 0;
    int index = 0;
    int tmp = 0;
    bool big_endian = is_big_endian();
    if (big_endian)
    {
        for (size_t i = seq_len - 1; i >= 0; i--)
        {
            tmp = (int)(seq[i]);
            dec_val += pow(16, index) * tmp;
            index++;
        }
    }
    else
    {
        for (size_t i = 0; i <= seq_len - 1; i++)
        {
            tmp = (int)(seq[i]);
            dec_val += pow(16, index) * tmp;
            index++;
        }
    }
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

int buffer_to_hex(const uint8_t *buffer, size_t buffer_len, uint8_t *out, size_t out_len)
{
    uint8_t map[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    int i;
    for (i = 0; i < buffer_len && i < out_len / 2; i++)
    {
        uint8_t b = (buffer[i] >> 4) & 0x0f;
        uint8_t l = buffer[i] & 0x0f;
        out[i * 2] = map[b];
        out[i * 2 + 1] = map[l];
    }

    return i * 2;
}

void test_rlp_decode()
{
    uint8_t seq[] = "f889008609184e72a00082271094000000000000000000000000000000000000000000a47f74657374320000000000000000000000000000000000000000000000000000006000571ca05e1d3a76fbf824220eafc8c79ad578ad2b67d01b0c2425eb1f1347e8f50882aba05bd428537f05f9830e93792f90ea6a3e2d1ee84952dd96edbae9f658f831ab13";
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
    assert(strcmp(my_resut.data[3], "0000000000000000000000000000000000000000") == 0);
    assert(strcmp(my_resut.data[4], "00") == 0);
    assert(strcmp(my_resut.data[5], "7f7465737432000000000000000000000000000000000000000000000000000000600057") == 0);
    assert(strcmp(my_resut.data[6], "1c") == 0);
    assert(strcmp(my_resut.data[7], "5e1d3a76fbf824220eafc8c79ad578ad2b67d01b0c2425eb1f1347e8f50882ab") == 0);
    assert(strcmp(my_resut.data[8], "5bd428537f05f9830e93792f90ea6a3e2d1ee84952dd96edbae9f658f831ab13") == 0);
}

void test_rlp_decode1()
{
    uint8_t seq[] = "f8aa018504e3b2920083030d409486fa049857e0209aa7d9e616f7eb3b3b78ecfdb080b844a9059cbb0000000000000000000000001febdb3539341a3005f3c5851854db7720a037fe00000000000000000000000000000000000000000000000002c68af0bb1400001ba0fe13bc4be8dc46e804f5b7eb6182d1f6feecdc2574eafface0ecc7f7be3516f4a042586cf962a3896e607214eede1a5cde727e13b62f86678efb980d9047a1017e";
    uint8_t buffer[(sizeof(seq)) / 2] = {0};
    hex_to_buffer(seq, sizeof(seq) - 1, buffer, (sizeof(seq) - 1) / 2);

    decode_result my_resut;
    char no_use;
    my_resut.data = malloc(sizeof(&no_use) * DECODE_RESULT_LEN);
    my_resut.capacity = DECODE_RESULT_LEN;
    my_resut.used_index = 0;

    rlp_decode(&my_resut, buffer, sizeof(buffer) / sizeof(buffer[0]));

    for (size_t i = 0; i < my_resut.used_index; i++)
    {
        printf("index:%d,data:%s\n", i, my_resut.data[i]);
    }
}

void test_rlp_decode2()
{
    uint8_t seq[] = "f8a903850a2fa8e8e2825dc09486fa049857e0209aa7d9e616f7eb3b3b78ecfdb080b844a9059cbb0000000000000000000000001febdb3539341a3005f3c5851854db7720a037fe000000000000000000000000000000000000000000000000016345785d8a00001ca023a0f5e57bfa3b518919078e1aa98c854a641322b1e2c7c6272c27d81e380d99a00c6102a9c0114772f9c64cc9c7aeccb7296640db079e35781276c66a0927b7ba";
    uint8_t buffer[(sizeof(seq)) / 2] = {0};
    hex_to_buffer(seq, sizeof(seq) - 1, buffer, (sizeof(seq) - 1) / 2);

    decode_result my_resut;
    char no_use;
    my_resut.data = malloc(sizeof(&no_use) * DECODE_RESULT_LEN);
    my_resut.capacity = DECODE_RESULT_LEN;
    my_resut.used_index = 0;

    rlp_decode(&my_resut, buffer, sizeof(buffer) / sizeof(buffer[0]));

    for (size_t i = 0; i < my_resut.used_index; i++)
    {
        printf("index:%d,data:%s\n", i, my_resut.data[i]);
    }
}

int main(int argc, uint8_t const *argv[])
{
    test_get_decode_length();
    test_rlp_decode();
    test_rlp_decode1();
    test_rlp_decode2();
    return 0;
}
