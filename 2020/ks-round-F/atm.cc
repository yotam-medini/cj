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
typedef vector<ul_t> vul_t;
typedef pair<ul_t, ul_t> ulul_t;
typedef vector<ulul_t> vulul_t;

static unsigned dbg_flags;

class Atm
{
 public:
    Atm(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t n;
    u_t x;
    vul_t a;
    vul_t solution;
};

Atm::Atm(istream& fi)
{
    fi >> n >> x;
    copy_n(istream_iterator<ul_t>(fi), n, back_inserter(a));
}

void Atm::solve_naive()
{
    vul_t need(a);
    ul_t out = 0;
    ul_t next = 0;
    while (out < n)
    {
        while (need[next] == 0)
        {
            next = (next + 1) % n;
        }
        if (need[next] <= x)
        {
            // solution[next] = ++out;
            solution.push_back(next + 1);
            ++out;
            need[next] = 0;
        }
        else
        {
            need[next] -= x;
        }
        next = (next + 1) % n;
    }
}

void Atm::solve()
{
    vulul_t turns;
    turns.reserve(n);
    for (ul_t i = 0; i != n; ++i)
    {
        ul_t t = (a[i] + x - 1) / x;
        ulul_t ti = ulul_t(t, i);
        turns.push_back(ti);
    }
    sort(turns.begin(), turns.end());
    for (const ulul_t& ti: turns)
    {
        solution.push_back(ti.second + 1);
    }
}

void Atm::print_solution(ostream &fo) const
{
    for (ul_t sol: solution)
    {
        fo << ' ' << sol;
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

    void (Atm::*psolve)() =
        (naive ? &Atm::solve_naive : &Atm::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Atm atm(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (atm.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        atm.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
