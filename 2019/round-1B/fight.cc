// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <algorithm>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<int> vi_t;
typedef array<int, 2> i2_t;


static unsigned dbg_flags;

class Fight
{
 public:
    Fight(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool is_fair(const i2_t &v) const;
    ul_t n;
    int k;
    vi_t cd[2];
    ull_t solution;
};

Fight::Fight(istream& fi) : solution(0)
{
    fi >> n >> k;
    for (u_t cdi = 0; cdi < 2; ++cdi)
    {
        cd[cdi].reserve(n);
        for (ul_t i = 0; i < n; ++i)
        {
            int skill;
            fi >> skill;
            cd[cdi].push_back(skill);
        }
    }
}

void Fight::solve_naive()
{
    unsigned nfair = 0;
    vi_t::const_iterator cbegin = cd[0].begin();
    vi_t::const_iterator dbegin = cd[1].begin();
    for (u_t l = 0; l < n; ++l)
    {
        for (u_t r = l + 1; r <= n; ++r)
        {
            int cmax = *max_element(cbegin + l, cbegin + r);
            int dmax = *max_element(dbegin + l, dbegin + r);
            int delta = (cmax < dmax ? dmax - cmax : cmax - dmax);
            if (delta <= k)
            {
                ++nfair;
            }
        }
    }
    solution = nfair;
}

bool Fight::is_fair(const i2_t &v) const
{
    int delta = (v[0] < v[1] ? v[1] - v[0] : v[0] - v[1]);
    return (delta <= k);
}

static void maximize(int &m, int x)
{
    if (m < x)
    {
        m = x;
    }
}

void Fight::solve()
{
    unsigned nfair = 0;
    for (u_t b = 0; b < n; ++b)
    {
        i2_t cdmax({cd[0][b], cd[1][b]});
        for (u_t e = b; e < n; ++e)
        {
            maximize(cdmax[0], cd[0][e]);
            maximize(cdmax[1], cd[1][e]);
            if (is_fair(cdmax))
            {
                ++nfair;            
            }
        }
    }
    solution = nfair;
}

void Fight::print_solution(ostream &fo) const
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

    void (Fight::*psolve)() =
        (naive ? &Fight::solve_naive : &Fight::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Fight fight(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (fight.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        fight.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
