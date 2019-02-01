// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>

#include <cstdlib>
#include <cmath>
#include <gmpxx.h>

using namespace std;

typedef mpz_class mpzc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;

static unsigned dbg_flags;

class ThreeSqrt5
{
 public:
    ThreeSqrt5(istream& fi);
    void solve_naive1();
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    mpzc_t power(const mpzc_t &x, unsigned p);
    unsigned p;
    unsigned solution;
};

ThreeSqrt5::ThreeSqrt5(istream& fi)
{
    fi >> p;
}

void ThreeSqrt5::solve_naive1()
{
    const long double r = 3 + sqrt(5);
    const long double dp = 1. / (long double)(p);
    mpzc_t low = power(5, p), high = power(6, p);
    
    while (high - low > 1)
    {
        mpzc_t mid = (low + high) / 2;
        long double dmid = mid.get_d();
        long double mid_root = powl(dmid, dp);
        if (mid_root < r)
        {
            low = mid;
        }
        else
        {
            high = mid;
        }
    }
    mpzc_t low3 = low % 1000;
    solution = low3.get_ui();
}

void ThreeSqrt5::solve_naive()
{
    mpzc_t a3 = 3, a5 = 1; //   a3 + a5 * sqrt(5)
    for (unsigned k = 1; k < p; ++k)
    {
        //  (a3 + a5 * sqrt(5)) * (3 + sqrt(5)
        mpzc_t a3next = 3*a3 + 5*a5;
        mpzc_t a5next = a3 + 3 * a5;
        a3 = a3next;
        a5 = a5next;
    }
    // a3 is simple, compute int(a5*sqrt(5))
    mpzc_t x2 = 5 * (a5 * a5);
    mpzc_t x = sqrt(x2);
    mpzc_t result = a3 + x;
    mpzc_t low3 = result % 1000;
    solution = low3.get_ui();
}


void ThreeSqrt5::solve()
{
    solve_naive();
}

void ThreeSqrt5::print_solution(ostream &fo) const
{
    char b[4];
    sprintf(b, "%03d", solution);
    fo << " " << b;
}

mpzc_t ThreeSqrt5::power(const mpzc_t &x, unsigned pp)
{
    mpzc_t ret = 1;
    if (pp > 0)
    {
        if (pp == 1)
        {
           ret = x;
        }
        else
        {
            unsigned ph = pp / 2;
            mpzc_t xph = power(x, ph);
            mpzc_t xph2 = xph * xph;
            ret = (pp % 2 == 0 ? xph2 : xph2 * x);
        }
    }
    return ret;
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

    void (ThreeSqrt5::*psolve)() =
        (naive ? &ThreeSqrt5::solve_naive : &ThreeSqrt5::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        ThreeSqrt5 problem(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
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
