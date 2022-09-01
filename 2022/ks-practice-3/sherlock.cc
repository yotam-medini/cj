// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

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
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

static const ull_t BIG_MOD = 1000000000 + 7;

ull_t sub_mod_big(ull_t a, ull_t b)
{
    ull_t sub = (a + (BIG_MOD - (b % BIG_MOD))) % BIG_MOD;
    return sub;
}

class Sherlock
{
 public:
    Sherlock(istream& fi);
    Sherlock(ull_t _A, ull_t _B, ull_t _N, ull_t _K) :
        A(_A), B(_B), N(_N), K(_K), solution(0) {}
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

ull_t power_mod(ull_t b, ull_t p, ull_t K)
{
    b %= K;
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
    return r % K;
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
    ull_t n_i_zeros_j_lt_nmk = 0;
    ull_t n_j_zeros_i_lt_nmk = 0;
    ull_t n_ij_eq = 0;
    ull_t n_i_lt_nmk = 0;
    ull_t n_j_lt_nmk = 0;
    ull_t n_ij_lt_nmk = 0;
    ull_t n_ij_eq_lt_nmk = 0;
#if 1
    vull_t n_iAmodK(K, 0), n_jBmodK(K, 0);
    vull_t n_iAmodK_i_lt_nmk(K, 0), n_jBmodK_j_lt_nmk(K, 0);
    vull_t iAs; iAs.reserve(K);
    vull_t jBs; jBs.reserve(K);
    for (ull_t x = 0; x < K; ++x)
    {
        const ull_t iA = power_mod(x, A, K);
        iAs.push_back(iA);
        const ull_t jB = power_mod(x, B, K);
        jBs.push_back(jB);
        ++n_iAmodK[iA];
        ++n_jBmodK[jB];
        if ((iA + jB) % K == 0)
        {
            ++n_ij_eq;
            if (x < NmodK)
            {
                ++n_ij_eq_lt_nmk;
            }
        }
        if (x < NmodK)
        {
            ++n_iAmodK_i_lt_nmk[iA];
            ++n_jBmodK_j_lt_nmk[jB];
        }
    }
    n_i_zeros = n_jBmodK[(K - iAs[0]) % K];
    n_j_zeros = n_iAmodK[(K - jBs[0]) % K];
    n_ij_zeros = ((iAs[0] + jBs[0]) % K == 0) ? 1 : 0;
    n_i_zeros_j_lt_nmk += n_jBmodK_j_lt_nmk[(K - iAs[0]) % K];
    n_j_zeros_i_lt_nmk += n_iAmodK_i_lt_nmk[(K - jBs[0]) % K];
    for (ull_t x = 0; x < K; ++x)
    {
        const ull_t iA = iAs[x];
        const ull_t jB = jBs[x];
        n_ij_lt_k += n_jBmodK[(K - iA) % K];
        if (x < NmodK)
        {
            n_ij_lt_nmk += n_jBmodK_j_lt_nmk[(K - iA) % K];
            n_i_lt_nmk += n_jBmodK[(K - iA) % K];
            n_j_lt_nmk += n_iAmodK[(K - jB) % K];
        }
    }
#else
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
                n_i_lt_nmk += int(i < NmodK);
                n_j_lt_nmk += int(j < NmodK);
                n_ij_lt_nmk += int((i < NmodK) && (j < NmodK));
                n_i_zeros_j_lt_nmk += int((i == 0) && (j < NmodK));
                n_j_zeros_i_lt_nmk += int((j == 0) && (i < NmodK));
                n_ij_eq_lt_nmk += int((i == j) && (i < NmodK));
            }
        }
    }
