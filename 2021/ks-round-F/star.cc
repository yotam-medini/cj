// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <utility>
#include <unordered_set>
#include <vector>
// #include <iterator>
// #include <map>
// #include <set>

#include <cmath>
#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef long long ll_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<u_t, 2> au2_t;
typedef array<ll_t, 2> all2_t;
typedef vector<u_t> vu_t;
typedef vector<au2_t> vau2_t;
typedef vector<all2_t> vall2_t;

static unsigned dbg_flags;

void permutation_first(vu_t &p, u_t n)
{
    p = vu_t(n);
    iota(p.begin(), p.end(), 0);
}

bool permutation_next(vu_t &p)
{
    u_t n = p.size();

    // find the last 'in order'
    int j = n - 1 - 1;
    while ((j >= 0) && (p[j] > p[j + 1]))
    {
        --j;
    }
    bool ret = j >= 0;
    if (ret)
    {
        // Find last > p[j]. Must find since p[j] < p[j+1]
        u_t l;
        for (l = n - 1; p[j] >= p[l]; --l);
        swap(p[j], p[l]);
        reverse(p.begin() + j + 1, p.end());
    }
    return ret;
}

void combination_first(vu_t &c, u_t n, u_t k)
{
    c = vu_t(k);
    iota(c.begin(), c.end(), 0);
}

bool combination_next(vu_t &c, u_t n)
{
    u_t j = 0, k = c.size();

    // sentinels (Knuth) (Why the second (0) ??!)
    c.push_back(n);  c.push_back(0);

    while ((j < k) && (c[j] + 1 == c[j + 1]))
    {
        c[j] = j;
        ++j;
    }
    bool ret = j < k;
    if (ret)
    {
        ++(c[j]);
    }

    c.pop_back(); c.pop_back(); // the sentinels

    return ret;
}

class Star
{
 public:
    Star(istream& fi);
    Star(const vall2_t& _stars, const all2_t& _blue_star) :
        N(_stars.size()), stars(_stars), blue_star(_blue_star),
        possible(false), solution(-1.)
        {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    double get_solution() const { return possible ? solution : 1.; }
 private:
    void all_tri() { all_ngon(3); }
    void all_quad() { all_ngon(4); }
    void all_ngon(u_t ng);
    void check_poly(const vu_t& poly);
    ll_t det_side(const all2_t& p0, const all2_t& p1, const all2_t& bp);
    u_t N;
    vall2_t stars;
    all2_t blue_star;
    bool possible;
    double solution;
};

Star::Star(istream& fi) : possible(false), solution(-1)
{
    fi >> N;
    stars.reserve(N);
    while (stars.size() < N)
    {
        all2_t star;
        fi >> star[0] >> star[1];
        stars.push_back(star);
    }
    fi >> blue_star[0] >> blue_star[1];
}

void Star::solve_naive()
{
    if (N >= 3)
    {
        all_tri();
        if ((!possible) && (N >= 4))
        {
            all_quad();
        }
    }
}

void Star::all_ngon(u_t ng)
{
    vu_t comb;
    combination_first(comb, N, ng);
    vu_t poly(ng, N);
    for (bool cmore = true; cmore; cmore = combination_next(comb, N))
    {
        vu_t perm;
        permutation_first(perm, ng);
        for (bool pmore = true; pmore; pmore = permutation_next(perm))
        {
            for (u_t i = 0; i < ng; ++i)
            {
                poly[i] = comb[perm[i]];
            }
            check_poly(poly);
        }
    }
}

void Star::check_poly(const vu_t& poly)
{
    bool left_side = true;
    const u_t sz = poly.size();
    for (u_t i = 0; left_side && (i < sz); ++i)
    {
        ll_t d = det_side(stars[i], stars[(i + 1) % sz], blue_star);
        left_side = (d < 0);
    }
    if (left_side)
    {
        double peri = 0;
        for (u_t i = 0; left_side && (i < sz); ++i)
        {
             const all2_t& p0 = stars[i];
             const all2_t& p1 = stars[(i + 1) % sz];
             const all2_t delta{p1[0] - p0[0], p1[1] - p0[1]};
             const double dsqare = delta[0]*delta[0] + delta[1]*delta[1];
             const double dist01 = sqrt(dsqare);
             peri += dist01;
        }
        if ((!possible) || (solution > peri))
        {
            possible = true;
            solution = peri;
        }
    }
}

void Star::solve()
{
    solve_naive();
}

ll_t Star::det_side(const all2_t& p0, const all2_t& p1, const all2_t& bp)
{
    const ll_t A = p1[1] - p0[1];
    const ll_t B = p0[0] - p1[0];
    // const ll_t C = (p1[0] * p0[1]) - (p0[0] * p1[1]);
    const ll_t C = p1[0] * p0[1] - p0[0] * p1[1];
    const ll_t r = A*bp[0] + B*bp[1] + C;
    return r;
}

static string dtos(const double x, u_t precision=6)
{
    char buf[0x20];
    snprintf(buf, sizeof(buf), "%.*f", precision, x);
    string ret(buf);
    return ret;
}

void Star::print_solution(ostream &fo) const
{
    if (possible)
    {
        fo << ' ' << dtos(solution);
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

    void (Star::*psolve)() =
        (naive ? &Star::solve_naive : &Star::solve);
    if (solve_ver == 1) { psolve = &Star::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Star star(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (star.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        star.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_case(const vall2_t& stars, const all2_t& blue_star)
{
    int rc = 0;
    const u_t N = stars.size();
    const bool small = (N <= 10);
    double solution = -1., solution_naive = -1.;
    if (small)
    {
        Star p(stars, blue_star);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Star p(stars, blue_star);
        p.solve();
        solution = p.get_solution();
    }
    if (small)
    {
        double demom = abs(solution) + abs(solution_naive) + 1;
        double error = abs(solution - solution_naive) / demom;
        if (error > 1./1000000.)
        {
            rc = 1;
            cerr << "solution = " << solution << " != " <<
                solution_naive << " = solution_naive\n";
            ofstream f("star-fail.in");
            f << "1\n" << N << '\n';
            for (const all2_t& star: stars)
            {
                f << star[0] << ' ' << star[1] << '\n';
            }
            f << blue_star[0] << ' ' << blue_star[1] << '\n';
            f.close();
        }
    }
    return rc;
}

u_t rand_range(u_t nmin, u_t nmax)
{
    u_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

class Hash_AI2 {
 public:
  size_t operator()(const all2_t& a) const {
    ll_t n = a[0] ^ (a[1] << 7);
    return hash_int(n);
  }
 private:
  hash<ll_t> hash_int;
};


static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t Nmin = strtoul(argv[ai++], 0, 0);
    u_t Nmax = strtoul(argv[ai++], 0, 0);
    cerr << "Nmin=" << Nmin << ", Nmax="<<Nmax << '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        static u_t STAR_MAX = 1000000;
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t N = rand_range(Nmin, Nmax);
        unordered_set<all2_t, Hash_AI2> taken;
        all2_t blue_star{rand_range(0, STAR_MAX), rand_range(0, STAR_MAX)};
        taken.insert(blue_star);
        vall2_t stars;
        while (stars.size() < N)
        {
            all2_t star{rand_range(0, STAR_MAX), rand_range(0, STAR_MAX)};
            if (taken.find(star) == taken.end())
            {
                 stars.push_back(star);
                 taken.insert(star);
            }
        }
        rc = test_case(stars, blue_star);
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
