// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;
typedef pair<int, int> ii_t;
typedef vector<ii_t> vii_t;
typedef set<ii_t> setii_t;

static unsigned dbg_flags;

class Pylon
{
 public:
    Pylon(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool is_good_jump(const ii_t &cell0, const ii_t &cell1);
    int r, c;
    vii_t solution;
    vii_t candidate;
    setii_t pending;
    bool advance();
    
};

Pylon::Pylon(istream& fi)
{
    fi >> r >> c;
}

void Pylon::solve_naive()
{
    for (int x = 1; x <= c; ++x)
    {
        for (int y = 1; y <= r; ++y)
        {
            pending.insert(pending.end(), (ii_t(x, y)));
        }
    }
    advance();    
}

bool Pylon::advance()
{
    bool found = pending.empty();
    if (found)
    {
        solution = candidate;
    }

    const ii_t curr = (candidate.empty()
        ? ii_t(0, r + c + 1) : candidate.back());
    vii_t lpending(pending.begin(), pending.end());
    for (vii_t::const_iterator i = lpending.begin(), e = lpending.end(); 
        solution.empty() && (i != e); ++i)
    {
        const ii_t cell = *i;
        bool good_jump = is_good_jump(curr, cell);
        if (good_jump)
        {
            candidate.push_back(cell);
            pending.erase(setii_t::value_type(cell));
            advance();
            pending.insert(pending.end(), cell);
            candidate.pop_back();
        }
    }

    return found;
}

bool Pylon::is_good_jump(const ii_t &cell0, const ii_t &cell1)
{
    int x0 = cell0.first;
    int y0 = cell0.second;
    int x1 = cell1.first;
    int y1 = cell1.second;
    bool ok = (x0 != x1) && (y0 != y1);
    ok = ok && ((x0 + y0) != (x1 + y1));
    ok = ok && ((x0 - y0) != (x1 - y1));
    return ok;
}

void Pylon::solve()
{
    solve_naive();
}

void Pylon::print_solution(ostream &fo) const
{
    if (solution.empty())
    {
        fo << " IMPOSSIBLE";
    }
    else
    {
        fo << " POSSIBLE";
        for (const ii_t cell: solution)
        {
            fo << "\n" << cell.first << " " << cell.second;
        }
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

    void (Pylon::*psolve)() =
        (naive ? &Pylon::solve_naive : &Pylon::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Pylon pylon(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (pylon.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        pylon.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
