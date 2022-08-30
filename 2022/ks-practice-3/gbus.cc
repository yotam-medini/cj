// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <unordered_map>
#include <vector>
#include <iterator>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
// typedef array<u_t, 2> au2_t;
typedef array<u_t, 3> au3_t;
typedef vector<au3_t> vau3_t;
// typedef unordered_map<u_t, u_t> utou_t;

static unsigned dbg_flags;

class Gbus
{
 public:
    Gbus(istream& fi);
    Gbus(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    u_t N;
    vu_t A, B;
    u_t P;
    vu_t C;
    vu_t solution;
};

Gbus::Gbus(istream& fi)
{
    fi >> N;
    A.reserve(N);
    B.reserve(N);
    while (A.size() < N)
    {
        u_t a, b;
        fi >> a >> b;
        A.push_back(a);
        B.push_back(b);
    }
    fi >> P;
    C.reserve(P);
    solution.reserve(P);
    while (C.size() < P)
    {
        u_t c;
        fi >> c;
        C.push_back(c);
    }
    string ignore;
    getline(fi, ignore);
}

void Gbus::solve_naive()
{
    for (u_t c: C)
    {
        u_t nbus = 0;
        for (u_t i = 0; i < N; ++i)
        {
            u_t a = min(A[i], B[i]);
            u_t b = max(A[i], B[i]);
            if (a <= c && c <= b)
            {
                ++nbus;
            }
        }
        solution.push_back(nbus);
    }
}

void Gbus::solve()
{
    vau3_t abc;

    abc.reserve(2*N + P);
    for (u_t i = 0; i < N; ++i)
    {
        u_t a = min(A[i], B[i]);
        u_t b = max(A[i], B[i]);
        abc.push_back(au3_t{a, 0, 0});
        abc.push_back(au3_t{b, 2, 0});
    }
    for (u_t i = 0; i < P; ++i)
    {
        const u_t c = C[i];
        abc.push_back(au3_t{c, 1, i});
    }
    sort(abc.begin(), abc.end());
    solution = vu_t(P, u_t(-1));
    u_t nbus = 0;
    for (const au3_t& xt: abc)
    {
        if (xt[1] == 0) // A
        {
            ++nbus;
        }
        else if (xt[1] == 2) // B
        {
            --nbus;
        }
        else // xt[1] == 1 ===>  C
        {
            u_t i = xt[2];
            solution[i] = nbus;
        }
    }
}

void Gbus::print_solution(ostream &fo) const
{
    for (u_t x: solution)
    {
        fo << ' ' << x;
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

    void (Gbus::*psolve)() =
        (naive ? &Gbus::solve_naive : &Gbus::solve);
    if (solve_ver == 1) { psolve = &Gbus::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Gbus gbus(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (gbus.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        gbus.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static u_t rand_range(u_t nmin, u_t nmax)
{
    u_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

static int test_case(int argc, char ** argv)
{
    int rc = rand_range(0, 1);
    ull_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (small)
    {
        Gbus p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Gbus p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution="<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        ofstream f("gbus-fail.in");
        f << "1\n";
        f.close();
    }
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
        rc = test_case(argc, argv);
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test_random(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
