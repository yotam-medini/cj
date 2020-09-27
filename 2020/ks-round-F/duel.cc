// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <fstream>
#include <iostream>
#include <array>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;

static unsigned dbg_flags;

class Duel
{
 public:
    Duel(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void solve_naive2();
    u_t S, RA, PA, RB, PB, C;
    int solution;
    vau2_t under;
};

Duel::Duel(istream& fi) : solution(0)
{
    fi >> S >> RA >> PA >> RB >> PB >> C;
    for (u_t i = 0; i != C; ++i)
    {
        au2_t uc;
        fi >> uc[0] >> uc[1];
        under.push_back(uc);
    }
}

void Duel::solve_naive()
{
    if (S == 2)
    {
        solve_naive2();
    }
}

void Duel::solve_naive2()
{
    if ((RA == 2) && (PA == 2))
    {  // Alma @ center
        solution = (C == 2 ? 0 : 1);
    }
    else if ((RB == 2) && (PB == 2))
    {  // Berthe @ center
        solution = (C == 2 ? 0 : -1);
    }
    else
    {
        if (C == 0)
        {
            solution = 2;
        }
        else if (C == 1)
        {
            au2_t uc = under[0];
            if ((uc[0] == 2) && (uc[1] == 2))
            {
                solution = 0;
            }
            else
            {
                solution = 1;
            }
        }
        else // C == 2
        {
            solution = 0;
        }
    }
}

void Duel::solve()
{
    solve_naive();
}

void Duel::print_solution(ostream &fo) const
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

    void (Duel::*psolve)() =
        (naive ? &Duel::solve_naive : &Duel::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Duel duel(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (duel.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        duel.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
