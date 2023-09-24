// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <fstream>
#include <iostream>
// #include <iterator>
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
// typedef vector<ul_t> vul_t;

static const ull_t BIG_MOD = 998244353;

static unsigned dbg_flags;

ull_t factorial(ull_t n) {
    ull_t r = 1;
    for (; n > 1; --n) {
        r = (r * n) % BIG_MOD;
    }
    return r;
}

class Calter
{
 public:
    Calter(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    string s;
    ull_t shortest, n_shortest;
};

Calter::Calter(istream& fi)
{
    fi >> s;
}

void Calter::solve_naive()
{
    shortest = 0;
    ull_t m = 0;
    n_shortest = 1;
    for (size_t i = 0; i < s.size(); ) {
        const size_t i0 = i;
        for (++i; (i < s.size()) && (s[i] == s[i0]); ++i) {
            ++shortest;
        }
        ull_t n_same = (i - i0);
        if (n_same > 1) {
            m += (n_same - 1);
            n_shortest = (n_shortest * n_same) % BIG_MOD;
        }
    }
    n_shortest = (n_shortest * factorial(m)) % BIG_MOD;
}

void Calter::solve()
{
    solve_naive();
}

void Calter::print_solution(ostream &fo) const
{
    fo << shortest << ' ' << n_shortest << '\n';
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

    void (Calter::*psolve)() =
        (naive ? &Calter::solve_naive : &Calter::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Calter calter(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (calter.*psolve)();
        calter.print_solution(fout);
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
