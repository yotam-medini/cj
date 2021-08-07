#include <iostream>
#include <iomanip>

using namespace std;

typedef unsigned u_t;
typedef unsigned long long ull_t;

int most_significant_bit(ull_t x)
{
    int msb = -1;
    if (x)
    {
        msb = 0;
        u_t mask_bits = 32;
        ull_t mask = (ull_t(1) << mask_bits) - 1; // 32 bits on
        while (mask)
        {
            if (x & (mask << (msb + mask_bits)))
            {
                msb += mask_bits;
            }
            mask_bits /= 2;
            mask >>= (mask_bits ? : 1);
        }
    }
    return msb;
}

int main(int argc, char **argv)
{
    int rc = 0;

    if (most_significant_bit(0) >= 0)
    {
        cerr << "Failed: @ x=0\n";
        rc = 0;
    }
    ull_t x = 1;
    for (int msb = 0; (msb < 64) && (rc == 0); ++msb)
    {
        if (most_significant_bit(x) != msb)
        {
            cerr << "Failed with x=" << hex << x << '\n';
            rc = 1;
        }
        x = (x << 1) | (msb % 2);
    }
    return rc;
}

