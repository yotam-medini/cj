// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned long ul_t;
typedef vector<ul_t> vul_t;
typedef vector<unsigned> vuints_t;
typedef map<ul_t, ul_t> ul2ul_t;

enum {BASE_MAX = 10};
ul2ul_t bases2happy[BASE_MAX]; // bases via bits

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

static void happy_bases_find()
{
    ul_t n;
    for (n = 2; bases2happy[BASE_MAX - 1].empty(); ++n)
    {
        if ((n & (n-1)) == 0) { cerr << "n="<<n<<"\n"; }
        ul_t curr_happy_bases = 0;
        unsigned sz = 0;
        for (unsigned base = 2; base <= BASE_MAX; ++base)
        {
            bool bhappy = is_happy(n, base);
            if (bhappy)
            {
                curr_happy_bases |= (1u << base);
                ++sz;
            }
        }
        if (sz > 0)
        {
            ul2ul_t& b2h = bases2happy[sz];
            auto w = b2h.find(curr_happy_bases);
            if (w == b2h.end())
            {
                ul2ul_t::value_type v(curr_happy_bases, n);
                b2h.insert(b2h.end(), v);
            }
        }
    }
    cerr << "All bases are happy with " << (n-1) << "\n";
}

static ul_t comb2bits(const vuints_t vb)
{
    ul_t ret = 0;
    for (auto i = vb.begin(), e = vb.end(); i != e; ++i)
    {
        unsigned base = (*i + 2);
        ret |= (1ul << base);
    }
    return ret;
}

static void happy_bases_complete()
{
    for (unsigned sz = 1; sz <= BASE_MAX - 1; ++sz)
    {
        vuints_t bases_m2;
        combination_first(bases_m2, BASE_MAX-1, sz);
        do
        {
            ul_t bases = comb2bits(bases_m2);
            ul2ul_t& b2h = bases2happy[sz];
            auto w = b2h.find(bases);
            if (w == b2h.end())
            {
                bool found = false;
                for (unsigned szup = sz + 1; (szup <= BASE_MAX) && !found;
                    ++szup)
                {
                    const ul2ul_t& ub2h = bases2happy[szup];
                    vuints_t up_bases_m2;
                    ul_t up_happy = ul_t(-1);
                    combination_first(up_bases_m2, BASE_MAX-1, szup);
                    do
                    {
                        ul_t up_bases = comb2bits(up_bases_m2);
                        auto upw = ub2h.find(up_bases);
                        if (upw != ub2h.end())
                        {
                            ul_t h = (*upw).second;
                            if (up_happy > h)
                            {
                                up_happy = h;
                            }
                        }
                        
                    } while (combination_next(up_bases_m2, BASE_MAX-1));
                    if (up_happy != ul_t(-1))
                    {
                         found = true;
                         b2h.insert(ul2ul_t::value_type(bases, up_happy));
                    }
                }
            }

        } while (combination_next(bases_m2, BASE_MAX-1));
    }
}

static void happy_bases_out()
{
    for (unsigned sz = 1; sz <= BASE_MAX-1; ++sz)
    { 
        const ul2ul_t& ub2h = bases2happy[sz];
        for (auto i = ub2h.begin(), e = ub2h.end(); i != e; ++i)
        {
            const ul2ul_t::value_type& v = *i;
            cout << "0x" << hex << v.first << " " << dec << v.second << "\n";
        }
    }
}

int main(int argc, char **argv)
{
    int rc = 0;
    happy_bases_find();
    happy_bases_complete();
    happy_bases_out();
    return rc;
}

