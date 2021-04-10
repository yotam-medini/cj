// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;

static unsigned dbg_flags;

class Prime
{
 public:
    Prime(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t M;
    vu_t P, N;
    ul_t solution;
};

Prime::Prime(istream& fi) : solution(0)
{
    fi >> M;
    P.reserve(M);
    N.reserve(M);
    for (u_t i = 0; i < M; ++i)
    {
        u_t p, n;
        fi >> p >> n;
        P.push_back(p);
        N.push_back(n);
    }
}

void Prime::solve_naive()
{
    vu_t nums;
    for (u_t i = 0; i < M; ++i)
    {
        for (u_t k = 0; k < N[i]; ++k)
        {
            nums.push_back(P[i]);
        }
    }
    const u_t nn = nums.size();
    const u_t mask_max = 1u << nn;
    for (u_t mask = 0; mask < mask_max; ++mask)
    {
        ull_t sum = 0, prod = 1;
        for (u_t i = 0; i < nn; ++i)
        {
            if (((1u << i) & mask) == 0)
            {
                sum += nums[i];
            }
            else
            {
                prod *= nums[i];
            }
        }
        if ((sum == prod) && (solution < sum))
        {
            solution = sum;
        }
    }
}

void Prime::solve()
{
    solve_naive();
}

void Prime::print_solution(ostream &fo) const
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

    void (Prime::*psolve)() =
        (naive ? &Prime::solve_naive : &Prime::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Prime prime(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (prime.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        prime.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
