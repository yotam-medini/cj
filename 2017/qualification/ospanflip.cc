// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef vector<bool> vbool_t;

static unsigned dbg_flags;

class OverSizePanFlip
{
 public:
    OverSizePanFlip(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    string s;
    unsigned k;
    int solution;
};

OverSizePanFlip::OverSizePanFlip(istream& fi) : solution(0)
{
    fi >> s;
    fi >> k;
}

void OverSizePanFlip::solve_naive()
{
    const unsigned slen = s.length();
    vbool_t happy(vbool_t::size_type(slen), false);
    for (unsigned i = 0, e = slen; i < e; ++i)
    {
        happy[i] = s[i] == '+';
    }
    unsigned i = 0, iend = slen - k + 1;
    for (; i < iend; ++i)
    {
        if (!happy[i])
        {
            ++solution;
            for (unsigned fi = i, fend = i + k; fi < fend; ++fi)
            {
                happy[fi] = !happy[fi];
            }
        }
    }
    for (; (i < slen) && (solution >= 0); ++i)
    {
        if (!happy[i])
        {
            solution = -1;
        }
    }
}

void OverSizePanFlip::solve()
{
    solve_naive();
}

#if 0
void OverSizePanFlip::solve()
{
    char flipme = '-';
    const unsigned slen = s.length();
    unsigned i = 0, iend = slen - k + 1;
    unsigned reset_flipme = slen;
    for (; i < iend; ++i)
    {
        if (i == reset_flipme)
        {
            flipme = '-';
        }
        if (s[i] == flipme)
        {
            ++solution;
            flipme = (flipme == '+' ? '-' : '+');
            reset_flipme = i + k;
        }
    }
    for (; (i < slen) && (solution >= 0); ++i)
    {
        if (s[i] == flipme)
        {
            solution = -1;
        }
    }
}
#endif

void OverSizePanFlip::print_solution(ostream &fo) const
{
    if (solution >= 0)
    {
        fo << " " << solution;
    }
    else
    {
        fo << " IMPOSSIBLE";
    }
}

int main(int argc, char ** argv)
{
    const string dash("-");

    unsigned n_opts = 0;
    bool naive = false;

    if ((argc > 1) && (string(argv[1]) == "-naive"))
    {
        naive = true;
        n_opts = 1;
    }
    int ai_in = n_opts + 1;
    int ai_out = ai_in + 1;
    int ai_dbg = ai_out + 1;
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

    if (ai_dbg < argc) { dbg_flags = strtoul(argv[ai_dbg], 0, 0); }

    unsigned n_cases;
    *pfi >> n_cases;

    void (OverSizePanFlip::*psolve)() =
        (naive ? &OverSizePanFlip::solve_naive : &OverSizePanFlip::solve);

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        if (getenv("TELLG"))
        {
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg="<<pfi->tellg() << "\n";
        }
        OverSizePanFlip problem(*pfi);
        (problem.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
