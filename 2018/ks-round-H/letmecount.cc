// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <vector>
#include <utility>
#include <numeric>
#include <algorithm>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;

static unsigned dbg_flags;

enum { MOD_BIG = 1000000007 };

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


bool good_permutation(vu_t p, u_t m)
{
    bool good = true;
    for (u_t i = 0, i1 = 1, psz = p.size(); good && (i1 < psz); i = i1++)
    {
        good = (p[i] >= 2*m) || ((p[i] ^ p[i + 1]) != 1);
    }
    return good;
}

ul_t n_good_permutations(u_t n, u_t m)
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

ul_t n_good_permutations_big(u_t n, u_t m)
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

class LetMeCount
{
 public:
    LetMeCount(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ul_t solution;
    u_t n, m;
};

LetMeCount::LetMeCount(istream& fi) : solution(0)
{
    fi >> n >> m;
}

void LetMeCount::solve_naive()
{
    solution = n_good_permutations(n, m);
}

void LetMeCount::solve()
{
    solve_naive();
}

void LetMeCount::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

int main(int argc, char ** argv)
{
    const string dash("-");

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
