#include <vector>
#include <iostream>

using namespace std;

typedef unsigned long long ull_t;
typedef vector<ull_t> vull_t;

static const ull_t MOD_BIG = 1000000007;

ull_t power_mod(ull_t n, ull_t p)
{
    ull_t ret = 1;
    ull_t p2 = n;
    while (p)
    {
        if (p & 1)
        {
            ret = (ret * p2) % MOD_BIG;
        }
        p2 = (p2 * p2) % MOD_BIG;
        p = p / ull_t(2);
    }
    return ret;
}


ull_t invmod(ull_t n)
{
    static vull_t _invmod;
    if (n > 1000000)
    {
        cerr << __func__ << " too large n="<<n << '\n'; 
        exit(1);
    }
    if (_invmod.size() <= n)
    {
        _invmod.clear();
        _invmod.insert(_invmod.end(), size_t(n + 1), 0);
        
        const ull_t MOD_BIGm1 = MOD_BIG - 1;
        _invmod[0] = 0; // infinity
        _invmod[1] = 1; 
        for (ull_t g = 5, p = 1; (g != 1); ++p)
        {
            if ((p & (p-1)) == 0) { cerr << "p="<<p << '\n'; }
            if (g <= n)
            {
                _invmod[g] = power_mod(5, MOD_BIGm1 - p);
            }
            g = (5*g) % MOD_BIG;
        }
    }
    ull_t ret = _invmod[n];
    return ret;
}

int main(int argc, char** argv)
{
    int rc = 0;
    for (int ai = 1; (ai < argc) && (rc == 0); ++ai)
    {
        const ull_t n = strtoul(argv[ai], 0, 0);
        const ull_t ninv = invmod(n);
        cout << "invmod(" << n << ") = " << ninv << '\n';
        const ull_t prod = (n * ninv) % MOD_BIG;
        if (prod != 1)
        {
            rc = 1;
            cerr << "Bad inverse\n";
        }
    }
    return rc;
}
