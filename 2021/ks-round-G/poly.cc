// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <fstream>
#include <iostream>
// #include <iterator>
// #include <map>
// #include <set>
#include <array>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;

typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau_t;

static unsigned dbg_flags;

class Poly
{
 public:
    Poly(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void poly3();
    void poly4();
    void poly5();
    u_t N, A;
    vau_t solution;
};

Poly::Poly(istream& fi)
{
    fi >> N >> A;
}

void Poly::solve_naive()
{
    switch (N)
    {
     case 3:
        poly3();
        break;
     case 4:
        if (A >= 2)
        {
            poly4();
        }
        break;
     case 5:
        if (A >= 5)
        {
           poly5();
        }
        break;
    }
}

void Poly::poly3()
{
    solution.push_back(au2_t{0, 0});
    solution.push_back(au2_t{1, 0});
    solution.push_back(au2_t{0, A});
}

void Poly::poly4()
{
    solution.push_back(au2_t{0, 0});
    solution.push_back(au2_t{(A + 1)/2, 0});
    solution.push_back(au2_t{A/2, 1});
    solution.push_back(au2_t{0, 1});
}

void Poly::poly5()
{
    solution.push_back(au2_t{0, 0});
    if (A == 5)
    {
         solution.push_back(au2_t{1, 0});
         solution.push_back(au2_t{2, 1});
         solution.push_back(au2_t{1, 2});
         solution.push_back(au2_t{0, 1});
    }
    else
    {
        u_t b;
        solution.push_back(au2_t{2, 0});
        switch (A % 4)
        {
          case 0:
              b = (A - 8)/4 + 1;
              solution.push_back(au2_t{2, b});
              solution.push_back(au2_t{1, b + 2});
              solution.push_back(au2_t{0, b});
              break;
          case 1:
              b = (A - 9)/4 + 1;
              solution.push_back(au2_t{2, b + 1});
              solution.push_back(au2_t{1, b + 2});
              solution.push_back(au2_t{0, b});
              break;
          case 2:
              b = (A - 6)/4 + 1;
              solution.push_back(au2_t{2, b});
              solution.push_back(au2_t{1, b + 1});
              solution.push_back(au2_t{0, b});
              break;
          case 3:
              b = (A - 7)/4 + 1;
              solution.push_back(au2_t{2, b + 1});
              solution.push_back(au2_t{1, b + 1});
              solution.push_back(au2_t{0, b});
              break;
        }
    }
}

void Poly::solve()
{
    if (N <= 5)
    {
        solve_naive();
    }
}

void Poly::print_solution(ostream &fo) const
{
    if (solution.empty())
    {
        fo << " IMPOSSIBLE";
    }
    else
    {
        fo << " POSSIBLE";
        for (const au2_t& xy: solution)
        {
            fo << '\n' << xy[0] << ' ' << xy[1];
        }
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

    void (Poly::*psolve)() =
        (naive ? &Poly::solve_naive : &Poly::solve);
    if (solve_ver == 1) { psolve = &Poly::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Poly poly(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (poly.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        poly.print_solution(fout);
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
        ? test(argc - 1, argv + 1)
        : real_main(argc, argv));
    return rc;
}
