// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <algorithm>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;

static unsigned dbg_flags;

class RankPure
{
 public:
    RankPure(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void bits2seq(vu_t &s, unsigned bits);
    bool is_pure(const vu_t &s);
    unsigned n;
    unsigned solution;
};

RankPure::RankPure(istream& fi) : solution(0)
{
    fi >> n;
}

void RankPure::bits2seq(vu_t &s, unsigned bits)
{
    s.clear();
    s.push_back(0); // dummy - so we can count from 1
    for (unsigned bit = 1; bit <= n; ++bit)
    {
        if ((bits & (1u << bit)) != 0)
        {
            s.push_back(bit);
        }
    }
}

bool RankPure::is_pure(const vu_t &s)
{
    bool pure = true;
    unsigned v = n;
    unsigned si = s.size();
    auto sb = s.begin();
    auto se = s.end();
    while (pure && (si > 1))
    {
        auto w = find(sb, se, v);
        if (w == se)
        {
            pure = false;
        }
        else
        {
            unsigned si_next = w - sb;
            pure = (si_next < si);
            si = si_next;
            v = si;
        }
    }
    return pure;
}

void RankPure::solve_naive()
{
    unsigned n_pure = 0;
    unsigned bit_n = (1u << n);
    vu_t s;
    for (unsigned bits = 0; bits < bit_n; bits += 2)
    {
        bits2seq(s, bits | bit_n);
        if (is_pure(s))
        {
            ++n_pure;
        }
    }
    solution = n_pure % 100003;
}

void RankPure::solve()
{
}

void RankPure::print_solution(ostream &fo) const
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

    void (RankPure::*psolve)() =
        (naive ? &RankPure::solve_naive : &RankPure::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        RankPure rankPure(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (rankPure.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        rankPure.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
