#include <initializer_list>

typedef unsigned long long ull_t;
typedef unsigned u_t;

// Knuth Combinatorial Algorithms Volume 4 - bitwise tricks and techniques
// Algorithm B, 

static ull_t mu_dk(u_t d, u_t k)
{
    ull_t numerator = (1ull << (1u << d)) - 1;
    ull_t denominator = (1ull << (1u << k)) + 1;
    ull_t ret = numerator / denominator;
    return ret;
}

static ull_t mu4_3to0()
{
    ull_t ret = mu_dk(4, 3);
    ret = (ret << 16) | mu_dk(4, 2);
    ret = (ret << 16) | mu_dk(4, 1);
    ret = (ret << 16) | mu_dk(4, 0);
    return ret;
}

int ilog2(ull_t x)
{
    int lambda = -1;
    if (x > 0)
    {
        static const u_t d = 4;
        static const u_t n = 64; // n = d*2^d = 64,  lg(d)=2
        
        // B1 - Scale Down
        lambda = 0;
        for (u_t k : {5u, 4u})
        {
            ull_t two_pk = 1u << k;
            if (x >= (1u << two_pk))
            {
                lambda += two_pk;
                x >>= two_pk;
            }
        }
        // B2 - Replicate
        for (u_t k: {0, 1})
        {
            x = x | (x << (1u << (d + k)));
        }
        // B3 - Change leading bit
        static const ull_t neg_mu4_3to0 = ~mu4_3to0();
        static const ull_t h = 0x8000800080008000;
        ull_t y = x & neg_mu4_3to0;
        ull_t t = h & (y | ((y | h) - (x ^ y)));
        // B5 - Compress bits
        for (u_t k: {0, 1})
        {
            const u_t shift = (1u << (d + k)) - (1u << k);
            t += t << shift;
        }
        lambda += (t >> (n - d));
    }
    return lambda;
}

#if defined(TEST_ILOG2)

#include <iostream>
using namespace std;

static int ilog2_naive(ull_t x)
{
    int lambda = -1;
    if (x > 0)
    {
         lambda = 0;
         while (x >> 1)
         {
             ++lambda;
             x >>= 1;
         }
    }
    return lambda;
}

static int test_ilog2(ull_t x)
{
    int rc = 0;
    int naive = ilog2_naive(x);
    int knuth = ilog2(x);
    if (knuth != naive)
    {
        rc = 1;
        cerr << "Failed: x=" << x << ", ilog2_naive(x) = " << naive <<
            ", knuth(x) = " << knuth << '\n';
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;
    for (int ai = 1; (rc == 0) && (ai < argc); ++ai)
    {
        rc = test_ilog2(strtoul(argv[ai], nullptr, 0));
    }
    return rc;
}


#endif
