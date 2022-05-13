#include <cstdlib>
typedef long long int ll_t;
typedef unsigned long long int ull_t;

// See: https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm
ll_t extended_gcd(ll_t& bezout_x, ll_t& bezout_y, const ll_t a, const ll_t b)
{
    
    ll_t s = 0, old_s = 1;
    ll_t r = b, old_r = a;
         
    while (r != 0)
    {
        div_t div_result = div(old_r, r);
        ll_t quotient = div_result.quot;
        ll_t smod = old_s - quotient * s;
        old_r = r;  r = div_result.rem;
        old_s = s;  s = smod;
    }

    bezout_x = old_s;
    bezout_y = (b ? (old_r - old_s * a) / b : 0);
    return ((a == 0) && (b == 0) ? 0 : old_r);
}

ull_t invmod(ull_t a, ull_t m)
{
    ll_t x, y;
    extended_gcd(x, y, a, m);
    ull_t inv = (x + m) % m;
    return inv;
}
    
#include <iostream>
#include <string>

using namespace std;

static int test_ab(ll_t a, ll_t b, bool verbose=false)
{
    int rc = 0;
    ll_t bezout_x, bezout_y, r;
    r = extended_gcd(bezout_x, bezout_y, a, b);
    if (verbose)
    {
        std::cout << "gcd(" << a << ", " << b << ") = " << r <<
            "    Bezout= (" << bezout_x << ", " << bezout_y << ")\n";
    }
    if ((a == 0) && (b == 0))
    {
        ; // ignore
    }
    else if ((a % r != 0) || (b % r != 0))
    {
        rc = 1;
    }
    else if (bezout_x*a + bezout_y*b != r)
    {
        rc = 1;
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;
    if (argc == 3)
    {
        ll_t a = strtol(argv[1], nullptr, 0);
        ll_t b = strtol(argv[2], nullptr, 0);
        rc = test_ab(a, b, true);
    }
    else if ((argc >= 4) && (string(argv[1]) == string("invmod")))
    {
        ll_t m = strtoul(argv[2], nullptr, 0);
        for (int ai = 3; ai < argc; ++ai)
        {
            ll_t a = strtoul(argv[ai], nullptr, 0);
            ull_t inv = invmod(a, m);
            cout << "invmod(" << a << ", " << m << ") = " << inv << '\n';
        }
    }
    else
    {
        for (ll_t a = -20; (rc == 0) && (a <= 20); ++a)
        {
            for (ll_t b = -20; (rc == 0) && (b <= 20); ++b)
            {
                rc = test_ab(a, b);
            }
        }
    }
    return rc;
}

