// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef unordered_map<ull_t, u_t> memo_t;

static unsigned dbg_flags;

class RangePart
{
 public:
    RangePart(istream& fi);
    RangePart(ull_t _N, ull_t _X, ull_t _Y) : N(_N), X(_X), Y(_Y) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    const vu_t get_solution() const { return solution; }
 private:
    u_t dp(ull_t target, u_t nmax);
    ull_t N, X, Y;
    vu_t solution;

    memo_t memo;
};

RangePart::RangePart(istream& fi)
{
    fi >> N >> X >> Y;
}

void RangePart::solve_naive()
{
    const ull_t mask_max = 1ull << N;
    const ull_t total = (N % 2 == 0 ? (N/2)*(N + 1) : ((N + 1)/2)*N);
    for (ull_t mask = 0; (mask < mask_max) && solution.empty(); ++mask)
    {
        ull_t alan = 0;
        for (ull_t i = 0; i < N; ++i)
        {
             ull_t bit = 1ull << i;
             if (mask & bit)
             {
                 alan += (i + 1);
             }
        }
        if (alan * Y == (total - alan) * X)
        {
            for (ull_t i = 0; i < N; ++i)
            {
                 ull_t bit = 1ull << i;
                 if (mask & bit)
                 {
                     solution.push_back(i + 1);
                 }
            }
        }
    }
}

void RangePart::solve()
{
    const ull_t total = (N % 2 == 0 ? (N/2)*(N + 1) : ((N + 1)/2)*N);
    bool possible = (total % (X + Y) == 0);
    if (possible)
    {
        ull_t alan = X * (total / (X + Y));
        ull_t top = dp(alan, min<ull_t>(alan, N));
        if (top > 0)
        {
            ull_t pending = alan;
            solution.push_back(top);
            pending -= top;
            while (pending > 0)
            {
                top = dp(pending, min(top - 1, pending));
                solution.push_back(top);
                pending -= top;
            }
            reverse(solution.begin(), solution.end());
        }
    }
}

// Alwats target >= nmax > 0
u_t RangePart::dp(ull_t target, u_t nmax)
{
    u_t ret = 0;
    const ull_t key = (target << 16) | ull_t(nmax);
    memo_t::iterator iter = memo.find(key);
    if (iter == memo.end())
    {
        if (target == nmax)
        {
            ret = nmax;
        }
        while ((ret == 0) && (nmax > 0))
        {
            ull_t target_sub = target - nmax;
            u_t ret_sub = dp(target_sub, min<ull_t>(target_sub, nmax - 1));
            if (ret_sub > 0)
            {
                ret = nmax;
            }
            else
            {
                --nmax;
            }
        }
        memo.insert(iter, memo_t::value_type{key, ret});
    }
    else
    {
        ret = iter->second;
    }
    return ret;
}

void RangePart::print_solution(ostream &fo) const
{
    if (solution.empty())
    {
        fo << " IMPOSSIBLE";
    }
    else
    {
        fo << " POSSIBLE\n" << solution.size() << '\n';
        const char* sep = "";
        for (u_t x: solution) { fo << sep << x; sep = " "; }
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

    void (RangePart::*psolve)() =
        (naive ? &RangePart::solve_naive : &RangePart::solve);
    if (solve_ver == 1) { psolve = &RangePart::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        RangePart rangepart(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (rangepart.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        rangepart.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static void save_case(const char* fn, ull_t N, ull_t X, ull_t Y)
{
    ofstream f(fn);
    f << "1\n" << N << ' ' << X << ' ' << Y << '\n';
    f.close();
}

static int test_case(ull_t N, ull_t X, ull_t Y)
{
    int rc = 0;
    vu_t solution, solution_naive;
    bool small = (N <= 20);
    if (dbg_flags & 0x100) { save_case("rangepart-curr.in", N, X, Y); }
    if (small)
    {
        RangePart p(N, X, Y);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        RangePart p(N, X, Y);
        p.solve();
        solution = p.get_solution();
    }
    ull_t sum_solution = accumulate(solution.begin(), solution.end(), 0ull);
    ull_t sum_solution_naive =
        accumulate(solution_naive.begin(), solution_naive.end(), 0ull);
    if (small && (sum_solution != sum_solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << sum_solution << " != " <<
            sum_solution_naive << " = solution_naive\n";
        save_case("rangepart-fail.in", N, X, Y);
    }
    if (rc == 0) { cerr << "N="<<N << ", X="<<X << ", Y="<<Y <<
        (small ? " (small) " : " (large) ") <<
         " -> " << sum_solution << '\n'; }
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
    const ull_t Nmin = strtoul(argv[ai++], nullptr, 0);
    const ull_t Nmax = strtoul(argv[ai++], nullptr, 0);
    const ull_t Xmin = strtoul(argv[ai++], nullptr, 0);
    const ull_t Xmax = strtoul(argv[ai++], nullptr, 0);
    const ull_t Ymin = strtoul(argv[ai++], nullptr, 0);
    const ull_t Ymax = strtoul(argv[ai++], nullptr, 0);
    cerr <<
         "Nmin="<<Nmin << ", Nmax="<<Nmax <<
       ", Xmin="<<Xmin << ", Xmax="<<Xmax <<
       ", Ymin="<<Ymin << ", Ymax="<<Ymax <<
       '\n';
    ull_t ti = 0;
    for (ull_t N = Nmin; (rc == 0) && (N <= Nmax); ++N)
    {
        for (u_t X = min(Xmin, N); (rc == 0) && (X <= min(Xmax, N)); ++X)
        {
            for (u_t Y = min(Ymin, N); (rc == 0) && (Y <= min(Ymax, N)); ++Y)
            {
                if (gcd(X, Y) == 1)
                {
                    cerr << "Tested: " << ti << '\n';
                    rc = test_case(N, X, Y);
                }
            }
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
