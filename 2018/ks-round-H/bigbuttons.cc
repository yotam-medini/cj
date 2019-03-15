// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2019/March/15

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<string> vs_t;

static unsigned dbg_flags;

class BigButtons
{
 public:
    BigButtons(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t n, p;
    vs_t prefixes;
    ull_t solution;
};

BigButtons::BigButtons(istream& fi) : solution(0)
{
    fi >> n >> p;
    prefixes.reserve(p);
    string s;
    for (u_t i = 0; i < p; ++i)
    {
        fi >> s;
        prefixes.push_back(s);
    }
}

void BigButtons::solve_naive()
{
    ull_t one = 1;
    solution = one << n;
    sort(prefixes.begin(), prefixes.end());
    u_t last_p = 0, last_sz = 0;
    for (u_t pi = 0; pi < p; ++pi)
    {
        const string prefix = prefixes[pi];
        u_t sz = prefix.size();
        bool sub = (last_p == 0);
        if (!sub)
        {
            const string &last = prefixes[last_p];
            sub = (last_sz > sz) ||
                !equal(last.begin(), last.end(), prefix.begin());
        }
        if (sub)
        {
            last_p = pi;
            last_sz = sz;
            ull_t solsub = one << (n - sz);
            solution -= solsub;
        }
    }
}

void BigButtons::solve()
{
    solve_naive();
}

void BigButtons::print_solution(ostream &fo) const
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

    void (BigButtons::*psolve)() =
        (naive ? &BigButtons::solve_naive : &BigButtons::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        BigButtons bigButtons(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (bigButtons.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        bigButtons.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