#endif
    if (dbg_flags & 0x1) { cerr << "N+1="<<N+1 << ", K="<<K <<
        ", #(ij_lt_k)="<<n_ij_lt_k << 
        ", #(i_zeros)="<<n_i_zeros << 
        ", #(j_zeros)="<<n_j_zeros << 
        ", #(ij_zeros)="<<n_ij_zeros << 
        ", #(ij_eq)="<<n_ij_eq << 
        ", #(i_lt_nmk)="<<n_i_lt_nmk << 
        ", #(j_lt_nmk)="<<n_j_lt_nmk << 
        ", #(ij_lt_nmk)="<<n_ij_lt_nmk << 
        ", #(ij_lt_nmk)="<<n_ij_lt_nmk << 
        ", #(i_zeros_j_lt_nmk)="<<n_i_zeros_j_lt_nmk << 
        ", #(j_zeros_i_lt_nmk)="<<n_j_zeros_i_lt_nmk << 
        ", #(ij_eq_lt_nmk)="<<n_ij_eq_lt_nmk << 
        '\n';
    }
    const ull_t d = (N + 1) / K;
    const ull_t d_mod_big = d % BIG_MOD;
    const ull_t d2 = (d_mod_big * d_mod_big) % BIG_MOD;
    solution += d2*n_ij_lt_k;
    if (NmodK > 0)
    {   // sides
        solution += d_mod_big*(n_i_lt_nmk + n_j_lt_nmk);
    }
    solution = solution % BIG_MOD;
    // distinguish case of having only mini-square, d=0.
    if (d == 0)
    {
        solution = (solution + n_ij_lt_nmk) % BIG_MOD;  // mini-square
        ull_t sub_L = ((n_i_zeros_j_lt_nmk + n_j_zeros_i_lt_nmk) - n_ij_zeros);
        solution = sub_mod_big(solution, sub_L);
        ull_t sub_diag = (n_ij_eq_lt_nmk - n_ij_zeros);
        solution = sub_mod_big(solution, sub_diag);
    }
    else
    {
        if (NmodK > 0)
        {
            solution += n_ij_lt_nmk;
        }
        if (n_ij_zeros)
        {
            // The following subtractions will remove (0,0) - 2 times.
            solution += 1; // to compensate
        }
        solution = solution % BIG_MOD;
        // deletions:
        ull_t diag_whole_squares = d * n_ij_eq;
        solution = sub_mod_big(solution, diag_whole_squares);
        ull_t zero_main = n_ij_zeros;
        ull_t zero_sides_squares = d*(n_i_zeros + n_j_zeros) - zero_main;
        solution = sub_mod_big(solution, zero_sides_squares);
        if (NmodK > 0)
        {
            // z-stripes
            ull_t s_stripes = (n_i_zeros_j_lt_nmk + n_j_zeros_i_lt_nmk);
            solution = sub_mod_big(solution, s_stripes);
            solution = sub_mod_big(solution, n_ij_eq_lt_nmk);
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

static ull_t rand_range(ull_t nmin, ull_t nmax)
{
    u_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

static int test_case(ull_t A, ull_t B, ull_t N, ull_t K)
{
    int rc = 0;
    ull_t solution(-1), solution_naive(-1);
    bool small = (A + B + N + K) < 0x80;
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
        cerr << "Failed: solution = "<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        ofstream f("sherlock-fail.in");
        f << "1\n" << A << ' ' << B << ' ' << N << ' ' << K << '\n';
        f.close();
    }
    cerr << "  " << (small ? "small " : "big   ") <<
        A << ' ' << B << ' ' << N << ' ' << K << " ==> " <<
        solution << '\n';
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    ull_t Amin = strtoul(argv[ai++], 0, 0);
    ull_t Amax = strtoul(argv[ai++], 0, 0);
    ull_t Bmin = strtoul(argv[ai++], 0, 0);
    ull_t Bmax = strtoul(argv[ai++], 0, 0);
    ull_t Nmin = strtoul(argv[ai++], 0, 0);
    ull_t Nmax = strtoul(argv[ai++], 0, 0);
    ull_t Kmin = strtoul(argv[ai++], 0, 0);
    ull_t Kmax = strtoul(argv[ai++], 0, 0);
    cerr << "n_tests=" << n_tests << 
        ", Amin="<<Amin << ", Amax="<<Amax <<
        ", Bmin="<<Bmin << ", Bmax="<<Bmax <<
        ", Nmin="<<Nmin << ", Nmax="<<Nmax <<
        ", Kmin="<<Kmin << ", Kmax="<<Kmax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        ull_t A = rand_range(Amin, Amax);
        ull_t B = rand_range(Bmin, Bmax);
        ull_t N = rand_range(Nmin, Nmax);
        ull_t K = rand_range(Kmin, Kmax);
        rc = test_case(A, B, N, K);
    }
    return rc;
}

static int test_small(int argc, char** argv)
{
    ull_t cmax = (argc > 0 ? strtoul(argv[0], nullptr, 0) : 0x10);
    int rc = 0;
    ull_t Amax = cmax;
    ull_t Bmax = cmax;
    ull_t Nmax = cmax;
    ull_t Kmax = cmax;
    ull_t n_tests = (Amax + 1)*(Bmax + 1)*Nmax*Kmax;
    ull_t ti = 0;
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
        ? ((argc > 2) && (string(argv[2]) == string("random"))
            ? test_random(argc - 3, argv + 3) : test_small(argc - 2, argv + 2))
        : real_main(argc, argv));
    return rc;
}
