#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

typedef unsigned long long ull_t;

ull_t sqroot(ull_t n)
{
    ull_t r = 0;
    if (n > 0)
    {
        r = 1;
        for (bool iter = true; iter; )
        {
            ull_t d = n / r;
            ull_t rmin = min(r, d);
            ull_t rmax = max(r, d);
            if (rmax - rmin > 1)
            {
                ull_t rnext = (r + d)/2;
                r = rnext;
            }
            else
            {
                iter = false;
                r =  (rmax * rmax <= n) ? rmax : rmin;
            }
        }
    }
    return r;
}

int main(int argc, char**argv)
{
    int rc = 0;
    for (int ai = 1; (rc == 0) && (ai < argc); ++ai)
    {
         ull_t n = strtoul(argv[ai], nullptr, 0);
         ull_t r = sqroot(n);
         bool good = (r*r <= n) && (n < (r + 1)*(r + 1));
         if (good)
         {
             cout << "sqroot(" << n << ") = " << r << '\n';
         }
         else
         {
             rc = 1;
             cerr << "Error: sqroot(" << n << ") returned " << r << '\n';
         }
    }
    return rc;
}
