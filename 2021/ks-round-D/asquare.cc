// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef array<u_t, 2> au2_t;

static unsigned dbg_flags;

class ASquare
{
 public:
    ASquare(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ll_t G[3][3];
    u_t solution;
};

ASquare::ASquare(istream& fi) : solution(0)
{
    for (u_t i: {0, 1, 2})
    {
        for (u_t j: {0, 1, 2})
        {
            if ((i != 1) || (j != 1))
            {
                fi >> G[i][j];
            }
        }
    }
}

void ASquare::solve_naive()
{
    u_t n_fixed = 0;
    for (u_t i: {0, 2})
    {
        if (2*G[i][1] == G[i][0] + G[i][2])
        {
            ++n_fixed;
        }
        if (2*G[1][i] == G[0][i] + G[2][i])
        {
            ++n_fixed;
        }
    }
    u_t best_mid_count = 0;
    static const au2_t As[4] =
        {au2_t{0, 0}, au2_t{0, 1}, au2_t{0, 2}, au2_t{1, 0}};
    for (const au2_t& A: As)
    {
        const au2_t B{2 - A[0], 2 - A[1]};
        const ll_t v = (G[A[0]][A[1]] + G[B[0]][B[1]]) / 2;
        u_t mid_count = 0;
        for (const au2_t& X: As)
        {
            const au2_t Y{2 - X[0], 2 - X[1]};
            if (2*v == (G[X[0]][X[1]] + G[Y[0]][Y[1]]))
            {
                ++mid_count;
            }
        }
        if (best_mid_count < mid_count)
        {
            best_mid_count = mid_count;
        }
    }
    solution = n_fixed + best_mid_count;
}

void ASquare::solve()
{
    solve_naive();
}

void ASquare::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int real_main(int argc, char ** argv)
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

    void (ASquare::*psolve)() =
        (naive ? &ASquare::solve_naive : &ASquare::solve);
    if (solve_ver == 1) { psolve = &ASquare::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        ASquare asquare(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (asquare.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        asquare.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_specific(int argc, char ** argv)
{
    int rc = 0;
    return rc;
}

static int test_random(int argc, char ** argv)
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

static int test(int argc, char ** argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("specific"))
        ? test_specific(argc - 1, argv + 1)
        : test_random(argc, argv));
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
