#include <vector>

using namespace std;

typedef unsigned long long ull_t;
typedef vector<ull_t> vull_t;

ull_t pi(ull_t x)
{
    // log(2) = 0.6931471805599453
    ull_t log2 = 0;
    for (; (1ull << log2) < x; ++log2) {}
    --log2;
    ull_t ret = (1000 * x) / (690 * log2);
    return ret;
}

void get_primes(vull_t& primes, ull_t till)
{
    // Eratosthenes
    vector<bool> sieve(till + 1, true);
    for (ull_t n = 2; n <= till; ++n)
    {
        if (sieve[n])
        {
            primes.push_back(n);
            for (ull_t s = n*n; s <= till; s += n)
            {
                sieve[s] = false;
            }
        }
    }
}

#include <iostream>
#include <cstdlib>

int main(int argc, char **argv)
{
    ull_t till = strtoull(argv[1], 0, 0);
    vull_t primes;
    cout << "pi(" << till << ") = " << pi(till) << '\n';
    get_primes(primes, till);
    cout << "#(primes < " <<  till << ") = " << primes.size() << '\n';
    for (ull_t p: primes)
    {
        cout << p << '\n';
    }
}
