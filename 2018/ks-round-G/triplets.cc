// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2019/March/23

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class Triplets
{
 public:
    Triplets(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t n;
    vull_t a;
    ull_t solution;
};

Triplets::Triplets(istream& fi) : solution(0)
{
    fi >> n;
    a.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        ull_t x;
        fi >> x;
        a.push_back(x);
    }
}

void Triplets::solve_naive()
{
    sort(a.begin(), a.end());
    for (u_t i = 0; i < n; ++i)
    {
        const ull_t &x = a[i];
        for (u_t j = i + 1; j < n; ++j)
        {
            const ull_t &y = a[j];
            for (u_t k = j + 1; k < n; ++k)
            {
                const ull_t &z = a[k];
                if ((x == y*z) || (y == x*z) || (z == (x*y)))
                {
                    ++solution;
                }
            }
        }
    }
}

void Triplets::solve()
{
    sort(a.begin(), a.end());
    // cerr << "MaxLast2=" << a[n-2]*a[n-1] << "\n";
    auto er0 = equal_range(a.begin(), a.end(), 0);
    ull_t nz = er0.second - er0.first;
    ull_t nonz = n - nz;
    ull_t triplets_z3 = (nz >= 3 ? (nz*(nz - 1)*(nz - 2))/6 : 0);
    ull_t triplets_z2 = (nz >= 2 ? ((nz*(nz - 1))/2)*nonz : 0);
    solution = triplets_z3 + triplets_z2;
    for (u_t i = nz; i < n; ++i)
    {
        ull_t ai = a[i];
        bool overflow = false;
        for (u_t j = i + 1, j1 = j + 1; (j1 < n) && !overflow; j = j1++)
        {
            ull_t aj = a[j];
            ull_t aa = ai * aj;
            auto er = equal_range(a.begin() + j1, a.end(), aa);
            if (er.first != er.second)
            {
                ull_t add = (er.second - er.first);
                solution += add;
            }
            else
            {
                overflow = (er.first == a.end());
            }
        }
    }
}

void Triplets::print_solution(ostream &fo) const
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

    void (Triplets::*psolve)() =
        (naive ? &Triplets::solve_naive : &Triplets::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Triplets triplets(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (triplets.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        triplets.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
