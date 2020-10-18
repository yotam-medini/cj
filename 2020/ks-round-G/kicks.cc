// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>
#include <cstring>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;

static unsigned dbg_flags;

class Kicks
{
 public:
    Kicks(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    string text;
    ull_t solution;
};

Kicks::Kicks(istream& fi) : solution(0)
{
    fi >> text;  
}

void Kicks::solve_naive()
{
    const char* s = text.c_str();
    const char* p = s;
    p = strstr(p, "KICK");
    if (p)
    {
        ull_t n_kicks = 0;
        const char* pkick = "dum";
        const char* pstart = "dum";
        while (pstart)
        {
            pkick = strstr(p, "KICK");
            pstart = strstr(p + 1, "START");
            if (pkick && pstart)
            {
                if (pkick < pstart)
                {
                    ++n_kicks;
                    p = pkick + 1;
                }
                else
                {
                    solution += n_kicks;
                    p = pstart + 1;
                }
            }
            else if (pstart)
            {
                solution += n_kicks;
                p = pstart + 1;
            }
        }
    }
}

void Kicks::solve()
{
    solve_naive();
}

void Kicks::print_solution(ostream &fo) const
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

    void (Kicks::*psolve)() =
        (naive ? &Kicks::solve_naive : &Kicks::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Kicks kicks(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (kicks.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        kicks.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
