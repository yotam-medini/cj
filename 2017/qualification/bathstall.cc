// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <set>
// #include <map>
// #include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;
typedef set<ull_t> set_ull_t;


static unsigned dbg_flags;

class BathStall
{
 public:
    BathStall(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void be_slr(ull_t b, ull_t e, ull_t &s, ull_t &l, ull_t &r);
    ull_t n;
    ull_t k;

    ull_t solution_max, solution_min;
};

BathStall::BathStall(istream& fi) : solution_max(0), solution_min(0)
{
    fi >> n >> k;
}

void BathStall::be_slr(ull_t b, ull_t e, ull_t &s, ull_t &l, ull_t &r)
{
    ull_t space = e - b;
    if (space == 0)
    {
        s = e; // undef
        l = 0;
        r = 0;
    }
    else
    {
        ull_t half = (space - 1) / 2;
        s = b + half;
        l = s - b;
        r = e - s - 1;
    }
}

void BathStall::solve_naive()
{
    set_ull_t taken;
    ull_t l = 0, r = 0; // undef?

    for (unsigned p = 0; p < k; ++p)
    {
        ull_t s = n; // undef
        if (taken.empty())
        {
            be_slr(ull_t(0), ull_t(n), s, l, r);
        }
        else
        {
            ull_t scandid, s_l, s_r;
            set_ull_t::iterator tb = taken.begin();
            set_ull_t::iterator te = taken.end();
            ull_t tfirst = *tb;
            ull_t tlast = *taken.rbegin();
            
            if (tfirst > 0)
            {
                be_slr(ull_t(0), tfirst, s, l, r);
            }

            set_ull_t::iterator ti(tb);
            set_ull_t::iterator tnext(ti);
            for (++tnext; tnext != te; ti = tnext, ++tnext)
            {
                be_slr(*ti + 1, *tnext, scandid, s_l, s_r);
                if ((s == n) || (l < s_l) || ((l == s_l) && (r < s_r)))
                {
                    s = scandid;
                    l = s_l;
                    r = s_r;
                }
            }

            if (tlast < n)
            {
                be_slr(tlast + 1, n, scandid, s_l, s_r);
                if ((s == n) || (l < s_l) || ((l == s_l) && (r < s_r)))
                {
                    s = scandid;
                    l = s_l;
                    r = s_r;
                }
            }
        }
        
        taken.insert(taken.end(), s);
    }
    solution_max = max(l, r);
    solution_min = min(l, r);
}

void BathStall::solve()
{
}

void BathStall::print_solution(ostream &fo) const
{
    fo << ' ' << solution_max << ' ' << solution_min;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    unsigned n_opts = 0;
    bool naive = false;

    if ((argc > 1) && (string(argv[1]) == "-naive"))
    {
        naive = true;
        n_opts = 1;
    }
    int ai_in = n_opts + 1;
    int ai_out = ai_in + 1;
    int ai_dbg = ai_out + 1;
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

    if (ai_dbg < argc) { dbg_flags = strtoul(argv[ai_dbg], 0, 0); }

    unsigned n_cases;
    *pfi >> n_cases;

    void (BathStall::*psolve)() =
        (naive ? &BathStall::solve_naive : &BathStall::solve);

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        if (getenv("TELLG"))
        {
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg="<<pfi->tellg() << "\n";
        }
        BathStall problem(*pfi);
        (problem.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
