// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;

static unsigned dbg_flags;

class Xwhat
{
 public:
    Xwhat(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool xor_even(u_t x);
    u_t n, q;
    vu_t a;
    vu_t p;
    vu_t v;
    vu_t solution;
};

Xwhat::Xwhat(istream& fi)
{
    fi >> n >> q;
    a.reserve(n);
    p.reserve(q);
    v.reserve(q);
    u_t x;
    for (u_t i = 0; i < n; ++i)
    {
        fi >> x;
        a.push_back(x);
    }
    for (u_t i = 0; i < q; ++i)
    {
        fi >> x;
        p.push_back(x);
        fi >> x;
        v.push_back(x);
    }
}

void Xwhat::solve_naive()
{
    vu_t as(a);
    for (u_t i = 0; i < q; ++i)
    {
        as[p[i]] = v[i];
        u_t maxlen = 0;
        for (u_t b = 0; b < n; ++b)
        {
            for (u_t e = b + 1; e < n + 1; ++e)
            {
                u_t x = 0;
                for (u_t j = b; j < e; ++j)
                {
                    x ^= as[j];
                }
                if (xor_even(x) && (maxlen < (e - b)))
                {
                    maxlen = e - b;
                }
            }
        }
        solution.push_back(maxlen);
    }
}

bool Xwhat::xor_even(u_t x)
{
    u_t nbits = 0;
    for (u_t i = 0; i <= 10; ++i)
    {
        if (x & (1u << i))
        {
            ++nbits;
        }
    }
    bool ret = ((nbits % 2) == 0);
    return ret;
}

#if 0
void Xwhat::solve()
{
    solve_naive();
}

#else 

void Xwhat::solve()
{
    // solve_naive();
    u_t max_even = 0;
    u_t max_odd = 0;
#if 0
    for (u_t i = 0; i < n; ++i)
    {
        if (xor_even(a[i]))
        {
            ++max_even;
        }
        else
        {
            ++max_odd;
        }
    }
#else
    for (u_t b = 0; b < n; ++b)
    {
        for (u_t e = b + 1; e <= n; ++e)
        {
            u_t x = 0;
            for (u_t i = b; i < e; ++i)
            {
                x ^= a[i];
            }
            u_t sz = e - b;
            if (xor_even(x))
            {
                if (max_even < sz)
                {
                    max_even = sz;
                }
            }
            else
            {
                if (max_odd < sz)
                {
                    max_odd = sz;
                }
            }
        }
    }
#endif
    solution.reserve(q);
    vu_t as(a);
    for (u_t i = 0; i < q; ++i)
    {
        u_t change = v[i] ^ as[p[i]];
        as[p[i]] = v[i];
        if (!xor_even(change))
        {
            swap(max_even, max_odd);
        }
        solution.push_back(max_even);
    }
}
#endif

void Xwhat::print_solution(ostream &fo) const
{
    for (u_t i = 0; i < q; ++i)
    {
        fo << ' ' << solution[i];
    }
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

    void (Xwhat::*psolve)() =
        (naive ? &Xwhat::solve_naive : &Xwhat::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Xwhat xwhat(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (xwhat.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        xwhat.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
