// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <numeric>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;

static unsigned dbg_flags;

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

class Sanagran
{
 public:
    Sanagran(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    string S;
    string solution;
};

Sanagran::Sanagran(istream& fi)
{
    fi >> S;
}

void Sanagran::solve_naive()
{
    const size_t n = S.size();
    vu_t p;
    bool found = false;
    permutation_first(p, n);
    for (permutation_first(p, n); (!found) && permutation_next(p); )
    {
        string a;
        bool good = true;
        for (size_t i = 0; good && (i < n); ++i)
        {
            const char c = S[p[i]];
            a.push_back(c);
            good = (c != S[i]);
        }
        if (good)
        {
            found = true;
            solution = a;
        }
    }
}

void Sanagran::solve()
{
     if (S.size() <= 8)
     {
         solve_naive();
     }
}

void Sanagran::print_solution(ostream &fo) const
{
    static const string impossible("IMPOSSIBLE");
    fo << ' ' << (solution.empty() ? impossible : solution);
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    u_t solve_ver = 0;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-solve1"))
        {
            solve_ver = 1;
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

    void (Sanagran::*psolve)() =
        (naive ? &Sanagran::solve_naive : &Sanagran::solve);
    if (solve_ver == 1) { psolve = &Sanagran::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Sanagran sanagran(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (sanagran.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        sanagran.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
