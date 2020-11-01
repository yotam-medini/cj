// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<int> vi_t;

static unsigned dbg_flags;

class LongArith
{
 public:
    LongArith(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t n;
    vi_t a;
    u_t solution;
};

LongArith::LongArith(istream& fi) : solution(0)
{
    fi >> n;
    copy_n(istream_iterator<int>(fi), n, back_inserter(a));
}

void LongArith::solve_naive()
{
    for (u_t i = 0; i + 1 < n; ++i)
    {
        u_t l = 2;
        int delta = a[i + 1] - a[i];
        for (u_t j = i + 2; (j < n) && (a[j] - a[j - 1] == delta); ++j)
	{
	    ++l;
	}
	if (solution < l)
	{
            solution = l;
        }
    }
}

void LongArith::solve()
{
    for (u_t i = 0; i + 1 < n; )
    {
        u_t j, l = 2;
        int delta = a[i + 1] - a[i];
        for (j = i + 2; (j < n) && (a[j] - a[j - 1] == delta); ++j)
	{
	    ++l;
	}
        i = j - 1;
	if (solution < l)
	{
            solution = l;
        }
    }

}

void LongArith::print_solution(ostream &fo) const
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

    void (LongArith::*psolve)() =
        (naive ? &LongArith::solve_naive : &LongArith::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        LongArith longarith(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (longarith.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        longarith.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
