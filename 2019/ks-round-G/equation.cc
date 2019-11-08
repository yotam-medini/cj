// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class Equation
{
 public:
    Equation(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    static const ull_t one = 1;
    int log2floor(ull_t x) const;
    u_t log2ceil(ull_t x) const;
    ull_t n, m;
    vull_t a;
    ll_t solution;
};

// const ull_t Equation::one = 1;

Equation::Equation(istream& fi) : solution(-1)
{
    fi >> n >> m;
    a.reserve(n);
    for (ull_t i = 0; i < n; ++i)
    {
        ull_t x;
        fi >> x;
        a.push_back(x);
    }
}

void Equation::solve_naive()
{
    ull_t amax = *(max_element(a.begin(), a.end()));
    ull_t namax = 2*n* max(amax, m); // !! must also max with m 
    for (ull_t k = 0; k <= namax; ++k)
    {
        ull_t sum = 0;
        for (ull_t i = 0; (i < n) && (sum <= m); ++i)
        {
            sum += (a[i] ^ k);
        }
        if (sum <= m)
        {
            solution = k;
        }
    }
}

void Equation::solve()
{
    int highest_bit = log2floor(m);
    for (ull_t x: a)
    {
        int p = log2floor(x);
        if (highest_bit < p)
        {
            highest_bit = p;
        }
    }
    vull_t koff_valuem = vull_t(vull_t::size_type(highest_bit + 1), 0);
    vull_t kon_valuem = vull_t(vull_t::size_type(highest_bit + 1), 0);
    ull_t candid = 0;
    ull_t xsum = 0;
    for (int p = 0; p <= highest_bit; ++p)
    {
        const ull_t twop = one << p;
        u_t n_have = 0;
        for (ull_t x: a)
        {
            if (x & twop)
            {
                ++n_have;
            }
        }
        koff_valuem[p] = n_have * twop;
        kon_valuem[p] = (n - n_have) * twop;
        if (koff_valuem[p] < kon_valuem[p])
        {
            xsum += koff_valuem[p];
        }
        else
        {
            xsum += kon_valuem[p];
            candid |= twop;
        }
    }
    if (xsum <= m)
    {
        for (int p = highest_bit; p >= 0; --p)
        {
            const ull_t twop = one << p;
            if ((candid & twop) == 0)
            {
                ull_t add = kon_valuem[p] - koff_valuem[p];
                if (xsum + add <= m)
                {
                    xsum += add;
                    candid |= twop;
                }
            }
        }
        solution = candid;
    }
}

int Equation::log2floor(ull_t x) const
{
    int p = 0;
    while ((one << p) <= x)
    {
        ++p;
    }
    --p;
    return p;
}

u_t Equation::log2ceil(ull_t x) const
{
    u_t p = 0;
    while ((one << p) < x)
    {
        ++p;
    }
    return p;
}

void Equation::print_solution(ostream &fo) const
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

    void (Equation::*psolve)() =
        (naive ? &Equation::solve_naive : &Equation::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Equation equation(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (equation.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        equation.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
