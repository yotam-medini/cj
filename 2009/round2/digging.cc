// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
// #include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class Digging
{
 public:
    Digging(istream& fi);
    ~Digging() { delete [] _cells; }
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    const bool &cell(unsigned x, unsigned y) const { return _cells[r*x + y]; }
    bool &cell(unsigned x, unsigned y) { return _cells[r*x + y]; }
    unsigned r, c, f;
    bool *_cells;
    unsigned solution;
};

Digging::Digging(istream& fi) : r(0), c(0), f(0), _cells(0), solution(0)
{
    fi >> r >> c >> f;
    _cells = new bool[r * c];
    for (unsigned y = 0; y < r; ++y)
    {
        string s;
        fi >> s;
        for (unsigned x = 0; x < c; ++x)
        {
            cell(x, y) = (s[x] == '*');
        }
    }
}

void Digging::solve_naive()
{
}

void Digging::solve()
{
}

void Digging::print_solution(ostream &fo) const
{
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (argv[ai][0] == '-') && argv[ai][1] != '\0'; ++ai)
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

    unsigned n_cases;
    *pfi >> n_cases;

    void (Digging::*psolve)() =
        (naive ? &Digging::solve_naive : &Digging::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Digging digging(*pfi);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << ", " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (digging.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        digging.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
