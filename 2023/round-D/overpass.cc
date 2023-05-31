// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <iterator>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<double> vd_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;

static unsigned dbg_flags;

class NodeStatNaive
{
 public:
    NodeStatNaive(ull_t s=0, u_t n=0) : sum_paths(s), n_descendants(n) {}
    ull_t sum_paths;
    u_t n_descendants;
};
typedef vector<NodeStatNaive> vNodeStatNaive_t;

class Overpass
{
 public:
    Overpass(istream& fi);
    Overpass(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    void build_we_vadjs();
    ull_t get_sum_paths(
        u_t& n_descendants,
        const vvu_t& vadjs,
        u_t pre_root,
        u_t root) const;
    u_t W, E, C;
    vu_t X, F;
    vau2_t AB;
    vd_t solution;

    vvu_t we_vadjs[2];
};

Overpass::Overpass(istream& fi)
{
    fi >> W >> E >> C;
    X.reserve(W);
    F.reserve(E);
    X.push_back(0); // unused
    F.push_back(0); // unused
    copy_n(istream_iterator<u_t>(fi), W - 1, back_inserter(X));
    copy_n(istream_iterator<u_t>(fi), E - 1, back_inserter(F));
    AB.reserve(C);
    while (AB.size() < C)
    {
        u_t a, b;
        fi >> a >> b;
        AB.push_back(au2_t{a, b});
    }
}

void Overpass::solve_naive()
{
    build_we_vadjs();
    u_t n_dummy;
    for (const au2_t& ab: AB)
    {
        double w_sum_paths = get_sum_paths(n_dummy, we_vadjs[0], 0, ab[0]);
        double e_sum_paths = get_sum_paths(n_dummy, we_vadjs[1], 1, ab[1]);
        double average = w_sum_paths / (W - 1.) + e_sum_paths / (E - 1.) + 1.;
        solution.push_back(average);
    }
}

void Overpass::build_we_vadjs()
{
    for (u_t wei: {0, 1})
    {
        const u_t n = (wei == 0 ? W : E);
        const vu_t& parents = (wei == 0 ? X : F);
        vvu_t& vadjs =  we_vadjs[wei];
        vadjs.assign(n + 1, vu_t());
        for (u_t c = 1; c < n; ++c)
        {
            vadjs[parents[c]].push_back(c);
        }
        for (u_t c = 1; c < n; ++c)
        {
            vadjs[c].push_back(parents[c]);
        }
    }
}

ull_t Overpass::get_sum_paths(
    u_t& n_descendants,
    const vvu_t& vadjs,
    u_t pre_root,
    u_t root) const
{
    ull_t sum_paths = 0;
    n_descendants = 0;
    for (u_t c: vadjs[root])
    {
        if (c != pre_root)
        {
            u_t n_sub_desc = 0;
            ull_t sum_sub_paths = get_sum_paths(n_sub_desc, vadjs, root, c);
            n_descendants += n_sub_desc + 1;
            sum_paths += sum_sub_paths + 1;
        }
    }
    return sum_paths;
}

void Overpass::solve()
{
    solve_naive();
}

void Overpass::print_solution(ostream &fo) const
{
    for (double x: solution) { fo << ' ' << x; }
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

    void (Overpass::*psolve)() =
        (naive ? &Overpass::solve_naive : &Overpass::solve);
    if (solve_ver == 1) { psolve = &Overpass::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Overpass overpass(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (overpass.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        overpass.print_solution(fout);
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
    if (dbg_flags & 0x100) { save_case("overpass-curr.in"); }
    if (small)
    {
        Overpass p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Overpass p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("overpass-fail.in");
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
    const u_t Nmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Nmax = strtoul(argv[ai++], nullptr, 0);
    cerr << "n_tests=" << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        '\n';
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
