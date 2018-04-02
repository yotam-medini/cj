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

// 2-headers - Just to see if the engine allows it:
// #include <boost/graph/adjacency_list.hpp>
#include <gmpxx.h>

using namespace std;

typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;

static unsigned dbg_flags = 0x1;

class NumGuess
{
 public:
    NumGuess(istream& fi);
    void solve_naive(istream&, ostream&);
    void solve(istream&, ostream&);
    // void print_solution(ostream&) const;
 private:
    ul_t a, b, n;
};

NumGuess::NumGuess(istream& fi)
{
    fi >> a >> b >> n;
    if (dbg_flags & 0x1) { cerr << "a="<<a << ", b="<<b << ", n="<<n << "\n"; }
}

void NumGuess::solve_naive(istream& fi, ostream &fo)
{
    bool guess = true;
    ul_t low = a + 1, high = b;
    static const string correct("CORRECT");
    static const string TOO_SMALL("TOO_SMALL");
    static const string TOO_BIG("TOO_BIG");
    static const string WRONG_ANSWER("WRONG_ANSWER");
    while (guess && (low <= high))
    {
        ul_t mid = (low + high)/2;
        if (dbg_flags & 0x1) { cerr << "Sending: " << mid << "\n"; }
        fo << mid << "\n"; fo.flush();
        string answer, dumnl;
        fi >> answer; getline(fi, dumnl);
        if (dbg_flags & 0x1) { cerr << "answer: " << answer << "\n"; }
        if (answer == correct)
        {
            guess = false;
        }
        else if (answer == TOO_SMALL)
        {
            low = mid + 1;
        }
        else if (answer == TOO_BIG)
        {
            high = mid - 1;
        }
        else if (answer == WRONG_ANSWER)
        {
            cerr << "Got WRONG_ANSWER\n";
            exit(1);
        }
        else
        {
            cerr << "Unexpected answer: " << answer << "\n";
            exit(1);
        }
    }
}

void NumGuess::solve(istream& fi, ostream &fo)
{
    solve_naive(fi, fo);
}

#if 0
void NumGuess::print_solution(ostream &fo) const
{
}
#endif

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
    if (dbg_flags & 0x1) { cerr << "n_cases="<<n_cases << "\n"; }

    void (NumGuess::*psolve)(istream&, ostream&) =
        (naive ? &NumGuess::solve_naive : &NumGuess::solve);
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        NumGuess numGuess(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (numGuess.*psolve)(*pfi, *pfo);
#if 0
        fout << "Case #"<< ci+1 << ":";
        numGuess.print_solution(fout);
        fout << "\n";
        fout.flush();
#endif
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
