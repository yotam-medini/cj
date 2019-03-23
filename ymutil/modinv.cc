#include <vector>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;

typedef vector<ul_t> vul_t;

ull_t power_mod(ull_t x, ul_t pwr, ul_t pmod)
{
    ul_t ret = 1;
    if (pwr > 0)
    {
        ul_t phalf = power_mod(x, pwr/2, pmod);
        ret = (phalf * phalf) % pmod;
        if ((pwr % 2) != 0)
        {
            ret = (x * ret) % pmod;
        }
    }
    return ret;
}

void get_modinv(vul_t &inv, ul_t pmod)
{
    inv.clear();
    inv.reserve(pmod);
    inv.push_back(0);
    for (ul_t k = 1; k < pmod; ++k)
    {
        ul_t invk = power_mod(k, pmod - 2, pmod);
        inv.push_back(invk);
    }
}

#if 1 || defined(POWERMOD)
#include <iostream>
#include <cstdlib>

int main(int argc, char **argv)
{
    ul_t pmod = strtoul(argv[1], 0, 0);
    vul_t inv;
    get_modinv(inv, pmod);
    for (ul_t k = 1; k < pmod; ++k)
    {
        cout << "inv("<<k<<")=" << inv[k] << "\n";
    }
    return 0;
}

#endif
