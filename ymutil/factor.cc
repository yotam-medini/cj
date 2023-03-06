#include <array>
#include <vector>

using namespace std;

typedef unsigned u_t;
typedef vector<u_t> vu_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;

void factor(vau2_t& factors, u_t n, const vu_t& primes)
{
    factors.clear();
    for (u_t pi = 0; n > 1;)
    {
        for (; (n > 1) && ((n % primes[pi]) != 0); ++pi) {}
        const u_t prime = primes[pi];
        au2_t pe{prime, 1};
        for ( n /= prime; n % prime == 0; ++pe[1], n /= prime) {}
        factors.push_back(pe);
    }
}


#if defined(TEST_FACTOR)
#include <algorithm>
#include <iostream>
#include <cstdlib>

extern void get_primes(vu_t& primes, u_t till);

int main(int argc, char **argv)
{
    int rc = 0;
    u_t n_max = 2;
    for (int ai = 1; ai < argc; ++ai)
    {
        u_t n = strtoul(argv[ai], nullptr, 0);
        n_max = max(n_max, n);
    }
    vu_t primes;
    get_primes(primes, n_max);
    for (int ai = 1; ai < argc; ++ai)
    {
        u_t n = strtoul(argv[ai], nullptr, 0);
        vau2_t factors;
        factor(factors, n, primes);
        cout << n << " =";
        for (const au2_t& pe: factors)
        {
            cout << ' ' << pe[0] << '^' << pe[1];
        }
        cout << '\n';
    }
    return rc;
}
#endif /* TEST_FACTOR */
