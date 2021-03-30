#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef vector<u_t> vu_t;

bool subsetsum_rec(vu_t& subset, const vu_t& a, u_t sz, u_t sum)
{
    bool ret = true;
    if (sum > 0)
    {
        ret = false;
        if (sz > 0)
        {
           u_t last = a[sz - 1];
           if (last <= sum)
           {
               subset.push_back(last);
               ret = subsetsum_rec(subset, a, sz - 1, sum - last);
               if (!ret)
               {
                   subset.pop_back();
               }
           }
           if (!ret)
           {
               ret = subsetsum_rec(subset, a, sz - 1, sum);
           }
        }
    }
    return ret;
}

bool subsetsum(vu_t& subset, const vu_t& a, u_t sum)
{
    vu_t sa(a);
    sort(sa.begin(), sa.end());
    subset.clear();
    bool ret = subsetsum_rec(subset, sa, a.size(), sum);
    return ret;
}

int main(int argc, char** argv)
{
    u_t sum = stoi(argv[1]);
    vu_t a, subset;
    for (int ai = 2; ai < argc; ++ai)
    {
        a.push_back(stoi(argv[ai]));
    }
    bool can = subsetsum(subset, a, sum);
    if (can)
    {
        cout << sum << " = ";
        const char* sep = "";
        for (u_t x: subset)
        {
            cout << sep << x;  sep = " + ";
        }
        cout << "\n";
    }
    else
    {
        cout << "Impossible\n";
    }
    return 0;
}
