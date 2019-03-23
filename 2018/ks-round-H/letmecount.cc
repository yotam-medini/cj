// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <utility>
#include <numeric>
#include <algorithm>

#include <cstdlib>
// #include <gmpxx.h> // Test Google system

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;
typedef pair<u_t, u_t> uu_t;
typedef map<uu_t, ul_t> uu2ul_t;

static unsigned dbg_flags;

enum { MOD_BIG = 1000000007 };

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

u_t choose(u_t n, u_t k)
{
    if (k + k > n)
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
    ull_t _r = 1;
    for (u_t j = 0; j < high.size(); ++j)
    {
        ull_t h = high[j];
        _r = (_r * h) % MOD_BIG;
    }
    u_t r = _r;
    return r;
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

static bool good_permutation(vu_t p, u_t m)
{
    bool good = true;
    for (u_t i = 0, i1 = 1, psz = p.size(); good && (i1 < psz); i = i1++)
    {
        good = (p[i] >= 2*m) || ((p[i] ^ p[i + 1]) != 1);
    }
    return good;
}

static ul_t n_good_permutations(u_t n, u_t m)
{
    ul_t n_good = 0;
    vu_t p;
    permutation_first(p, 2*n);
    do
    {
        if (good_permutation(p, m))
        {
            ++n_good;
        }
    } while (permutation_next(p));
    
    return n_good;
}

static ull_t n_perm_push(u_t n_in, u_t m_pending)
{
    ull_t ret = 1;
    if (m_pending > 0)
    {
        ull_t sub1 = (n_in - 1) % MOD_BIG;
        sub1 = sub1 * n_perm_push(n_in + 1, m_pending - 1);
        ull_t sub2 = 0;
        if (m_pending >= 2)
        {
            sub2 = (2 * (m_pending - 1)) % MOD_BIG;
            sub2 = sub2 * n_perm_push(n_in + 1, m_pending - 2);
        }
        ret = (sub1 + sub2) % MOD_BIG;
    }
    return ret;
}

static ul_t n_good_permutations_big(u_t n, u_t m)
{
    u_t n_in = 2*(n - m) + m;
    ul_t cur = 1;
    for (ul_t x = 1; x <= n_in; ++x)
    {
        cur = (cur * x) % MOD_BIG;
    }

    cur = (cur * n_perm_push(n_in, m)) % MOD_BIG;
    return cur;
}

static void test_small()
{
    for (u_t n = 1; n <= 5; ++n)
    {
        for (u_t m = 0; m <= n; ++m)
        {
            ul_t y = n_good_permutations(n, m);
            cerr << "Good("<<n<<", "<<m<<")="<<y<<'\n';
            ul_t yb = n_good_permutations_big(n, m);
            if (y != yb)
            {
                cerr << "Error: BigGood="<<yb<<"\n";
                exit(1);
            }
        }
    }
}

class LetMeCount
{
 public:
    LetMeCount(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    static uu2ul_t memo;
    static vull_t factorial;
    static vull_t twop;
    ul_t n_perm_insertions(u_t n_in, u_t m_pending);
    void init_statics();
    ull_t n_adjacent(u_t k, ull_t choose_Nmk_k, ull_t choose_m_k) const;
    ul_t solution;
    u_t n, m;
};
uu2ul_t LetMeCount::memo;
vull_t LetMeCount::factorial;
vull_t LetMeCount::twop;

LetMeCount::LetMeCount(istream& fi) : solution(0)
{
    fi >> n >> m;
}

void LetMeCount::solve_naive()
{
    if (dbg_flags & 0x2)
    {
        solution = n_good_permutations(n, m);
    }
    else
    {
        u_t n_in = 2*(n - m) + m;
        ul_t cur = 1;
        for (ul_t x = 1; x <= n_in; ++x)
        {
            cur = (cur * x) % MOD_BIG;
        }
        solution = (cur * n_perm_insertions(n_in, m)) % MOD_BIG;
    }
}

ul_t LetMeCount::n_perm_insertions(u_t n_in, u_t m_pending)
{
    ul_t ret = 1;
    if (m_pending > 0)
    {
        uu2ul_t::key_type key(n_in, m_pending);
        auto er = memo.equal_range(key);
        if (er.first == er.second)
        {
            ull_t ins1 = (n_in - 1) % MOD_BIG;
            ins1 = (ins1 * n_perm_insertions(n_in + 1, m_pending - 1)) %
                MOD_BIG;
            ull_t ins2 = 0;
            if (m_pending >= 2)
            {
                ins2 = (2 * (m_pending - 1)) % MOD_BIG;
                ins2 = (ins2 * n_perm_insertions(n_in + 1, m_pending - 2)) %
                    MOD_BIG;
            }
            ret = (ins1 + ins2) % MOD_BIG;
            memo.insert(er.first, uu2ul_t::value_type(key, ret));
        }
        else
        {
            ret = (*er.first).second;
        }
    }
    return ret;
}

void LetMeCount::solve()
{
    init_statics();
    ull_t inex = factorial[2*n];
    bool do_add = false;
    ull_t N = 2*n;
    ull_t choose_Nmk_k = choose(N - 0, 0);
    ull_t choose_m_k = choose(m, 0);
    for (ul_t k = 1; k <= m; ++k, do_add = !do_add)
    {
        ull_t an = ((N - k - (k - 2)) * (N - k - (k - 1)));
        ull_t ad = k * (N - (k - 1));
        choose_Nmk_k = ((an * choose_Nmk_k) / ad) % MOD_BIG;
        if (choose_Nmk_k != choose(N - k, k)) {
            cerr << "fuck\n"; }
        choose_m_k = (((m - (k - 1)) * choose_m_k) / k) % MOD_BIG;
        if (choose_m_k != choose(m, k)) {
            cerr << "fuck\n"; }
        ull_t gk = n_adjacent(k, choose_Nmk_k, choose_m_k);
        if (do_add)
        {
            inex = (inex + gk) % MOD_BIG;
        }
        else
        {
            inex = (inex + (MOD_BIG - gk)) % MOD_BIG;
        }
    }
    solution = inex;
}

void LetMeCount::init_statics()
{
    factorial.reserve(max(u_t(factorial.size()), 2*n + 1));
    twop.reserve(max(u_t(twop.size()), m + 1));
    if (factorial.empty())
    {
        factorial.push_back(1); // 0!
        factorial.push_back(1); // 1:
    }
    if (twop.empty())
    {
        twop.push_back(1); // 2^0
    }
    for (ul_t k = factorial.size(); k <= 2*n; ++k)
    {
        factorial.push_back((k * factorial.back()) % MOD_BIG);
    }
    for (ul_t k = twop.size(); k <= m; ++k)
    {
        twop.push_back((2 * twop.back()) % MOD_BIG);
    }
}

ull_t LetMeCount::n_adjacent(u_t k, ull_t choose_Nmk_k, ull_t choose_m_k) const
{
    u_t N = 2*n;
    // ull_t choose_Nmk_k = choose(N - k, k);
    // ull_t choose_m_k = choose(m, k);
    ull_t fact_k = factorial[k];
    ull_t twop_k = twop[k];
    ull_t fact_N_m2k = factorial[N - 2*k];
    ull_t ret = (choose_Nmk_k * choose_m_k) % MOD_BIG;
    ret = (ret * fact_k) % MOD_BIG;
    ret = (ret * twop_k) % MOD_BIG;
    ret = (ret * fact_N_m2k) % MOD_BIG;
    return ret;
}

void LetMeCount::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-debug"))
        {
            dbg_flags = strtoul(argv[++ai], 0, 0);
        }
        else if (opt == string("-tellg"))
        {
            tellg = true;
        }
        else
        {
            cerr << "Bad option: " << opt << "\n";
            return 1;
        }
    }

    if (dbg_flags & 0x1) { test_small(); }
    int ai_in = ai;
    int ai_out = ai + 1;
    istream *pfi = (argc <= ai_in || (string(argv[ai_in]) == dash))
         ? &cin
         : new ifstream(argv[ai_in]);
    ostream *pfo = (argc <= ai_out || (string(argv[ai_out]) == dash))
         ? &cout
         : new ofstream(argv[ai_out]);

    if ((!pfi) || (!pfo) || pfi->fail() || pfo->fail())
    {
        cerr << "Open file error\n";
        exit(1);
    }

    string ignore;
    unsigned n_cases;
    *pfi >> n_cases;
    getline(*pfi, ignore);

    void (LetMeCount::*psolve)() =
        (naive ? &LetMeCount::solve_naive : &LetMeCount::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        LetMeCount letMeCount(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (letMeCount.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        letMeCount.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
