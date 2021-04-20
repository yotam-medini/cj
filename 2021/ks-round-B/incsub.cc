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
typedef vector<u_t> vu_t;

static unsigned dbg_flags;

class IncSub
{
 public:
    IncSub(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t N;
    string S;
    vu_t solution;
};

IncSub::IncSub(istream& fi)
{
    fi >> N;
    fi >> S;
}

void IncSub::solve_naive()
{
    solution.reserve(N);
    for (u_t i = 0; i < N; ++i)
    {
        u_t n_inc = 0;
        for (u_t b = 0; b <= i; ++b)
        {
            bool inc = true;
            for (u_t j = b + 1; inc && (j <= i); ++j)
            {
                inc = S[j - 1] < S[j];
            }
            if (inc)
            {
                ++n_inc;
            }
        }
        solution.push_back(n_inc);
    }
}

void IncSub::solve()
{
    solution = vu_t(size_t(N), 0);
    for (int i = N - 1; i >= 0; )
    {
        int j = i - 1;
        for (j = i - 1; (j >= 0) && (S[j] < S[j + 1]); --j)
        {
            ;
        }
        u_t n_inc = 1;
        for (int k = j + 1; k <= i; ++k, ++n_inc)
        {
            solution[k] = n_inc;
        }
        i = j;
    }
}

void IncSub::print_solution(ostream &fo) const
{
    for (u_t n: solution)
    {
        fo << ' ' << n;
    }
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

    void (IncSub::*psolve)() =
        (naive ? &IncSub::solve_naive : &IncSub::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        IncSub incsub(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (incsub.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        incsub.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
