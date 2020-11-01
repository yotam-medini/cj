// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;

static unsigned dbg_flags;

class HighBuild
{
 public:
    HighBuild(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t n, a, b, c;
    vu_t solution;
};

HighBuild::HighBuild(istream& fi)
{
    fi >> n >> a >> b >> c;
}

void HighBuild::solve_naive()
{
    for (u_t i = 0; i < n; ++i)
    {
        solution.push_back(1);
    }
    bool possible = false, done = false;
    while (!(possible or done))
    {
        u_t na = 0, nb = 0, nc = 0;
        for (u_t i = 0; i < n; ++i)
        {
            bool asee = true, bsee = true;
            for (u_t j = 0; (j < i) && asee; ++j)
            {
                asee = solution[j] <= solution[i];
            }
            for (u_t j = i + 1; (j < n) && bsee; ++j)
            {
                bsee = solution[j] <= solution[i];
            }
            na += (asee ? 1 : 0);
            nb += (bsee ? 1 : 0);
            nc += (asee && bsee ? 1 : 0);
        }
        possible = (a == na) && (b == nb) && (c == nc);
        if (!possible)
        {
            u_t k;
            for (k = 0; (k < n) && (solution[k] == n); ++k) 
            {}
            done = (k == n);
            if (!done)
            {
                for (u_t low = 0; low < k; ++low)
                {
                    solution[low] = 1;
                }
                ++solution[k];
            }
        }
    }
    if (!possible)
    {
        solution.clear();
    }    
}

void HighBuild::solve()
{
    solve_naive();
#if 0
    bool possible = (c <= n);
    possible = possible && (a <= c) && (b <= c);
    possible = possible && (c <= a + b);
    if (possible)
    {
    }
#endif
}

void HighBuild::print_solution(ostream &fo) const
{
    if (solution.empty())
    {
        fo << " IMPOSSIBLE";
    }
    else
    {
        for (u_t x: solution)
        {
            fo << ' ' << x;
        }
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

    void (HighBuild::*psolve)() =
        (naive ? &HighBuild::solve_naive : &HighBuild::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        HighBuild highbuild(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (highbuild.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        highbuild.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
