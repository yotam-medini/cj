// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
// #include <map>
// #include <set>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<vvu_t> vvvu_t;
typedef vector<bool> vb_t;

static unsigned dbg_flags;

class Level
{
 public:
    Level(istream& fi);
    Level(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    void set_cycles();
    void print_cycles() const;
    u_t N;
    vu_t _P;
    vu_t P; // shift to 0
    vu_t solution;
    // vvu_t cycles; // size -> initial indices
    vvvu_t sz_cycles; // size 
};

Level::Level(istream& fi)
{
    fi >> N;
    copy_n(istream_iterator<u_t>(fi), N, back_inserter(_P));
    P.reserve(N);
    solution.reserve(N);
    for (u_t n: _P) { P.push_back(n - 1); }
}

void Level::solve_naive()
{
    set_cycles();
    for (u_t level = 1; level <= N; ++level)
    {
        if (!sz_cycles[level].empty())
        {
            solution.push_back(0);
        }
        else
        {
            u_t n_swaps = 0;
            u_t sz_max = N;
            for ( ; sz_cycles[sz_max].empty(); --sz_max) {}
            if (level == 1)
            {
                vu_t& cycle = sz_cycles[sz_max].back();
                if (sz_max == 2)
                {
                    n_swaps = 1;
                    swap(cycle[0], cycle[1]);
                    for (u_t i: {0, 1})
                    {
                        vu_t c1; c1.push_back(cycle[i]);
                        sz_cycles[1].push_back(c1);
                    }
                    sz_cycles[2].pop_back();
                }
                else
                {
                    n_swaps = 2;
                    vu_t c1;
                    c1.push_back(cycle.back());
                    cycle.pop_back();
                    // move cycle sz_cycles -> sz_cycles-1
                    sz_cycles[sz_max - 1].push_back(cycle);
                    sz_cycles[sz_max].pop_back();
                }
            }
            else
            {
                vu_t cycle;
                swap(cycle, sz_cycles[level - 1].back());
                sz_cycles[level - 1].pop_back();
                u_t sz_low = 1;
                for ( ; sz_cycles[sz_low].empty(); ++sz_low) {}
                if (sz_low == 1)
                {
                    swap(cycle.back(), sz_cycles[1].back()[0]);
                    cycle.push_back(sz_cycles[1].back()[0]);
                    sz_cycles[1].pop_back();
                    n_swaps = 1;
                }
                else  //  0123   4567
                {     //  1230   5674
                      //  1230   5647
                      //  1237   5640
                      vu_t cycle_low;
                      swap(cycle_low, sz_cycles[sz_low].back());
                      sz_cycles[sz_low].pop_back();
                      swap(cycle_low[sz_low - 2], cycle_low[sz_low - 1]);
                      cycle.push_back(cycle_low.back());
                      cycle_low.pop_back();
                      sz_cycles[sz_low - 1].push_back(cycle_low);
                      n_swaps = 2;
                }
                sz_cycles[level].push_back(cycle);                
            }
            solution.push_back(n_swaps);
        }
        if (dbg_flags & 0x2) { cerr << "level="<<level<<'\n'; print_cycles(); }
    }
}

void Level::solve()
{
    solve_naive();
}

void Level::set_cycles()
{
    sz_cycles = vvvu_t(N + 1, vvu_t());
    vb_t used(N, false);
    for (u_t initial = 0; initial < N; ++initial)
    {
        if (!used[initial])
        {
            vu_t cycle;
            used[initial] = true;
            cycle.push_back(initial);
            u_t sz = 1;
            for (u_t i = P[initial]; i != initial; i = P[i], ++sz)
            {
                used[i] = true;
                cycle.push_back(i);
            }
            // cycles[sz].push_back(initial);
            sz_cycles[sz].push_back(cycle);
        }
    }
    if (dbg_flags & 0x1) { print_cycles(); }
}

void Level::print_cycles() const
{
    cerr << "{ cycles:\n";
    for (u_t sz = 1; sz <= N; ++sz) {
        const vvu_t& cycles = sz_cycles[sz];
        if (!cycles.empty()) {
            cerr << "  [" << sz << "]:\n";
            for (const vu_t& cycle: cycles) {
                cerr << "   ";
                for (u_t i: cycle) { cerr << ' ' << i; }
                cerr << '\n';
            }
        }
    }
    cerr << "}\n";
}

void Level::print_solution(ostream &fo) const
{
    for (u_t n: solution) { fo << ' ' << n; }
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

    void (Level::*psolve)() =
        (naive ? &Level::solve_naive : &Level::solve);
    if (solve_ver == 1) { psolve = &Level::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Level level(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (level.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        level.print_solution(fout);
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
    if (dbg_flags & 0x100) { save_case("level-curr.in"); }
    if (small)
    {
        Level p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Level p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("level-fail.in");
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
