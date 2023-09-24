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

static unsigned dbg_flags;

static const ull_t BIG_MOD = 998244353;

class Dsumxor
{
 public:
    Dsumxor(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    size_t n;
    vull_t a;
    vull_t xors;
    ull_t solution;
};

Dsumxor::Dsumxor(istream& fi) : solution(0)
{
    fi >> n;
    copy_n(istream_iterator<ull_t>(fi), n, back_inserter(a));
}

void Dsumxor::solve_naive()
{
    xors.reserve(n + 1);
    xors.push_back(0);
    for (size_t i = 0; i < n; ++i) {
        xors.push_back(xors.back() ^ a[i]);
    }
    for (size_t l = 0; l < n; ++l) {
        for (size_t r = l + 1; r <= n; ++r) {
            ull_t xor_lr = xors[r] ^ xors[l];
            solution = (solution + (r - l)*xor_lr) % BIG_MOD;
        }
    }
}

void Dsumxor::solve()
{
    solve_naive();
}

void Dsumxor::print_solution(ostream &fo) const
{
    fo << solution << '\n';
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
    unsigned n_cases = 1;
    // *pfi >> n_cases;
    // getline(*pfi, ignore);

    void (Dsumxor::*psolve)() =
        (naive ? &Dsumxor::solve_naive : &Dsumxor::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Dsumxor dsumxor(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (dsumxor.*psolve)();
        dsumxor.print_solution(fout);
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
