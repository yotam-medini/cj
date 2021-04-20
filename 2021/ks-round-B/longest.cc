// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;
typedef vector<ll_t> vll_t;
typedef unordered_map<ll_t, u_t> ll2u_t;

static unsigned dbg_flags;

class Longest
{
 public:
    Longest(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t N;
    vll_t a;
    u_t solution;
};

Longest::Longest(istream& fi) : solution(0)
{
    fi >> N;
    copy_n(istream_iterator<ll_t>(fi), N, back_inserter(a));
}

void Longest::solve_naive()
{
    u_t b_best = 0, e_best = 0;
    for (u_t b = 0; b < N; ++b)
    {
        for (u_t e = b + 2; e <= N; ++e)
        {
            u_t sz = e - b;
            bool progression = sz <= 3;
            if (!progression)
            {
                for (u_t r = b; (r < e) && !progression; ++r)
                {
                    ll_t delta = (r > 1 ? a[1] - a[0] : a[sz - 1] - a[sz - 2]);
                    bool arith = true;
                    arith = arith && ((r == 0) || (r + 1 == sz) || 
                        (2*delta == a[r + 1] - a[r - 1]));
                    for (u_t k = b + 1; arith && (k < e); ++k)
                    {
                        arith = (k == r) || (k - 1 == r) || 
                            (delta == a[k] - a[k - 1]);
                    }
                    progression = arith;
                }
            }
            if (progression && (solution < sz))
            {
                solution = sz;
                b_best = b; e_best = e;
            }
        }
    }
    if (dbg_flags & 0x1) { cerr << "b="<<b_best << ", e="<<e_best << '\n'; }
}

void maxby(u_t& v, u_t x)
{
    if (v < x)
    {
        v = x;
    }
}

void Longest::solve()
{
    solution = (N <= 3 ? N : 3);
    if (N >= 4)
    {
        for (u_t b = 0; b + 3 < N; ++b)
        {
            for (u_t di = 1; di <= 3; ++di)
            {
                const int delta = a[b + di] - a[b + di - 1];
                if (a[b + 1] - a[b] == delta)
                {
                    u_t e = b + 1;
                    for (e = b + 1; (e < N) && (a[e] - a[e - 1]) == delta; ++e)
                    {
                        ;
                    }
                    maxby(solution, e - b);
                    if (e == N - 1)
                    {
                         maxby(solution, N - b);
                    }
                    else if ((e + 1 < N) && (a[e + 1] - a[e - 1] == 2*delta))
                    {
                        for (e += 2; (e < N) && (a[e] - a[e - 1]) == delta; ++e)
                        {
                            ;
                        }
                        maxby(solution, e - b);
                    }
                }
                else
                {
                    u_t e;
                    for (e = b + 3; (e < N) && (a[e] - a[e - 1]) == delta; ++e)
                    {
                        ;
                    }
                    maxby(solution, e - b);
                }
            }
        }
    }
}

void Longest::print_solution(ostream &fo) const
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

    void (Longest::*psolve)() =
        (naive ? &Longest::solve_naive : &Longest::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Longest longest(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (longest.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        longest.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
