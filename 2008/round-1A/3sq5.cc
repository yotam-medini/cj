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
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    unsigned four_power_mod1000(unsigned n) const;
    unsigned x_power_mod1000(unsigned n) const;
    unsigned p;
    unsigned solution;
};

ThreeSqrt5::ThreeSqrt5(istream& fi)
{
    fi >> p;
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

unsigned ThreeSqrt5::four_power_mod1000(unsigned n) const
{
    static const unsigned m1000[] = {
        16,  64, 256,  24,  96, 384, 536, 144, 576, 304,
       216, 864, 456, 824, 296, 184, 736, 944, 776, 104,
       416, 664, 656, 624, 496, 984, 936, 744, 976, 904,
       616, 464, 856, 424, 696, 784, 136, 544, 176, 704,
       816, 264,  56, 224, 896, 584, 336, 344, 376, 504
    };
    unsigned ret = (n == 0 ? 1 : 4);
    if (n >= 2) {
        static const unsigned m_1000_size = sizeof(m1000) / sizeof(m1000[0]);
        ret = m1000[(n - 2) % m_1000_size];
    }
    return ret;
}

unsigned ThreeSqrt5::x_power_mod1000(unsigned n) const
{
    unsigned ret = 0;
    if (n < 3)
    {
        static const unsigned low_rets[] = {2, 6, 28};
        ret = low_rets[n];
    }
    else if (n % 3 == 0)
    {
        unsigned xnd3 = x_power_mod1000(n/3);
        unsigned xnd3p3 = xnd3 * xnd3 * xnd3;
        unsigned p4 = four_power_mod1000(n/3);
        ret = (xnd3p3 + (4000 - 3*((p4*xnd3) % 1000))) % 1000;
    }
    else if (n % 2 == 0)
    {
        unsigned xnd2 = x_power_mod1000(n/2);
        unsigned xnd2p2 = xnd2 * xnd2;
        unsigned p4 = four_power_mod1000(n/2);
        ret = (xnd2p2 + (3000 - 2*p4)) % 1000;
    }
    else 
    {
        unsigned xn_m1 = x_power_mod1000(n - 1);
        unsigned x1 = x_power_mod1000(1);
        unsigned xn_x1 = xn_m1 * x1;
        unsigned xn_m2 = x_power_mod1000(n - 2);
        ret = (xn_x1 + (5000 - 4*xn_m2)) % 1000;
    }
    return ret;
}

void ThreeSqrt5::solve()
{
    // solve_naive();
    // Using published analysis tips
    // a = 3 + sqrt(5),   b = 3 - sqrt(5),
    // x_n = a^n + b^n
    // ==>   x_n  integer, b^n < 1,   ab = 4.
    // Using the following recursive identitoes
    // x_n * x_1 = x_{n+1} + 4*x_{n-1}
    // x_n ** 2 = x_{2n} + 2*(4**n)
    // x_n ** 3 = x_{3n} + 3*(4**n)*x_n
    // ==>
    // x_{3n}  = x_n ** 3 - 3*(4**n)*x_n
    // x_{2n}  = x_n ** 2 - 2*(4**n)
    // x_{n+1} =  x_n * x_1 - 4*x_{n-1}
    unsigned xn = x_power_mod1000(p);
    solution = (xn + (1000 - 1)) % 1000;
}

void ThreeSqrt5::print_solution(ostream &fo) const
{
    char b[4];
    sprintf(b, "%03d", solution);
    fo << " " << b;
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
