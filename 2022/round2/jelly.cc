// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef array<ll_t, 2> all2_t;
typedef array<u_t, 2> au2_t;
typedef vector<all2_t> vall2_t;
typedef vector<au2_t> vau2_t;

static unsigned dbg_flags;

class Jelly
{
 public:
    Jelly(istream& fi);
    Jelly(const vall2_t& _children, const vall2_t& _sweets) :
        N(_children.size()), children(_children), sweets(_sweets) {}; 
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    bool is_possible() const { return !solution.empty(); }
 private:
    ll_t dist2(const all2_t& p0, const all2_t& p1) const
    {
        const ll_t dx = p0[0] - p1[0];
        const ll_t dy = p0[1] - p1[1];
        const ll_t d2 = dx*dx + dy*dy;
        return d2;
    }
    u_t N;
    vall2_t children;
    vall2_t sweets;
    vau2_t solution;
};

Jelly::Jelly(istream& fi)
{
    fi >> N;
    children.reserve(N);
    while (children.size() < N)
    {
        all2_t xy;
        fi >> xy[0] >> xy[1];
        children.push_back(xy);
    }
    sweets.reserve(N + 1);
    while (sweets.size() < N + 1)
    {
        all2_t xy;
        fi >> xy[0] >> xy[1];
        sweets.push_back(xy);
    }
}

void Jelly::solve_naive()
{
    vu_t perm(N, 0);
    iota(perm.begin(), perm.end(), 0);
    for (bool more = true; solution.empty() && more;
        more = next_permutation(perm.begin(), perm.end()))
    {
        vau2_t candidate;
        bool possible = true;
        vu_t available(N + 1, 0);
        iota(available.begin(), available.end(), 0);
        for (u_t pi = 0; possible && (pi < N); ++pi)
        {
            const u_t ci = perm[pi];
            ll_t d2_min = numeric_limits<ll_t>::max();
            u_t ai_min = N + 1, si_min = ai_min;
            for (u_t ai = 0; ai < available.size(); ++ai)
            {
                const u_t si = available[ai];
                const ll_t d2 = dist2(children[ci], sweets[si]);
                if ((d2_min > d2) || ((d2_min == d2) && (si_min == 0)))
                {
                    d2_min = d2;
                    ai_min = ai;
                    si_min = si;
                }
            }
            possible = (si_min > 0);
            candidate.push_back(au2_t{ci, si_min});
            available[ai_min] = available.back();
            available.pop_back();
        }
        if (possible && (candidate.size() == N))
        {
            solution = candidate;
        }
    }
}

void Jelly::solve()
{
    // solve_naive();
}

void Jelly::print_solution(ostream &fo) const
{
    if (solution.empty())
    {
        fo << ' ' << "IMPOSSIBLE";
    }
    else
    {
        fo << ' ' << "POSSIBLE";
        for (const au2_t& ab: solution)
        {
            fo << '\n' << ab[0] + 1 << ' ' << ab[1] + 1;
        }
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

    void (Jelly::*psolve)() =
        (naive ? &Jelly::solve_naive : &Jelly::solve);
    if (solve_ver == 1) { psolve = &Jelly::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Jelly jelly(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (jelly.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        jelly.print_solution(fout);
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

static ll_t rand_range(ll_t nmin, ll_t nmax)
{
    ll_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

static void save_case(
    const char* fn, const vall2_t& children, const vall2_t& sweets)
{
    const u_t N = children.size();
    ofstream f(fn);
    f << "1\n" << N << '\n';
    for (const all2_t& xy: children) { f << xy[0] << ' ' << xy[1] << '\n'; }
    for (const all2_t& xy: sweets) { f << xy[0] << ' ' << xy[1] << '\n'; }
    f.close();
}

static int test_case(
    bool& possible,
    const vall2_t& children,
    const vall2_t& sweets)
{
    int rc = 0;
    possible = false;
    bool possible_naive = false;
    const u_t N = children.size();
    bool small = (N <= 10);
    if (dbg_flags & 0x100) { save_case("jelly-curr.in", children, sweets); }
    if (small)
    {
        Jelly p{children, sweets};
        p.solve_naive();
        possible_naive = p.is_possible();
    }
    {
        Jelly p{children, sweets};
        p.solve();
        possible = p.is_possible();
    }
    if (small && (possible != possible_naive))
    {
        rc = 1;
        cerr << "Failed: possible = " << possible << " != " <<
            possible_naive << " = possible_naive\n";
        save_case("jelly-fail.in", children, sweets);
    }
    if (rc == 0) { cerr << "  ..." <<
        (small ? " (small) " : " (large) ") << " --> " <<
        (possible ? "POSSIBLE" : "IMPOSSIBLE") << '\n'; }
    return rc;
}

static void random_points(
    vall2_t& points,
    u_t N, 
    ll_t Xmin,
    ll_t Xmax,
    ll_t Ymin,
    ll_t Ymax)
{
    set<all2_t> set_points;
    points.size();
    while (points.size() < N)
    {
        all2_t xy;
        bool found = false;
        while (found)
        {
            xy[0] = rand_range(Xmin, Xmax);
            xy[1] = rand_range(Ymin, Ymax);
            found = (set_points.find(xy) == set_points.end());
        }
        points.push_back(xy);
    }
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
    const ll_t Xmin = strtol(argv[ai++], nullptr, 0);
    const ll_t Xmax = strtol(argv[ai++], nullptr, 0);
    const ll_t Ymin = strtol(argv[ai++], nullptr, 0);
    const ll_t Ymax = strtol(argv[ai++], nullptr, 0);
    cerr << "n_tests=" << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        ", Xmin=" << Xmin << ", Xmax=" << Xmax <<
        ", Ymin=" << Ymin << ", Ymax=" << Ymax <<
        '\n';
    for (u_t ti = 0, n_impossible = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests <<
            " #(impossible)=" << n_impossible << '\n';
        const u_t N = rand_range(Nmin, Nmax);
        vall2_t children, sweets;
        random_points(children, N, Xmin, Xmax, Ymin, Ymax);
        random_points(sweets, N + 1, Xmin, Xmax, Ymin, Ymax);
        bool possible = false;
        rc = test_case(possible, children, sweets);
        if (!possible) { ++n_impossible; }
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
