// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;
typedef vector<ll_t> vll_t;
typedef unordered_map<ll_t, u_t> ll2u_t;

static unsigned dbg_flags;

class Longest
{
 public:
    Longest(istream& fi);
    Longest(const vll_t& _a) : N(_a.size()), a(_a) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    u_t get_solution() const { return solution; }
 private:
    u_t N;
    vll_t a;
    u_t solution;
};

Longest::Longest(istream& fi) : solution(0)
{
    fi >> N;
    copy_n(istream_iterator<ll_t>(fi), N, back_inserter(a));
}

void Longest::solve_naive()
{
    u_t b_best = 0, e_best = 0;
    for (u_t b = 0; b < N; ++b)
    {
        for (u_t e = b + 2; e <= N; ++e)
        {
            u_t sz = e - b;
            bool progression = sz <= 3;
            if (!progression)
            {
                for (u_t r = b; (r < e) && !progression; ++r)
                {
                    ll_t delta = (r > 1 ? a[1] - a[0] : a[sz - 1] - a[sz - 2]);
                    bool arith = true;
                    arith = arith && ((r == 0) || (r + 1 == sz) || 
                        (2*delta == a[r + 1] - a[r - 1]));
                    for (u_t k = b + 1; arith && (k < e); ++k)
                    {
                        arith = (k == r) || (k - 1 == r) || 
                            (delta == a[k] - a[k - 1]);
                    }
                    progression = arith;
                }
            }
            if (progression && (solution < sz))
            {
                solution = sz;
                b_best = b; e_best = e;
            }
        }
    }
    if (dbg_flags & 0x1) { cerr << "b="<<b_best << ", e="<<e_best << '\n'; }
}

void maxby(u_t& v, u_t x)
{
    if (v < x)
    {
        v = x;
    }
}

void Longest::solve()
{
    solution = (N <= 3 ? N : 3);
    if (N >= 4)
    {
        for (u_t b = 0; b + 3 < N; ++b)
        {
            for (u_t di = 1; di <= 3; ++di)
            {
                const int delta = a[b + di] - a[b + di - 1];
                if (a[b + 1] - a[b] == delta)
                {
                    u_t e = b + 1;
                    for (e = b + 1; (e < N) && (a[e] - a[e - 1]) == delta; ++e)
                    {
                        ;
                    }
                    maxby(solution, e - b);
                    if (e == N - 1)
                    {
                         maxby(solution, N - b);
                    }
                    else if ((e + 1 < N) && (a[e + 1] - a[e - 1] == 2*delta))
                    {
                        for (e += 2; (e < N) && (a[e] - a[e - 1]) == delta; ++e)
                        {
                            ;
                        }
                        maxby(solution, e - b);
                    }
                }
                else
                {
                    u_t e;
                    for (e = b + 3; (e < N) && (a[e] - a[e - 1]) == delta; ++e)
                    {
                        ;
                    }
                    maxby(solution, e - b);
                }
            }
        }
    }
}

void Longest::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int real_main(int argc, char ** argv)
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

    void (Longest::*psolve)() =
        (naive ? &Longest::solve_naive : &Longest::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Longest longest(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (longest.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        longest.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_case(vll_t& a)
{
    int rc = 0;
    u_t solution_naive = a.size() + 1, solution = a.size() + 2;
    {
        Longest p(a);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Longest p(a);
        p.solve();
        solution = p.get_solution();
    }
    if (solution != solution_naive)
    {
        rc = 1;
        ofstream f("longest-fail.in");
        f << "1\n" << a.size();
        const char *sep = "\n";
        for (ll_t x: a)
        {
            f << sep << x; sep = " ";
        }
        f << '\n';
        f.close();
    }
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t Nmin = strtoul(argv[ai++], 0, 0);
    u_t Nmax = strtoul(argv[ai++], 0, 0);
    ll_t delta_max = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t N = Nmin + (rand() % (Nmax + 1 - Nmin));
        vll_t a;
        a.push_back(0);
        while (a.size() < N)
        {
            ull_t delta = rand() % delta_max;
            a.push_back(a.back() + delta);
            rc = test_case(a);
        }
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
