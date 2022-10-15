// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
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
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;

static unsigned dbg_flags;


class Flower
{
 public:
    Flower(ull_t x=0, ull_t y=0, ll_t c=0) : X(x), Y(y), C(c) {}
    ull_t X, Y;
    ll_t C;
};
typedef vector<Flower> vflower_t;


class Cute
{
 public:
    Cute(istream& fi);
    Cute(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    void try_perm(const vu_t& perm);
    void try_energy(ll_t energy)
    {
        if (solution < energy)
        {
            solution = energy;
        }
    }
    u_t N;
    ll_t E;
    vflower_t flowers;
    ll_t solution;
};

Cute::Cute(istream& fi) : solution(0)
{
    fi >> N >> E;
    flowers.reserve(N);
    while (flowers.size() < N)
    {
        ull_t x, y;
        ll_t c;
        fi >> x >> y >> c;
        flowers.push_back(Flower(x, y, c));
    }
}

void Cute::solve_naive()
{
    vu_t perm; perm.reserve(N);
    while (perm.size() < N) { perm.push_back(perm.size()); }
    for (bool more = true; more;
        more = next_permutation(perm.begin(), perm.end()))
    {
         try_perm(perm);
    }
}

void Cute::try_perm(const vu_t& perm)
{
    ull_t xcurr = 0, ycurr = 1000000000000000000;
    bool possible = true;
    bool positive = true;
    ll_t energy = 0;
    for (u_t fi = 0; possible && (fi < N); ++fi)
    {
        const Flower& f = flowers[perm[fi]];
        possible = f.Y <= ycurr;
        if (possible)
        {
            // const ll_t dy = ycurr - f.Y;
            // const ll_t dx = (xcurr < f.X ? f.X - xcurr : xcurr - f.X);
            possible = true; // (dy <= dx);
            if (possible)
            {
                if (positive)
                {
                    if (f.X < xcurr)
                    {
                        energy -= E;
                        positive = false;
                    }
                }
                else // !positive
                {
                    if (xcurr < f.X)
                    {
                        energy -= E;
                        positive = true;
                    }
                }
                energy += f.C;
            }
            try_energy(energy);
            xcurr = f.X;
            ycurr = f.Y;
        }
    }
}

void Cute::solve()
{
    solve_naive();
}

void Cute::print_solution(ostream &fo) const
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

    void (Cute::*psolve)() =
        (naive ? &Cute::solve_naive : &Cute::solve);
    if (solve_ver == 1) { psolve = &Cute::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Cute cute(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (cute.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        cute.print_solution(fout);
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
    if (dbg_flags & 0x100) { save_case("cute-curr.in"); }
    if (small)
    {
        Cute p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Cute p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("cute-fail.in");
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
