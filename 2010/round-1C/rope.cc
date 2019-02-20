// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;
typedef pair<ul_t, ul_t> ulul_t;
typedef vector<ulul_t> vulul_t;

static unsigned dbg_flags;

class Rope
{
 public:
    Rope(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool intersect(const ulul_t &w0, const ulul_t &w1) const;
    u_t n;
    vulul_t wires;
    ul_t solution;
};

Rope::Rope(istream& fi) : solution(0)
{
    fi >> n;
    wires.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        ul_t a, b;
        fi >> a >> b;
        wires.push_back(ulul_t(a, b));
    }
}

void Rope::solve_naive()
{
    for (unsigned i = 0; i < n; ++i)
    {
        for (unsigned j = i + 1; j < n; ++j)
        {
            if (intersect(wires[i], wires[j]))
            {
                ++solution;
            }
        }
    }
}

bool Rope::intersect(const ulul_t &w0, const ulul_t &w1) const
{
    bool x = (w0.first < w1.first) != (w0.second < w1.second);
    return x;
}

void Rope::solve()
{
    solve_naive();
}

void Rope::print_solution(ostream &fo) const
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

    void (Rope::*psolve)() =
        (naive ? &Rope::solve_naive : &Rope::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Rope rope(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (rope.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        rope.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
