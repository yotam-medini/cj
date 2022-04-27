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
typedef vector<ll_t> vll_t;

static unsigned dbg_flags;

class Candies
{
 public:
    Candies(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t N, O;
    ll_t D;
    ll_t A, B, C, L;
    ull_t M;
    vll_t S;
    bool possible;
    ll_t solution;
};

Candies::Candies(istream& fi) : possible(false), solution(0)
{
    // copy_n(istream_iterator<u_t>(fi), N, back_inserter(a));
    fi >> N >> O >> D;
    ll_t X1, X2;
    fi >> X1 >> X2 >> A >> B >> C >> M >> L;
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
            if ((n_odd <= O) && (sum_be <= D))
            {
                if ((!possible) || (solution < sum_be))
                {
                    possible = true;
                    solution = sum_be;
                }
            }
        }
    }
}

void Candies::solve()
{
    solve_naive();
}

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

static int test_case(int argc, char ** argv)
{
    int rc = rand_range(0, 1);
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
