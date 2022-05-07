// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;
typedef vector<u_t> vu_t;
typedef vector<u_t> vu_t;
typedef vector<ll_t> vll_t;

static unsigned dbg_flags;

ull_t sqroot(ull_t n)
{
    ull_t r = 0;
    if (n > 0)
    {
        r = 1;
        for (bool iter = true; iter; )
        {
            ull_t d = n / r;
            ull_t rmin = min(r, d);
            ull_t rmax = max(r, d);
            if (rmax - rmin > 1)
            {
                ull_t rnext = (r + d)/2;
                r = rnext;
            }
            else
            {
                iter = false;
                r =  (rmax * rmax <= n) ? rmax : rmin;
            }
        }
    }
    return r;
}

class Squary
{
 public:
    Squary(istream& fi);
    Squary(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    u_t N, K;
    vll_t E;
    bool possible;
    vll_t solution;
};

Squary::Squary(istream& fi) : possible(false)
{
    fi >> N >> K;
    copy_n(istream_iterator<ll_t>(fi), N, back_inserter(E));
}

void Squary::solve_naive()
{
    ll_t sum_e = 0; // accumulate(E.begin(), e.end(), 0ull);
    ll_t sum_e2 = 0;
    for (ll_t x: E)
    {
         sum_e += x;
         sum_e2 += (x*x);
    }
    if (sum_e == 0)
    {
        if (sum_e2 == 0)
        {
            possible = true;
            solution.push_back(0);            
        }
    }
    else
    {
        // x*2*sum_e = sum_e2 - (sum_e * sum_e)
        const ll_t rhs = sum_e2 - (sum_e * sum_e);
        if (rhs % (2*abs(sum_e)) == 0)
        {
            possible = true;
            ll_t x = rhs / (2*sum_e);
            solution.push_back(x);
        }
    }
}

void Squary::solve()
{
#if 1
    if (K <= 1)
    {
        solve_naive();
    }
    else
#endif
    {
        ll_t e1 = accumulate(E.begin(), E.end(), 0ll);
        ll_t e2 = accumulate(E.begin(), E.end(), 0ll, 
            [](ll_t lhs, ll_t rhs) { return lhs + rhs*rhs; });
        if (dbg_flags & 0x1) { cerr<<"e1="<<e1 << ", e2="<<e2 << '\n'; }
        ll_t x1 = 0;
        const ll_t e1sqr = e1*e1;
        if (e1 == 0)
        {
            x1 = sqroot(e2/2);
            if (x1*x1 < e2/2)
            {
                ++x1;
            }
        }
        else if (e1sqr < e2)
        {
            ll_t ae1 = llabs(e1);
            x1 = (e2 - e1sqr + 2*ae1 - 1) / (2*ae1);
            if (e1 < 0) { x1 = -x1; }
            const ll_t e1x = e1 + x1;
            const ll_t e2x = e2 + x1*x1;
            if (e1x * e1x < e2x) 
            {
                cerr << "This is sad\n";
            }
        }
        solution.push_back(x1);
        if (x1 != 0)
        {
            e1 += x1;
            e2 += (x1*x1);    
        }
        const ll_t e1e1 = e1*e1;
        while ((solution.size() < K) && (e1e1 > e2))
        {
            ll_t delta = e1e1 - e2;
            ll_t r = sqroot(delta/2);
            solution.push_back(r);
            solution.push_back(-r);
            e2 += 2*r*r;
        }
        possible = (solution.size() <= K) && (e1e1 == e2);
    }
}

void Squary::print_solution(ostream &fo) const
{
    if (possible)
    {
        for (ll_t x: solution)
        {
            fo << ' ' << x;
        }
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

    void (Squary::*psolve)() =
        (naive ? &Squary::solve_naive : &Squary::solve);
    if (solve_ver == 1) { psolve = &Squary::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Squary squary(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (squary.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        squary.print_solution(fout);
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
        Squary p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Squary p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution="<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        ofstream f("squary-fail.in");
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
