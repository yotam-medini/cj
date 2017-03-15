#include <cstdlib>
#include <gmpxx.h>
#include <iostream>
#include <map>
#include <string>
#include <utility>

using namespace std;

typedef mpz_class mpzc_t;

typedef pair<unsigned, unsigned> uu_t;
typedef map<uu_t, mpzc_t> uu2mpz_t;
typedef uu2mpz_t::value_type uu2mpzv_t;

static const mpzc_t& choose(const uu2mpz_t& cd, unsigned m, unsigned n)
{
    n = min(n, m - n);
    auto w = cd.find(uu_t(m, n));
    const mpzc_t& ret = w->second;
    return ret;
}

int main(int argc, char** argv)
{
    int rc = 0;
    uu2mpz_t cd;
    unsigned limit = argc > 1 ? strtoul(argv[1], 0, 0) : 6;
cerr << "limit=" << limit << "\n";
    cd.insert(cd.end(), uu2mpzv_t(uu_t(1, 0), 1));
    cout << "1 0 1\n";
    for (unsigned m = 2; m <= limit; ++m)
    {
        unsigned mm1 = m - 1;
        unsigned half = m/2;
        for (unsigned n = 0; n <= half; ++n)
        {
            mpzc_t v = (choose(cd, mm1, n) * m) / (m - n);
            cout << m << ' ' << n << ' ' << v.get_str(16) << "\n";
        }
    }
    return rc;
}
