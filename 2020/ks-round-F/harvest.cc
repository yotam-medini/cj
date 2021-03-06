// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <array>
// #include <map>
// #include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;

static unsigned dbg_flags;

class Harvest
{
 public:
    Harvest(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t n, k;
    vau2_t intervals;
    u_t solution;
};

Harvest::Harvest(istream& fi) : solution(0)
{
    // copy_n(istream_iterator<u_t>(fi), N, back_inserter(a));
    fi >> n >> k;
    intervals.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        au2_t interval;
        fi >> interval[0] >> interval[1];
        intervals.push_back(interval);
    }
}

void Harvest::solve_naive()
{
    sort(intervals.begin(), intervals.end());
    u_t i = 0, t = 0;
    while (i < n)
    {
        const au2_t& interval = intervals[i];
        u_t dt = interval[1] - interval[0];
        if (t <= interval[0])
        {
            t = interval[0];
            u_t nr = (dt + k - 1) / k;
            solution += nr;
            t += nr * k;
        }
        else if (t < interval[1])
        {
            dt = interval[1] - t;
            u_t nr = (dt + k - 1) / k;
            solution += nr;
            t += nr * k;
        }
        ++i;
    }
}

void Harvest::solve()
{
    solve_naive();
}

void Harvest::print_solution(ostream &fo) const
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

    void (Harvest::*psolve)() =
        (naive ? &Harvest::solve_naive : &Harvest::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Harvest harvest(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (harvest.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        harvest.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
