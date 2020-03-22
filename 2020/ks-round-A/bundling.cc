// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<string> vs_t;

static unsigned dbg_flags;

class Bundling
{
 public:
    Bundling(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t n, k;
    vs_t ss;
    u_t solution;    
};

Bundling::Bundling(istream& fi) : solution(0)
{
    fi >> n >> k;
    ss.reserve(n);
    for (u_t si = 0; si < n; ++si)
    {
        string s;
        fi >> s;
        ss.push_back(s);
    }
}

void Bundling::solve_naive()
{
    sort(ss.begin(), ss.end());
    for (u_t si = 0; si < n; si += k)
    {
        // common prefix [ s[si] ... s[si + k] )
        u_t common = 0;
        bool match = true;
        for (u_t pfx = 0; match; ++pfx)
        {
            match = (ss[si].size() > pfx);
            for (u_t sj = si + 1; match && (sj < si + k); ++sj)
            {
                match = (ss[sj].size() > pfx) && (ss[sj][pfx] == ss[si][pfx]);
            }
            if (match)
            {
                common = pfx + 1;
            }
        }
        solution += common;
    }
}

void Bundling::solve()
{
    solve_naive();
}

void Bundling::print_solution(ostream &fo) const
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

    void (Bundling::*psolve)() =
        (naive ? &Bundling::solve_naive : &Bundling::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Bundling bundling(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (bundling.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        bundling.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
