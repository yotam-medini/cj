// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <utility>
#include <string>
#include <cstring>

#include <cstdlib>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;

static unsigned dbg_flags;

class TidyNum
{
 public:
    TidyNum(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    // bool improve();
    ull_t N;
    string sN;
    const char *csN;
    unsigned Nlen;

    string max_fwd_digit;
    // string pfx;

    string solution;
};

TidyNum::TidyNum(istream& fi) : solution("")
{
    fi >> sN;
    csN = sN.c_str();
    N = strtoull(csN, 0, 10);
    Nlen = sN.length();
}

void TidyNum::solve_naive()
{
    for (ull_t n = N; (n > 0) && solution == ""; --n)
    {
        char buf[0x20];
        sprintf(buf, "%lld", n);
        unsigned slen = strlen(buf);
        bool nondecrease = true;
        for (unsigned si = 1; nondecrease && (si < slen); ++si)
        {
            nondecrease = buf[si - 1] <= buf[si];
        }
        if (nondecrease)
        {
            solution = string(buf);
        }
    }
}

void TidyNum::solve()
{
    unsigned nondecr = 0;
    for (unsigned next = 1; (next < Nlen) && sN[nondecr] <= sN[next]; 
        nondecr = next++);
    if (nondecr == Nlen - 1)
    {
        solution = sN;
    }
    else
    {
        // go back to decrease
        int back = nondecr;
        if (back > 0)
        {
            while ((back >= 0) && sN[back] == sN[nondecr])
            {
                --back;
            }
            ++back;
        }

        if (back == 0)
        {
            solution = string(Nlen - 1, '9');
            if (sN[0] != '1')
            {
                char dh = sN[0] - 1;
                solution = string(1, dh) + solution;
            }
        }
        else
        {
            char d = sN[back] - 1;
            const string head(sN.substr(0, back));
            const string mid(1, d);
            const string tail(Nlen - back - 1, '9');
// cerr << "head="<<head << ", mid="<<mid << ", tail="<<tail << "\n";
            solution = head + mid + tail;
        }
    }
}


void TidyNum::print_solution(ostream &fo) const
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

    void (TidyNum::*psolve)() =
        (naive ? &TidyNum::solve_naive : &TidyNum::solve);

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        if (getenv("TELLG"))
        {
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg="<<pfi->tellg() << "\n";
        }
        TidyNum problem(*pfi);
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
