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
typedef vector<ull_t> vull_t;
typedef vector<vull_t> vvull_t;

static unsigned dbg_flags;

class Coins
{
 public:
    Coins(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t n;
    vvull_t coins;
    ull_t solution;
};

Coins::Coins(istream& fi) : solution(0)
{
    fi >> n;
    coins.reserve(n);
    for (u_t r = 0; r < n; ++r)
    {
        vull_t row;
        row.reserve(n);
        copy_n(istream_iterator<u_t>(fi), n, back_inserter(row));
        coins.push_back(row);
    }
}

void maxby(ull_t& v, ull_t x)
{
    if (v < x)
    {
        v = x;
    }
}

void Coins::solve_naive()
{
    for (u_t r = 0; r < n; ++r)
    {
        ull_t diag = 0;
        for (u_t c = 0; r + c < n; ++c)
        {
            diag += coins[r + c][c];
        }
        maxby(solution, diag);
    }

    for (u_t c = 0; c < n; ++c)
    {
        ull_t diag = 0;
        for (u_t r = 0; c + r < n; ++r)
        {
            diag += coins[r][c + r];
        }
        maxby(solution, diag);
    }
}

void Coins::solve()
{
    solve_naive();
}

void Coins::print_solution(ostream &fo) const
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

    void (Coins::*psolve)() =
        (naive ? &Coins::solve_naive : &Coins::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Coins coins(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (coins.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        coins.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
