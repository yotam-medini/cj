// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include <array>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef array<u_t, 2> au2_t;

static unsigned dbg_flags;

class Banana
{
 public:
    Banana(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    int knip(u_t n, u_t target, const au2_t& last2);
    u_t N, K;
    vu_t B;
    int solution;
};

Banana::Banana(istream& fi) : solution(-1)
{
    fi >> N >> K;
    B.reserve(N);
    copy_n(istream_iterator<u_t>(fi), N, back_inserter(B));
}

void Banana::solve_naive()
{
    au2_t last2 = {N, N + 2};
    solution = knip(N, K, last2);
}

int Banana::knip(u_t n, u_t target, const au2_t& last2)
{
    int ret = -1;
    if (n > 0)
    {
        if (n == 1)
        {
            ret = (target == B[0] ? 1 : -1);
        }
        else
        {
            if (target == B[n - 1])
            {
                ret = 1;
            }
            else if (target > B[n - 1])
            {
                int sol_with = -1;
                if ((n != last2[0]) || (n + 1 != last2[1]))
                {
                    au2_t sub_last2 = {n - 1, last2[0]};
                    sol_with = knip(n - 1, target - B[n - 1], sub_last2);
                }
                int sol_without = knip(n - 1, target, last2);
                if (sol_with >= 0)
                {
                    sol_with += 1;
                    if (sol_without >= 0)
                    {
                        ret = min(sol_with, sol_without);
                    }
                    else
                    {
                        ret = sol_with;
                    }
                }
                else
                {
                    ret = sol_without;
                }
            }
        }
    }
    return ret;
}

void Banana::solve()
{
     solve_naive();
}

void Banana::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int test(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
    }
    return rc;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    u_t solve_ver = 0;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-solve1"))
        {
            solve_ver = 1;
        }
        else if (opt == string("-debug"))
        {
            dbg_flags = strtoul(argv[++ai], 0, 0);
        }
        else if (opt == string("-test"))
        {
            rc = test(argc - (ai + 1), argv + (ai + 1));
            return rc;
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

    void (Banana::*psolve)() =
        (naive ? &Banana::solve_naive : &Banana::solve);
    if (solve_ver == 1) { psolve = &Banana::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Banana banana(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (banana.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        banana.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
