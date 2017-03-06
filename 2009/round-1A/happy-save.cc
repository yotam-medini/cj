// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef vector<ul_t> vul_t;
typedef vector<unsigned> vuints_t;
typedef set<ul_t> set_ul_t;
typedef map<vuints_t, ul_t> vu2ul_t;

vu2ul_t bases2happy[10];

static void iota(vuints_t& v, unsigned n, unsigned val=0)
{
    v.clear();
    v.reserve(n);
    for (unsigned k = 0; k < n; ++k, val++)
    {
        v.push_back(val);
    }
}

static void combination_first(vuints_t &c, unsigned n, unsigned k)
{
    iota(c, k, 0);
}

static bool combination_next(vuints_t &c, unsigned n)
{
    unsigned j = 0, k = c.size();

    // sentinels (Knuth) (Why the second (0) ??!)
    c.push_back(n);  c.push_back(0);

    while ((j < k) && (c[j] + 1 == c[j + 1]))
    {
        c[j] = j;
        ++j;
    }
    bool ret = j < k;
    if (ret)
    {
        ++(c[j]);
    }

    c.pop_back(); c.pop_back(); // the sentinels

    return ret;
}

static ul_t happy_iterate(ul_t n, unsigned base)
{
    ul_t next_val = 0;
    while (n)
    {
        ul_t d = n % base;
        n /= base;
        next_val += d*d;
    }
    return next_val;
}

static bool is_happy(ul_t n, unsigned base)
{
    ul_t slow = n;
    bool ret = false;
    do
    {
        n = happy_iterate(n, base);
        ret = (n == 1);
        n = happy_iterate(n, base);
        ret = ret || (n == 1);
        slow = happy_iterate(slow, base);
    } while ((slow != n) && !ret);
    return ret;
}

enum {BASE_MAX = 9};
static void happy_bases_find()
{
    ul_t n;
    for (n = 2; bases2happy[BASE_MAX - 1].empty(); ++n)
    {
        if ((n & (n-1)) == 0) { cerr << "n="<<n<<"\n"; }
        vuints_t curr_happy_bases;
        for (unsigned base = 2; base <= BASE_MAX; ++base)
        {
            bool bhappy = is_happy(n, base);
            if (bhappy)
            {
                curr_happy_bases.push_back(base);
            }
        }
        unsigned sz = curr_happy_bases.size();
        if (sz > 0)
        {
            vu2ul_t& b2h = bases2happy[sz];
            auto w = b2h.find(curr_happy_bases);
            if (w == b2h.end())
            {
                vu2ul_t::value_type v(curr_happy_bases, n);
                b2h.insert(b2h.end(), v);
            }
        }
    }
    cerr << "All bases are happy with " << (n-1) << "\n";
}

static void happy_bases_complete()
{
    for (unsigned sz = 1; sz <= BASE_MAX; ++sz)
    {
        vuints_t bases_m2;
        combination_first(bases_m2, BASE_MAX-2, sz);
        do
        {
            vuints_t bases(vuints_t::size_type(sz), 0);
            for (unsigned k = 0; k < sz; ++k)
            {
                bases[k] = bases_m2[k] + 2;
            }
            vu2ul_t& b2h = bases2happy[sz];
            auto w = b2h.find(bases);
            if (w == b2h.end())
            {
                cerr << "Need to complete";
                for (k = 0; k < bases.size(); ++k) {
                     cerr << " " << bases[k];
                } cerr << "\n";
                bool found = false;
                for (unsigned szup = sz + 1; (sz <= BASE_MAX) && !found;
                    ++szup)
                {
                    
                }
            }

        } while (combination_next(bases_m2, 10-2));
    }
}

int main(int argc, char **argv)
{
    int rc = 0;
    happy_bases_find();
    happy_bases_complete();
    return rc;
}

