// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <utility>
#include <vector>
#include <iterator>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long int ull_t;
typedef long long int ll_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

// See: https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm
ll_t extended_gcd(ll_t& bezout_x, ll_t& bezout_y, const ll_t a, const ll_t b)
{
    
    ll_t s = 0, old_s = 1;
    ll_t r = b, old_r = a;
         
    while (r != 0)
    {
        lldiv_t div_result = lldiv(old_r, r);
        ll_t quotient = div_result.quot;
        ll_t smod = old_s - quotient * s;
        old_r = r;  r = div_result.rem;
        old_s = s;  s = smod;
    }

    bezout_x = old_s;
    bezout_y = (b ? (old_r - old_s * a) / b : 0);
    return ((a == 0) && (b == 0) ? 0 : old_r);
}

ull_t invmod(ull_t a, ull_t m)
{
    ll_t x, y;
    extended_gcd(x, y, a, m);
    ull_t inv = (x + m) % m;
    return inv;
}
    
class Intruder
{
 public:
    Intruder(istream& fi);
    Intruder(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    u_t W;
    ull_t N, D;
    vull_t X;
    bool possible;
    ull_t solution;
};

Intruder::Intruder(istream& fi) : possible(true), solution(0)
{
    fi >> W >> N >> D;
    copy_n(istream_iterator<ull_t>(fi), W, back_inserter(X));
}

void Intruder::solve_naive()
{
    for (u_t i = 0, j = W - 1; possible && (i < j); ++i, --j)
    {
        const ull_t xi = X[i], xj = X[j];
        ull_t steps = 0;
        for (ull_t xi_fwd = xi, xj_fwd = xj;
             (xi_fwd != xj) && (xj_fwd != xi) && (steps < N);
             ++steps,
             xi_fwd = (xi_fwd + D) % N,
             xj_fwd = (xj_fwd + D) % N)
        {
            ;
        }
        possible = (steps < N);
        if (possible)
        {
            solution += steps;
        }
    }
}

void Intruder::solve()
{
    const ull_t g = gcd(N, D);
    const ull_t N_dg = N / g;
    const ull_t D_dg = D / g;
    const ull_t inv_D_dg = invmod(D_dg, N_dg);
    for (u_t i = 0, j = W - 1; possible && (i < j); ++i, --j)
    {
        const ull_t xi = X[i], xj = X[j];
        const ull_t delta = (xi < xj ? xj - xi : xi - xj);
        possible = (delta % g == 0);
        if (possible)
        {
            const ull_t delta_dg = delta / g;
            // steps * D_dg =~= delta_dg
            const ull_t steps_fwd = (delta_dg * inv_D_dg) % N_dg;
            const ull_t steps_bwd = N_dg - steps_fwd;
            const ull_t steps = min(steps_fwd, steps_bwd);
            solution += steps;
        }
    }

}

void Intruder::print_solution(ostream &fo) const
{
    if (possible)
    {
        fo << ' ' << solution;
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

    void (Intruder::*psolve)() =
        (naive ? &Intruder::solve_naive : &Intruder::solve);
    if (solve_ver == 1) { psolve = &Intruder::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Intruder intruder(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (intruder.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        intruder.print_solution(fout);
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

static void save_case(const char* fn)
{
    ofstream f(fn);
    f << "1\n";
    f.close();
}

static int test_case(int argc, char ** argv)
{
    int rc = rand_range(0, 1);
    ull_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (dbg_flags & 0x100) { save_case("intruder-curr.in"); }
    if (small)
    {
        Intruder p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Intruder p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("intruder-fail.in");
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
    const u_t n_tests = strtoul(argv[ai++], nullptr, 0);
    const u_t Nmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Nmax = strtoul(argv[ai++], nullptr, 0);
    cerr << "n_tests=" << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        '\n';
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
