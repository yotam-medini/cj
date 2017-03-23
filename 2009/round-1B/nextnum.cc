// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<char> vc_t;

static unsigned dbg_flags;

class NextNum
{
 public:
    NextNum(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    string num;
    string solution;
};

NextNum::NextNum(istream& fi)
{
    fi >> num;
}

void NextNum::solve_naive()
{
}

void NextNum::solve()
{
    unsigned slen = num.length();
    vc_t vc(vc_t::size_type(slen), ' ');
    for (unsigned si = 0; si < slen; ++si)
    {
        vc[si] = num[si];
    }
    // find decrease
    int i = slen - 2;
    while ((i >= 0) && (vc[i] >= vc[i + 1]))
    {
        --i;
    }
    if (i >= 0) // vc[i] < vc[i + 1];
    {
        // find who to swap with vc[i]
        unsigned jswap = i + 1;
        for (unsigned j = jswap + 1; j < slen; ++j)
        {
            if ((vc[i] < vc[j]) && (vc[j] < vc[jswap]))
            {
                jswap = j;
            }
        }
        swap(vc[i], vc[jswap]);
        sort(vc.begin() + i + 1, vc.end());
    }
    else
    {
        // find minimal non-zero
        char min_nonzero = ' ';
        unsigned i_min_nonzero = slen;
        for (unsigned i = 0; i < slen; ++i)
        {
            char c = vc[i];
            if ((c != '0') && ((min_nonzero == ' ') || (c < min_nonzero)))
            {
                min_nonzero = c;
                i_min_nonzero = i;
            }
        }
        solution.append(1, min_nonzero);
        vc[i_min_nonzero] = '0';
        sort(vc.begin(), vc.end());
    }
    for (unsigned i = 0; i < slen; ++i)
    {
        solution.append(1, vc[i]);
    }
}

void NextNum::print_solution(ostream &fo) const
{
   fo << " " << solution;
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

    void (NextNum::*psolve)() =
        (naive ? &NextNum::solve_naive : &NextNum::solve);

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        if (getenv("TELLG"))
        {
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg="<<pfi->tellg() << "\n";
        }
        NextNum problem(*pfi);
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
