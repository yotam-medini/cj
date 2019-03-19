// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2019/March/20

#include <iostream>
#include <fstream>
#include <string>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;

static unsigned dbg_flags;

class Mural
{
 public:
    Mural(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t n;
    string beauty;
    u_t solution;
};

Mural::Mural(istream& fi)
{
    fi >> n;
    fi >> beauty;
}

void Mural::solve_naive()
{
    u_t n_half = (n + 1)/2;
    u_t curr = 0;
    u_t i;
    for (i = 0; i < n_half; ++i)
    {
        curr += (beauty.at(i) - '0');
    }
    solution = curr;
    u_t im = 0;
    for (; i < n; ++im, ++i)
    {
        curr += (beauty.at(i) - '0');
        curr -= (beauty.at(im) - '0');
        if (solution < curr)
        {
            solution = curr;           
        }
    }
}

void Mural::solve()
{
    solve_naive();
}

void Mural::print_solution(ostream &fo) const
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

    void (Mural::*psolve)() =
        (naive ? &Mural::solve_naive : &Mural::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Mural mural(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (mural.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        mural.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
