// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
// #include <vector>
#include <utility>

#include <cstdlib>
#include <gmpxx.h>

using namespace std;

typedef mpz_class mpzc_t;
typedef set<char> setc_t;
typedef map<char, unsigned> c2u_t;

static unsigned dbg_flags;

class AllBase
{
 public:
    AllBase(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    string message;
    mpzc_t solution;
};

AllBase::AllBase(istream& fi) : solution(0)
{
    fi >> message;
}

void AllBase::solve_naive()
{
}

void AllBase::solve()
{
    c2u_t cvals;
    const unsigned mlen = message.length();
    cvals.insert(cvals.end(), c2u_t::value_type(message[0], 1));
    unsigned nextval = 0;
    for (unsigned mi = 1; mi < mlen; ++mi)
    {
        char c = message[mi];
        if (cvals.find(c) == cvals.end())
        {
            cvals.insert(cvals.end(), c2u_t::value_type(c, nextval));
            nextval = (nextval == 0 ? 2 : nextval + 1);
        }
    }
    unsigned base = max(unsigned(cvals.size()), 2u);
    for (unsigned mi = 0; mi < mlen; ++mi)
    {
        char c = message[mi];
        unsigned cval = cvals[c];
        solution = base * solution + cval;
    }
}

void AllBase::print_solution(ostream &fo) const
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

    void (AllBase::*psolve)() =
        (naive ? &AllBase::solve_naive : &AllBase::solve);

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        if (getenv("TELLG"))
        {
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg="<<pfi->tellg() << "\n";
        }
        AllBase problem(*pfi);
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
