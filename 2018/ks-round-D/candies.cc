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
typedef long long ll_t;
typedef vector<u_t> vu_t;
typedef vector<ll_t> vll_t;

static unsigned dbg_flags;

class Candies
{
 public:
    Candies(istream& fi);
    Candies(
        u_t _N, u_t _O, ll_t _D,
        ll_t _X1, ll_t _X2, ll_t _A, ll_t _B, ll_t _C, ull_t _M, ll_t _L);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ll_t get_solution(bool& poss) const { return solution; }
 private:
    void generate_sweet();
    void candidate_check(ll_t candidate)
    {
        if ((candidate <= D) && ((!possible) || (solution < candidate)))
        {
            possible = true;
            solution = candidate;
        }
    }
    u_t N, O;
    ll_t D;
    ll_t X1, X2, A, B, C;
    ull_t M;
    ll_t L;
    vll_t S;
    bool possible;
    ll_t solution;
};

Candies::Candies(istream& fi) : possible(false), solution(0)
{
    // copy_n(istream_iterator<u_t>(fi), N, back_inserter(a));
    fi >> N >> O >> D;
    fi >> X1 >> X2 >> A >> B >> C >> M >> L;
    generate_sweet();
}

Candies::Candies(
    u_t _N, u_t _O, ll_t _D,
    ll_t _X1, ll_t _X2, ll_t _A, ll_t _B, ll_t _C, ull_t _M, ll_t _L
) : N(_N), O(_O), D(_D),
    X1(_X1), X2(_X2), A(_A), B(_B), C(_C), M(_M), L(_L),
    possible(false), solution(0)
{
    generate_sweet();
}
    
void Candies::generate_sweet()
{
    S.reserve(N);
    vll_t X;
    X.reserve(N);
    X.push_back(X1);
    X.push_back(X2);
    S.push_back(X1 + L);
    S.push_back(X2 + L);
    for (u_t i = 2; i < N; ++i)
    {
        // Xi = (A × Xi - 1 + B × Xi - 2 + C) modulo M, for i = 3 to N.
        ll_t xi = (A*X[i - 1] + B*X[i - 2] + C) % M;
        X.push_back(xi);
        S.push_back(xi + L);
    }
    if (dbg_flags & 0x1) {
        cerr << "S:"; for (ll_t s: S) { cerr << ' ' << s; } cerr << '\n'; }
}

void Candies::solve_naive()
{
    for (u_t b = 0; b < N; ++b)
    {
        for (u_t e = b + 1; e <= N; ++e)
        {
            u_t n_odd = 0;
            ll_t sum_be = 0;
            for (u_t i = b; i < e; ++i)
            {
                ll_t x = S[i];
                n_odd += (x % 2 == 0 ? 0 : 1);
                sum_be += x;
            }
            if (n_odd <= O)
            {
                candidate_check(sum_be);
            }
        }
    }
}

ll_t v_at_minus1(const vll_t& v, u_t i)
{
    ll_t ret = ((0 < i) && (i <= v.size()) ? v[i - 1] : 0);
    return ret;
}

void Candies::solve()
{
    // solve_naive();
    // static const u_t undef(-1);
    vll_t sub_sums; sub_sums.reserve(N + 1);
    vu_t odd_count; odd_count.reserve(N + 1);
    sub_sums.push_back(0);
    odd_count.push_back(0);
    for (u_t i = 0; i < N; ++i)
    {
        sub_sums.push_back(sub_sums.back() + S[i]);
        u_t zo = (S[i] % 2 == 0 ? 0 : 1);
        odd_count.push_back(odd_count.back() + zo);
    }
    for (u_t b = 0; b < N; ++b)
    {
        for (u_t e = b + 1; e <= N; ++e)
        {
            u_t n_odd = odd_count[e] - odd_count[b];
            if (n_odd <= O)
            {
                ll_t be_sum = sub_sums[e] - sub_sums[b];
                candidate_check(be_sum);
            }
        }
    }
}

