// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <array>
#include <numeric>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<u_t, 4> au4_t;

static unsigned dbg_flags;

class Threed
{
 public:
    Threed(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    au4_t printers[3];
    bool possible;
    au4_t solution;
};

Threed::Threed(istream& fi) : possible(false)
{
    for (u_t pi = 0; pi < 3; ++pi)
    {
         for (u_t c = 0; c < 4; ++c)
         {
             fi >> printers[pi][c];
         }
    }
}

void Threed::solve_naive()
{
    static const u_t GOAL = 1000000;
    u_t total = 0;
    solution = au4_t{0, 0, 0, 0};
    u_t c = 0;
    for ( ; (c < 4) && (total < GOAL); ++ c)
    {
         u_t cmin = GOAL - total;
         for (u_t pi = 0; pi < 3; ++pi)
         {
             cmin = min(cmin, printers[pi][c]);
         }
         solution[c] = cmin;
         total += cmin;
    }
    possible = (total == GOAL);
}

void Threed::solve()
{
    solve_naive();
}

void Threed::print_solution(ostream &fo) const
{
    if (possible)
    {
        for (u_t c: solution)
        {
            fo << ' ' << c;
        }
    }
    else
    {
        fo << " IMPOSSIBLE";
    }
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

    void (Threed::*psolve)() =
        (naive ? &Threed::solve_naive : &Threed::solve);
    if (solve_ver == 1) { psolve = &Threed::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Threed threed(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (threed.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        threed.print_solution(fout);
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
