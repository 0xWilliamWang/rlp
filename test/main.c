#include "../src/rlp.h"

int main(int argc, char const *argv[])
{
    test_hex2dec();
    test_get_decode_length();
    test_rlp_decode();
    test_rlp_decode1();
    test_rlp_decode2();
    return 0;
}