#if 0
void Candies::solve()
{
    // solve_naive();
    // static const u_t undef(-1);
    vll_t sub_sums;
    vu_t odd_idx;
    vu_t pos_idx; // after odd
    ll_t vsum_last = 0;
    u_t curr_pos_start = 0;
    for (int i = 0; i < int(N); ++i)
    {
        const ll_t v = S[i];
        if ((O > 0) || ((v % 2) == 0))
        {
            candidate_check(v); // for single negative
        }
        vsum_last += v;
        sub_sums.push_back(vsum_last);
        ll_t pre_sum = v_at_minus1(sub_sums, curr_pos_start);
        ll_t curr_sum = vsum_last - pre_sum;
        candidate_check(curr_sum);
        if (curr_sum < 0)
        {
            curr_pos_start = i + 1;
        }
        if (v % 2 != 0)
        {
            odd_idx.push_back(i);
            pos_idx.push_back(i);
            u_t n_odds = odd_idx.size();
            if (n_odds > O)
            {
                curr_pos_start = max(curr_pos_start, pos_idx[n_odds - O]);
            }
        }
        if ((!pos_idx.empty()) && (S[pos_idx.back()] < 0) && (v >= 0))
        {
            pos_idx.back() = i;
        }
        {
            pre_sum = v_at_minus1(sub_sums, curr_pos_start);
            ll_t candid = sub_sums.back() - pre_sum;
            candidate_check(candid);
        }
    }
}
#endif

void Candies::print_solution(ostream &fo) const
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

    void (Candies::*psolve)() =
        (naive ? &Candies::solve_naive : &Candies::solve);
    if (solve_ver == 1) { psolve = &Candies::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Candies candies(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (candies.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        candies.print_solution(fout);
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

static int test_case(
    u_t N, u_t O, ll_t D,
    ll_t X1, ll_t X2, ll_t A, ll_t B, ll_t C, ull_t M, ll_t L
)
{
    int rc = 0;
    bool possible = false, possible_naive = false;
    ll_t solution = 0, solution_naive = 0;
    bool small = (N < 0x20);
    if (small)
    {
        Candies p(
            N, O, D,
            X1, X2, A, B, C, M, L);
        p.solve_naive();
        solution_naive = p.get_solution(possible_naive);
    }
    {
        Candies p(
            N, O, D,
            X1, X2, A, B, C, M, L);
        p.solve();
        solution = p.get_solution(possible);
    }
    if (small && ((solution != solution_naive) || (possible != possible_naive)))
    {
        rc = 1;
        cerr << "Fail: inconsistent\n";
        ofstream f("candies-fail.in");
        f << "1\n" << N << ' ' << O << ' ' << D << '\n' <<
            X1 << ' ' << X2 << ' ' <<
            A << ' ' << B << ' ' << C << ' ' << M << ' ' << L << '\n';
            
        f.close();
    }
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    ull_t Nmin = strtoul(argv[ai++], 0, 0);
    ull_t Nmax = strtoul(argv[ai++], 0, 0);
    ull_t Omin = strtoul(argv[ai++], 0, 0);
    ull_t Omax = strtoul(argv[ai++], 0, 0);
    ll_t Dmin = strtoul(argv[ai++], 0, 0);
    ll_t Dmax = strtoul(argv[ai++], 0, 0);
    ull_t X12max = strtoul(argv[ai++], 0, 0);
    ull_t Amax = strtoul(argv[ai++], 0, 0);
    ull_t Bmax = strtoul(argv[ai++], 0, 0);
    ull_t Cmax = strtoul(argv[ai++], 0, 0);
    ull_t Mmax = strtoul(argv[ai++], 0, 0);
    ll_t Lmax = strtoul(argv[ai++], 0, 0);
    cerr << "n_tests="<<n_tests <<
        ", Nmin="<<Nmin << ", Nmax="<<Nmax <<
        ", Omin="<<Omin << ", Omax="<<Omax <<
        ", Dmin="<<Dmin << ", Dmax="<<Dmax <<
        ", X12Max="<<X12max <<
        ", Amax="<<Amax <<
        ", Bmax="<<Bmax <<
        ", Cmax="<<Cmax <<
        ", Mmax="<<Mmax <<
        ", Lmax="<<Lmax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        ull_t N = rand_range(Nmin, Nmax);
        ull_t O = rand_range(Omin, Omax);
        ll_t D = Dmin + rand_range(0, Dmax - Dmin);
        ll_t X1 = rand_range(0, X12max);
        ll_t X2 = rand_range(0, X12max);
        ll_t A = rand_range(0, Amax);
        ll_t B = rand_range(0, Bmax);
        ll_t C = rand_range(0, Cmax);
        ull_t M = rand_range(1, Mmax);
        ll_t L = -(ll_t(rand_range(0, Lmax)));
        rc = test_case(
            N, O, D,
            X1, X2, A, B, C, M, L);
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
