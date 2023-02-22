// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
// #include <iterator>
// #include <map>
// #include <set>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;

static unsigned dbg_flags;

static u_t N_MAX = 100; // 

static vu_t primes;

static void set_primes()
{
    vector<bool> sieve(N_MAX + 1, true);
    primes.clear();
    for (u_t n = 2; n <= N_MAX; ++n)
    {
         if (sieve[n])
         {
             primes.push_back(n);
             for (u_t k = n*n; k <= N_MAX; k += n)
             {
                 sieve[k] = false;
             }
         }
    }
}

class NumPrimes
{
 public:
    NumPrimes(u_t _n=1, const vau2_t& pp = vau2_t()) :
        n(_n), primes_powers(pp) {}
    u_t n;
    vau2_t primes_powers;
};

class Matrygons
{
 public:
    Matrygons(istream& fi);
    Matrygons(u_t _N) : N(_N), solution(0) {};
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    u_t get_solution() const { return solution; }
 private:
    u_t best_under(u_t n, u_t target);
    u_t N;
    u_t solution;
};

Matrygons::Matrygons(istream& fi) : solution(0)
{
    fi >> N;
}

void Matrygons::solve_naive()
{
    N_MAX = 1000;
    for (u_t p = 3; p <= N; ++p)
    {
        u_t np = best_under(p, N);
        if (solution < np)
        {
            solution = np;
        }
    }
}

u_t Matrygons::best_under(u_t n, u_t target)
{
    u_t best = (target == n ? 1 : 0);
    if ((3 <= n) && (n < target))
    {
        target -= n;
        for (u_t d = 3; 2*d <= n; ++d)
        {
            if (n % d == 0)
            {
                u_t np = best_under(d, target);
                if ((np > 0) && (best < np + 1))
                {
                    best = np + 1;
                }
            }     
        }
    }
    return best;
}

void Matrygons::solve()
{
    N_MAX = 1000000;
    set_primes();
    solve_naive();
}

void Matrygons::print_solution(ostream &fo) const
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
            dbg_flags = strtoul(argv[++ai], nullptr, 0);
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

    void (Matrygons::*psolve)() =
        (naive ? &Matrygons::solve_naive : &Matrygons::solve);
    if (solve_ver == 1) { psolve = &Matrygons::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Matrygons matrygons(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (matrygons.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        matrygons.print_solution(fout);
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

static void save_case(const char* fn, u_t N)
{
    ofstream f(fn);
    f << "1\n" << N << '\n';
    f.close();
}

static int test_case(u_t N)
{
    int rc = rand_range(0, 1);
    u_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (dbg_flags & 0x100) { save_case("matrygons-curr.in", N); }
    if (small)
    {
        Matrygons p{N};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Matrygons p{N};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("matrygons-fail.in", N);
    }
    if (rc == 0) { cerr << "  ..." <<
        (small ? " (small) " : " (large) ") << " --> " <<
        solution << '\n'; }
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    if (string(argv[ai]) == string("-debug"))
    {
        dbg_flags = strtoul(argv[ai + 1], nullptr, 0);
        ai += 2;
    }
    const u_t Nmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Nmax = strtoul(argv[ai++], nullptr, 0);
    const u_t n_tests = Nmax + 1 - Nmin;
    cerr << "n_tests=" << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        '\n';
    for (u_t ti = 0, N = Nmin; (rc == 0) && (ti < n_tests); ++ti, ++N)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        rc = test_case(N);
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
