// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>
#include <boost/numeric/ublas/matrix.hpp>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
// typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;
typedef boost::numeric::ublas::matrix<ul_t> mtx_ul_t;

static unsigned dbg_flags;

class Horse
{
 public:
    Horse(ul_t ve=0, ul_t vs=0) : e(ve), s(vs) {}
    ul_t e;
    ul_t s;
};
typedef vector<Horse> vh_t;

class Delivery
{
 public:
    Delivery(unsigned vs=0, unsigned vd=0): start(vs), dest(vd) {}
    unsigned start;
    unsigned dest;
};
typedef vector<Delivery> deliveries_t;

class Problem
{
 public:
    Problem(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    unsigned n, q;
    mtx_ul_t dist;
    vh_t horses;
    deliveries_t deliveries;        
};

Problem::Problem(istream& fi)
{
    fi >> n >> q;
    horses.reserve(n);
    for (unsigned i = 0; i < n; ++i)
    {
        ul_t e, s;
        fi >> e >> s;
        horses.push_back(Horse(e, s));
    }
    dist = mtx_ul_t(n, n);
    for (unsigned s = 0; s < n; ++s)
    {
        for (unsigned d = 0; d < n; ++d)
        {
            fi >> dist(s, d);
        }
    }
    deliveries.reserve(q);
    for (unsigned i = 0; i < q; ++i)
    {
        unsigned s, d;
        fi >> s >> d;
        deliveries.push_back(Delivery(s, d));
    }
}

void Problem::solve_naive()
{
}

void Problem::solve()
{
}

void Problem::print_solution(ostream &fo) const
{
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

    void (Problem::*psolve)() =
        (naive ? &Problem::solve_naive : &Problem::solve);

    ostream &fout = *pfo;
    bool tellg = getenv("TELLG");
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Problem problem(*pfi);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << ", " << fpos << ")\n";
            fpos_last = fpos;
        }

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
