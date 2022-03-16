// Count number of bits on

typedef unsigned u_t;
typedef unsigned long long ull_t;

u_t __byte_nbits[0x100];

static void __byte_nbits_set()
{
    for (u_t n = 0; n < 0x100; ++n)
    {
        u_t nbits = 0;
        for (u_t i = 0; i < 8; ++i)
        {
            nbits += (n & (1u << i)) ? 1 : 0;
        }
        __byte_nbits[n] = nbits;
    }
}

u_t nbits_on(ull_t n)
{
    u_t nb = 0;
    while (n)
    {
        nb += __byte_nbits[n & 0xff];
        n >>= 8;
    }
    return nb;
}

#if defined(TEST_NBITSON)
#include <iostream>
#include <cstdlib>
using namespace std;

int main(int argc, char **argv)
{
    __byte_nbits_set();
    for (int ai = 1; ai < argc; ++ai)
    {
        ull_t n = strtoul(argv[ai], 0, 0);
        cout << "#bits(" << argv[ai] << ") = " << nbits_on(n) << '\n';
    }
    return 0;
}
#endif
