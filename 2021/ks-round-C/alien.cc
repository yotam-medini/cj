// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

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

static unsigned dbg_flags;

class Alien
{
 public:
    Alien(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ull_t G;
    ull_t solution;
};

Alien::Alien(istream& fi) : solution(0)
{
    fi >> G;
}

void Alien::solve_naive()
{
    for (ull_t k = 1; k <= G; ++k)
    {
        ull_t total = 0;
        for (ull_t kk = k; total < G; ++kk)
        {
            total += kk;
        }
        if (total == G)
        {
            ++solution;
        }
    }
}

void Alien::solve()
{
     solution = 1;
     ull_t total = 0;
     for (ull_t k = 1; total < G; ++k)
     {
         if ((dbg_flags & 0x1) && ((k & (k - 1)) == 0)) {
             cerr << "k=" << k << '\n';
         }
         total += k;
         ull_t delta = (total < G ? G - total : 0);
         if ((delta > 0) && ((delta) % (k + 1) == 0))
         {
             ++solution;
         }
     }
}

void Alien::print_solution(ostream &fo) const
{
     fo << ' ' << solution;
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

    void (Alien::*psolve)() =
        (naive ? &Alien::solve_naive : &Alien::solve);
    if (solve_ver == 1) { psolve = &Alien::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Alien alien(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (alien.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        alien.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
