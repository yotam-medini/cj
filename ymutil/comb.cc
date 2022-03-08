// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2014/May/03

// #include <gmpxx.h>

#include <iostream>
#include <algorithm>
#include <array>
#include <vector>
#include <utility>
#include <map>
#include <set>
#include <sstream>
#include <numeric>

using namespace std;

typedef unsigned u_t;
typedef pair<u_t, u_t> uu_t;
typedef vector<u_t> vu_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;
typedef std::multiset<u_t> msuints_t;

static const string vu_to_str(const vu_t& a)
{
    ostringstream os;
    os << '{';
    const char* sep = "";
    for (u_t x: a)
    {
        os << sep << x; sep = " ";
    }
    os << '}';
    string ret = os.str();
    return ret;
}

#if 0
static const string ms_to_str(const msuints_t& m)
{
    ostringstream os;
    os << '{';
    const char* sep = "";
    for (u_t x: m)
    {
        os << sep << x; sep = " ";
    }
    os << '}';
    string ret = os.str();
    return ret;
}

static const string vau2_to_str(const vau2_t& vau2)
{
    ostringstream os;
    os << '{';
    const char* sep = "";
    for (const au2_t& x: vau2)
    {
        os << sep << x[0] << '*' << x[1]; sep = " ";
    }
    os << '}';
    string ret = os.str();
    return ret;
}
#endif

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

#if 0
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
#endif

// caller should initialize t = vu_t(bound.size(), 0)
void tuple_next(vu_t& t, const vu_t& bound)
{
    u_t i, sz = bound.size();
    bool done = false;
    for (i = 0; (i < sz) && !done; ++i)
    {
        ++t[i];
        done = (t[i] < bound[i]);
        if (!done)
        {
            fill_n(t.begin(), i + 1, 0);
        }
    }
    if (!done)
    {
        t.clear();
    }
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

class MultiChoose
{
 public:
    MultiChoose(u_t _n, u_t _k) : n(_n), k(_k)
    {
        bars = vu_t(size_t(n - 1));
        iota(bars.begin(), bars.end(), 0);
        bars2m();
    }
    const vu_t& current() const { return m; }
    const vu_t& next() // empty if end
    {
        m.clear();
        if (combination_next(bars, n + k - 1))
        {
            bars2m();
        }
        return m;
    }
 private:
    void bars2m()
    {
        m.insert(m.end(), size_t(bars[0]), 0);
        for (u_t v = 1; v < n - 1; ++v)
        {
            m.insert(m.end(), size_t(bars[v] - bars[v - 1] - 1), v);
        }
        m.insert(m.end(), size_t(k - m.size()), n - 1);
    }
    u_t n, k;
    vu_t bars;
    vu_t m;
};

class Composition
{
 public:
    Composition(u_t _n, u_t _k) : n(_n), k(_k)
    {
        bars = vu_t(size_t(k - 1));
        iota(bars.begin(), bars.end(), 0);
        bars2m();
    }
    const vu_t& current() const { return m; }
    const vu_t& next() // empty if end
    {
        m.clear();
        if (combination_next(bars, n - 1))
        {
            bars2m();
        }
        return m;
    }
 private:
    void bars2m()
    {
        m.push_back(bars[0] + 1);
        for (u_t v = 1; v < k - 1; ++v)
        {
            u_t d = bars[v] - bars[v - 1];
            m.push_back(d);
        }
        u_t d = (n - 1) - bars.back();
        m.push_back(d);
    }
    u_t n, k;
    vu_t bars;
    vu_t m;
};

static int sanity()
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

    {
        vu_t bound{2, 3, 5};
        vu_t t{0, 0, 0};
        u_t n = 0;
        for (; !t.empty(); tuple_next(t, bound))
        {
            ++n;
        }
        if (n != 2*3*5)
        {
            cerr << "failure tuple-travese: n="<<n<<"!=2*3*5\n";
            rc = 1;
        }
    }

    return rc;
}

static int test_tuples(int argc, char **argv)
{
    int rc = 0;
    vu_t bound;
    for (int ai = 0; ai < argc; ++ai)
    {
        bound.push_back(stoi(argv[ai]));
    }
    vu_t t(bound.size(), 0);
    for (; !t.empty(); tuple_next(t, bound))
    {
        cout << vu_to_str(t) << '\n';
    }
    return rc;
}

static int test_permutations(int argc, char **argv)
{
    int rc = 0;
    u_t n = stoi(argv[0]);
    vu_t p;
    permutation_first(p, n);
    for (bool more = true; more; more = permutation_next(p))
    {
        cout << vu_to_str(p) << '\n';
    }
    return rc;
}

static int test_combinations(int argc, char **argv)
{
    int rc = 0;
    u_t n = stoi(argv[0]);
    u_t k = stoi(argv[1]);
    vu_t comb;
    combination_first(comb, n, k);
    for (bool more = true; more; more = combination_next(comb, n))
    {
        cout << vu_to_str(comb) << '\n';
    }
    return rc;
}

static int test_multichoose(int argc, char **argv)
{
    int rc = 0;
    u_t n = stoi(argv[0]);
    u_t k = stoi(argv[1]);
    MultiChoose mc(n, k);
    while (!mc.current().empty())
    {
        const vu_t& m = mc.current();
        cout << vu_to_str(m) << '\n';
        mc.next();
    }
                   
    return rc;
}

static int test_compositions(int argc, char **argv)
{
    int rc = 0;
    u_t n = stoi(argv[0]);
    u_t k = stoi(argv[1]);
    Composition composition(n, k);
    while (!composition.current().empty())
    {
        const vu_t& m = composition.current();
        cout << vu_to_str(m) << '\n';
        composition.next();
    }
                   
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;
    if (argc == 1)
    {
        rc = sanity();
    }
    else
    {
        const string test(argv[1]);   
        if (test == "tuples")
        {
            rc = test_tuples(argc - 2, argv + 2);
        }
        else if (test == "permutations")
        {
            rc = test_permutations(argc - 2, argv + 2);
        }
        else if (test == "combinations")
        {
            rc = test_combinations(argc - 2, argv + 2);
        }
        else if (test == "multichoose")
        {
            rc = test_multichoose(argc - 2, argv + 2);
        }
        else if (test == "compositions")
        {
            rc = test_compositions(argc - 2, argv + 2);
        }
        else
        {
            cerr << "Unsupported test: " << test << '\n';
            rc = 1;
        }
    }
    return rc;
}
