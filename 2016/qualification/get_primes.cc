#include "get_primes.h"

using namespace std;

typedef unsigned long ul_t;
typedef vector<ul_t> vul_t;
typedef vector<bool> vb_t;

const vul_t &get_primes(unsigned long at_least_until)
{
    static vul_t primes;
    static ul_t until;

    if (primes.empty() || until < at_least_until)
    {
        until = at_least_until;
        primes.clear();
        vb_t sieve(vb_t::size_type(until + 1), true);

        for (ul_t d = 2; d * d <= until; ++d)
        {
            if (sieve[d])
            {
                for (ul_t np = d*d; np <= until; np += d)
                {
                    sieve[np] = false;
                }
            }
        }
        // for (auto si = sieve.begin(); si != sieve.end(); ++si)
        for (ul_t si = 2; si <= until; ++si)
        {
            if (sieve[si])
            {
                primes.push_back(si);
            }
        }
    }
    return primes;
}

#if defined(TEST)

#include <iostream>
#include <cstdlib>

int main(int argc, char **argv)
{
    ul_t till = (argc > 1 ? strtoul(argv[1], 0, 0) : 0x20);
    const vul_t &primes = get_primes(till);
    for (unsigned i = 0; i < primes.size(); ++i)
    {
        cout << "P["<<i<<"] = " << primes[i] << "\n";
    }
    return 0;
}

#endif
