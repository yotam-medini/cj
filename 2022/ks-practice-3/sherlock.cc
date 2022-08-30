// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
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

static unsigned dbg_flags;

static const ull_t BIG_MOD = 1000000000 + 7;

class Sherlock
{
 public:
    Sherlock(istream& fi);
    Sherlock(ull_t _A, ull_t _B, ull_t _N, ull_t _K) :
        A(_A), B(_B), N(_N), K(_K) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
 private:
    ull_t A, B, N, K;
    ull_t solution;
};

Sherlock::Sherlock(istream& fi) : solution(0)
{
    fi >> A >> B >> N >> K;
}

ull_t power_mod(ull_t b, u_t p, ull_t K)
{
    ull_t r = 1;
    ull_t b2p= b;
    while (p)
    {
        if (p % 2)
        {
            r = (r*b2p) % K;
        }
        b2p = (b2p * b2p) % K;
        p /= 2;
    }
    return r;
}

void Sherlock::solve_naive()
{
#if 0
 for (ull_t b = 1; b <= 6; ++b) {
   for (u_t p = 0; p <= 12; ++p) {
       ull_t bp = 1;
       for (u_t k = 0; k < p; ++k) {
           bp *= b;
       }
       ull_t pm = power_mod(b, p);
       if (pm != bp) {
           cerr << "BUG in power_mod\n";
           exit(1);
       }
    }
 }
#endif
    for (ull_t i = 1; i <= N; ++i)
    {
        ull_t iA = power_mod(i, A, K);
        for (ull_t j = 1; j <= N; ++j)
        {
            if (i != j)
            {
                ull_t jB = power_mod(j, B, K);
                ull_t iA_jB = iA + jB;
                if (iA_jB % K == 0)
                {
                    solution = (solution + 1) % BIG_MOD;
                    if (dbg_flags & 0x1) {cerr << "i="<<i << ", j="<<j <<'\n';}
                }
            }
        }
    }
}

void Sherlock::solve()
{
    ull_t NmodK = (N + 1) % K;
    ull_t n_ij_lt_k = 0;
    ull_t n_i_zeros = 0;
    ull_t n_j_zeros = 0;
    ull_t n_ij_zeros = 0;
    ull_t n_ij_eq = 0;
    ull_t n_i_lt_mnk = 0;
    ull_t n_j_lt_mnk = 0;
    ull_t n_ij_lt_mnk = 0;
    ull_t n_ij_eq_lt_mnk = 0;
    for (ull_t i = 0; i < K; ++i)
    {
        ull_t iA = power_mod(i, A, K);
        for (ull_t j = 0; j < K; ++j)
        {
            ull_t jB = power_mod(j, B, K);
            ull_t iA_jB = iA + jB;
            if (iA_jB % K == 0)
            {
                if (dbg_flags & 0x2) { cerr << "i="<<i << ", j="<<j <<'\n'; }
                n_ij_lt_k = (n_ij_lt_k + 1) % BIG_MOD;
                n_i_zeros += int(i == 0);
                n_j_zeros += int(j == 0);
                n_ij_zeros += int((i == 0) && (j == 0));
                n_ij_eq += int(i == j);
                n_i_lt_mnk += int(i < NmodK);
                n_j_lt_mnk += int(j < NmodK);
                n_ij_lt_mnk += int((i < NmodK) && (j < NmodK));
                n_ij_eq_lt_mnk += int((i == j) && (i < NmodK));
            }
        }
    }
    if (dbg_flags & 0x1) { cerr << "N+1="<<N+1 << ", K="<<K <<
        ", #(ij_lt_k)="<<n_ij_lt_k << 
        ", #(i_zeros)="<<n_i_zeros << 
        ", #(j_zeros)="<<n_j_zeros << 
        ", #(ij_zeros)="<<n_ij_zeros << 
        ", #(ij_eq)="<<n_ij_eq << 
        ", #(i_lt_mnk)="<<n_i_lt_mnk << 
        ", #(j_lt_mnk)="<<n_j_lt_mnk << 
        ", #(ij_lt_mnk)="<<n_ij_lt_mnk << 
        ", #(ij_lt_mnk)="<<n_ij_lt_mnk << 
        ", #(ij_eq_lt_mnk)="<<n_ij_eq_lt_mnk << 
        '\n';
    }
    ull_t d = (N + 1) / K;
    solution = d*d*n_ij_lt_k;
    if (n_ij_zeros)
    {
        // The following two subtractions will remove (0,0) - 3 times.
        solution += 2; // to compensate
    }
    solution -= d*n_ij_eq;
    solution -= d*(n_i_zeros + n_j_zeros);
    if (NmodK > 0)
    {
        solution += d*(n_i_lt_mnk + n_j_lt_mnk) + n_ij_lt_mnk;
        solution -= d*(n_i_zeros + n_j_zeros);
        if (n_ij_zeros)
        {
            solution -= 1;
        }
    }
}

void Sherlock::print_solution(ostream &fo) const
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

    void (Sherlock::*psolve)() =
        (naive ? &Sherlock::solve_naive : &Sherlock::solve);
    if (solve_ver == 1) { psolve = &Sherlock::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Sherlock sherlock(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (sherlock.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        sherlock.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

#if 0
static u_t rand_range(u_t nmin, u_t nmax)
{
    u_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}
#endif

static int test_case(ull_t A, ull_t B, ull_t N, ull_t K)
{
    int rc = 0;
    ull_t solution(-1), solution_naive(-1);
    bool small = (A + B + N + K) < 0x20;
    if (small)
    {
        Sherlock p(A, B, N, K);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Sherlock p(A, B, N, K);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution="<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        ofstream f("sherlock-fail.in");
        f << "1\n" << A << ' ' << B << ' ' << N << ' ' << K << '\n';
        f.close();
    }
    cerr << "  " << A << ' ' << B << ' ' << N << ' ' << K << " ==> " <<
        solution << '\n';
    return rc;
}

#if 0
static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        rc = test_case(1, 1, 1, 1);
    }
    return rc;
}
#endif

static int test_small()
{
    int rc = 0;
    ull_t Amax = 5;
    ull_t Bmax = 5;
    ull_t Nmax = 5;
    ull_t Kmax = 5;
    u_t n_tests = (Amax + 1)*(Bmax + 1)*Nmax*Kmax;
    u_t ti = 0;
    for (ull_t A = 0; (rc == 0) && (A <= Amax); ++A)
    {
        for (ull_t B = 0; (rc == 0) && (B <= Bmax); ++B)
        {
            for (ull_t N = 1; (rc == 0) && (N <= Nmax); ++N)
            {
                for (ull_t K = 1; (rc == 0) && (K <= Kmax); ++K, ++ti)
                {
                    cerr << "Tested: " << ti << '/' << n_tests << '\n';
                    rc = test_case(A, B, N, K);
                }
            }
        }
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test_small()
        : real_main(argc, argv));
    return rc;
}
