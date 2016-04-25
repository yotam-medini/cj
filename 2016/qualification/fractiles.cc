// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>
#include <gmpxx.h>

using namespace std;

typedef unsigned long ul_t;
typedef vector<ul_t> vul_t;

typedef mpz_class mpzc_t;
typedef vector<mpzc_t> vmpzc_t;

static unsigned dbg_flags;

class Fractiles
{
 public:
    Fractiles(istream& fi);
    void solve();
    void print_solution(ostream&);
 private:
    void solve_large();
    void solve_naive();
    void solve_seqk();
    unsigned K, C, S;
    vmpzc_t solution;
};

Fractiles::Fractiles(istream& fi)
{
    fi >> K >> C >> S;
}

void Fractiles::solve()
{
    if (dbg_flags & 0x100)
    {
        solve_naive();
    }
    else
    {
        solve_large();
    }
}

void Fractiles::solve_large()
{
    unsigned s_needed = (K + C - 1)/C;
    if (s_needed <= S)
    {
        unsigned n = 0;
        for (unsigned s = 0; s < s_needed; ++s)
        {
            mpzc_t g = 0;
            for (unsigned c = 0; (c < C); ++c, ++n)
            {
                g = (K * g) + min(n, K - 1);
            }
            solution.push_back(g + 1);
        }
    }
}

void Fractiles::solve_naive()
{
    if (S == K)
    {
        solve_seqk();
    }
}

void Fractiles::solve_seqk()
{
    const mpzc_t mpK = K;
    mpzc_t k_power_cm1 = 1;
    for (unsigned p = 0; p < (C - 1); ++p)
    {
        k_power_cm1 *= mpK;
    }
    for (unsigned i = 0; i < S; ++i)
    {
        solution.push_back(mpzc_t(i)*k_power_cm1 + 1);
    }
}

void Fractiles::print_solution(ostream &fo)
{
    if (solution.empty())
    {
        fo << " " << "IMPOSSIBLE";
    }
    else
    {
         for (auto i = solution.begin(), e = solution.end(); i != e; ++i)
         {
             fo << " " << *i;
         }
    }
}

int main(int argc, char ** argv)
{
    const string dash("-");

    istream *pfi = (argc < 2 || (string(argv[1]) == dash))
         ? &cin
         : new ifstream(argv[1]);
    ostream *pfo = (argc < 3 || (string(argv[2]) == dash))
         ? &cout
         : new ofstream(argv[2]);

    if ((!pfi) || (!pfo) || pfi->fail() || pfo->fail())
    {
        cerr << "Open file error\n";
        exit(1);
    }

    if (argc > 3) { dbg_flags = strtoul(argv[3], 0, 0); }
    // bool large = (argc > 4) && !strcmp(argv[4], "-large");

    unsigned n_cases;
    *pfi >> n_cases;

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Fractiles fractiles(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        fractiles.solve();
        fout << "Case #"<< ci+1 << ":";
        fractiles.print_solution(fout);
        fout << "\n";
        fout.flush();
        
    }
    
    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

