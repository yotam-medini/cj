// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2014/May/03

#include <gmpxx.h>

#include <iostream>
#include <algorithm>
#include <vector>
#include <utility>
#include <map>
#include <set>
#include <numeric>

using namespace std;

typedef unsigned u_t;
typedef pair<u_t, u_t> uu_t;
typedef vector<u_t> vu_t;
typedef std::multiset<unsigned> msuints_t;

u_t gcd(u_t m, u_t n)
{
   while (n)
   {
      u_t t = n;
      n = m % n;
      m = t;
   }
   return m;
}

u_t lcm(u_t m, u_t n)
{
    return (m*n)/gcd(m, n);
}

u_t factorial(u_t n)
{
    u_t f = 1;
    ++n;
    while (--n)
    {
        f *= n;
    }
    return f;
}

u_t choose(u_t n, u_t k)
{
    if (k > n/2)
    {
        k = n - k;
    }
    vu_t high, low;
    high = vu_t(vu_t::size_type(k), 0);
    iota(high.begin(), high.end(), n - k + 1);
    u_t d = 2;
    while (d <= k)
    {
        u_t dd = d;
        for (vu_t::iterator i = high.begin(), e = high.end();
            (dd > 1) && (i != e); ++i)
        {
            u_t g = gcd(dd, *i);
            if (g > 1)
            {
                *i /= g;
                dd /= g;
            }
        }
        ++d;
    }
    u_t r =
        accumulate(high.begin(), high.end(), 1, multiplies<u_t>());
    return r;
}

u_t choose_cache(u_t n, u_t k)
{
    typedef map<uu_t, u_t> uu2u_t;
    static uu2u_t nk2c;

    u_t ret = 0;
    uu2u_t::key_type nk(n, k);
    pair<uu2u_t::iterator, uu2u_t::iterator> w = nk2c.equal_range(nk);
    if (w.first == w.second)
    {
        ret = choose(n, k);
        uu2u_t::value_type v(nk, ret);
        nk2c.insert(w.first, v);
    }
    else
    {
        ret = (*w.first).second;
    }
    return ret;
}


mpz_class gcd(mpz_class m, mpz_class n)
{
   while (n != 0)
   {
      mpz_class t(n);
      n = m % n;
      m = t;
   }
   return m;
}

mpz_class lcm(mpz_class m, mpz_class n)
{
    return (m*n)/gcd(m, n);
}

void permutation_first(vu_t &p, u_t n)
{
    p = vu_t(n);
    iota(p.begin(), p.end(), 0);
}

bool permutation_next(vu_t &p)
{
    u_t n = p.size();

    // find the last 'in order'
    int j = n - 1 - 1;
    while ((j >= 0) && (p[j] > p[j + 1]))
    {
        --j;
    }
    bool ret = j >= 0;
    if (ret)
    {
        // Find last > p[j]. Must find since p[j] < p[j+1]
        u_t l;
        for (l = n - 1; p[j] >= p[l]; --l);
        swap(p[j], p[l]);
        reverse(p.begin() + j + 1, p.end());
    }
    return ret;
}

void combination_first(vu_t &c, u_t n, u_t k)
{
    c = vu_t(k);
    iota(c.begin(), c.end(), 0);
}

bool combination_next(vu_t &c, u_t n)
{
    u_t j = 0, k = c.size();

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

void multiset_first(vu_t &bars, u_t n, u_t k)
{
    bars = vu_t(n - 1);
    iota(bars.begin(), bars.end(), 0);
}

bool multiset_next(vu_t &bars, u_t k)
{
    return combination_next(bars, bars.size() + k);
}

static void ms_insert(msuints_t &m, u_t val, int count)
{
   for ( ; count > 0; --count)
   {
       m.insert(m.end(), val);
   }
}

void bars2multiset(msuints_t &m, const vu_t &bars, u_t k)
{
    m.clear();
    u_t last_vi = 0;
    u_t val = 0;
    for (vu_t::const_iterator i = bars.begin(), e = bars.end();
        i != e; ++i, ++val)
    {
        u_t vi = *i;
        ms_insert(m, val, vi - last_vi - 1);
        last_vi = vi;
    }
    ms_insert(m, val, bars.size() + k - last_vi - 1);
}

void bars2multiset(vu_t &m, const vu_t &bars, u_t k)
{
    m.clear();
    u_t last_vi = 0;
    u_t val = 0;
    for (vu_t::const_iterator i = bars.begin(), e = bars.end();
        i != e; ++i, ++val)
    {
        u_t vi = *i;
        vu_t::size_type count = vi - last_vi;
        m.insert(m.end(), count, val);
        last_vi = vi + 1;
    }
    vu_t::size_type count = bars.size() + k - last_vi;
    m.insert(m.end(), count, val);
}

void v2m(msuints_t &m, const vu_t &v)
{
    m.clear();
    for (vu_t::const_iterator i = v.begin(), e = v.end(); i != e; ++i)
    {
       m.insert(m.end(), *i);
    }
}

void m2v(vu_t &v, const msuints_t &m)
{
    v.clear();
    v.reserve(m.size());
    v.insert(v.end(), m.begin(), m.end());
}

int main(int argc, char **argv)
{
    int rc = 0;
    for (u_t n = 0; (rc == 0) && (n <= 6); ++n)
    {
        u_t f = factorial(n);
        u_t np = 1;
        vu_t p;
        permutation_first(p, n);
        while (permutation_next(p)) { ++np; }
        if (np != f)
        {
            cerr << "Bad #(permutation)=" << np << "!=" << f << "\n";
            rc = 1;
        }
    }

    for (u_t n = 0; (rc == 0) && (n <= 10); ++n)
    {
        for (u_t k = 0; (rc == 0) && (k <= n); ++k)
        {
            u_t cnk = choose(n, k);
            u_t dff = factorial(k)*factorial(n - k);
            if (cnk * dff != factorial(n))
            {
                cerr << "failure: choose("<<n<<", "<<k<<")\n";
                rc = 1;
            }
            vu_t comb;
            u_t nc = 1;
            combination_first(comb, n, k);
            while (combination_next(comb, n))
            {
                ++nc;
            }
            if (nc != cnk)
            {
                cerr << "failure: choose("<<n<<", "<<k<<")="<<cnk <<
                    " != #(combinations)=" << nc << "\n";
                rc = 1;
            }
        }
    }
    return rc;
}