// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <iterator>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<ll_t> vll_t;

static unsigned dbg_flags;

class VI
{
 public:
    VI(ll_t _v=0, u_t _i=0) : v(_v), i(_i) {}
    tuple<ll_t, u_t> as_tuple() const { return make_tuple(v, i); }
    ll_t v;
    u_t i;
};
bool operator<(const VI& vi0, const VI& vi1)
{
    bool lt = vi0.as_tuple() < vi1.as_tuple();
    return lt;
}
typedef vector<VI> vVI_t;

class Spacious
{
 public:
    Spacious(istream& fi);
    Spacious(const vll_t& _A, ll_t _K) :N(_A.size()), K(_K), A(_A) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    const vu_t& get_solution() const { return solution; }
 private:
    u_t N;
    ll_t K;
    vll_t A;
    vu_t solution;
};

Spacious::Spacious(istream& fi)
{
    fi >> N >> K;
    copy_n(istream_iterator<ull_t>(fi), N, back_inserter(A));
    solution.reserve(N);
}

void Spacious::solve_naive()
{
    vVI_t As; As.reserve(N);
    for (u_t i = 0; i < N; ++i)
    {
        As.push_back(VI(A[i], i));
    }
    sort(As.begin(), As.end());
    solution.assign(N, 0);
    for (int i = 0; i < int(N); ++i)
    {
        u_t ss = 1;
        ll_t last;
        // going down
        last = As[i].v;
        for (int j = i; j >= 0; )
        {
            for (--j; (j >= 0) && (last - As[j].v < K); --j) {}
            if (j >= 0)
            {
                last = As[j].v;
                ++ss;
            }
        }
        // going up
        last = As[i].v;
        for (u_t j = i; j < N; )
        {
            for (++j; (j < N) && (As[j].v - last < K); ++j) {}
            if (j < N)
            {
                last = As[j].v;
                ++ss;
            }
        }
        solution[As[i].i] = ss;
    }
}

void Spacious::solve()
{
    vVI_t As; As.reserve(N);
    for (u_t i = 0; i < N; ++i)
    {
        As.push_back(VI(A[i], i));
    }
    sort(As.begin(), As.end());
    solution.assign(N, 1);
    
    for (u_t i = 0, j = 0, n = 0; j < N; ++j)
    {
        solution[As[j].i] += n;
        if (As[i].v + K <= As[j].v)
        {
             ++n;
             solution[As[j].i] += 1;
             i = j;
        }
    }
    for (int i = N - 1, j = N - 1, n = 0; j >= 0; --j)
    {
        solution[As[j].i] += n;
        if (As[j].v + K <= As[i].v)
        {
             ++n;
             solution[As[j].i] += 1;
             i = j;
        }
    }
}

void Spacious::print_solution(ostream &fo) const
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

    void (Spacious::*psolve)() =
        (naive ? &Spacious::solve_naive : &Spacious::solve);
    if (solve_ver == 1) { psolve = &Spacious::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Spacious spacious(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (spacious.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        spacious.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static ll_t rand_range(ll_t nmin, ll_t nmax)
{
    ll_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

static void save_case(const char* fn, const vll_t& A, ll_t K)
{
    const u_t N = A.size();
    ofstream f(fn);
    f << "1\n" << N << ' ' << K;
    char sep = '\n';
    for (ll_t x: A)
    {
        f << sep << x; sep = ' ';
    }
    f << '\n';
    f.close();
}

static int test_case(const vll_t& A, ll_t K)
{
    int rc = 0;
    const u_t N = A.size();
    vu_t solution, solution_naive;
    bool small = (N < 0x10);
    if (dbg_flags & 0x100) { save_case("spacious-curr.in", A, K); }
    if (small)
    {
        Spacious p(A, K);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Spacious p(A, K);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = != solution_naive\n";
        save_case("spacious-fail.in", A, K);
    }
    if (rc == 0) { cerr << "  ..." <<
        (small ? " (small) " : " (large) ") << " --> ";
        if (small) { for (u_t x: solution) { cerr << ' ' << x; } }
        cerr<< '\n'; }
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
    const ll_t Kmin = strtoul(argv[ai++], nullptr, 0);
    const ll_t Kmax = strtoul(argv[ai++], nullptr, 0);
    const ll_t Amin = strtoul(argv[ai++], nullptr, 0);
    const ll_t Amax = strtoul(argv[ai++], nullptr, 0);
    cerr << "n_tests=" << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        ", Kmin=" << Kmin << ", Kmax=" << Kmax <<
        ", Amin=" << Amin << ", Amax=" << Amax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t N = rand_range(Nmin, Nmax);
        ull_t K = rand_range(Kmin, Kmax);
        vll_t A; A.reserve(N);
        while (A.size() < N)
        {
            A.push_back(rand_range(Amin, Amax));
        }
        rc = test_case(A, K);
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
