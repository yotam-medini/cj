// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
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
typedef unordered_map<u_t, u_t> u2u_t;

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
    void set_flat_cycles_sizes();
    u_t gen_level(u_t level);
    void print_cycles() const;
    bool sub_sum_exists(u_t target, u_t top_idx, u_t max_summands);
    u_t N;
    vu_t _P;
    vu_t P; // shift to 0
    vu_t solution;
    // vvu_t cycles; // size -> initial indices
    vvvu_t sz_cycles; // size 
    u_t cycle_max_size;
    vu_t flat_cycle_sizes;
    vu_t flat_cycle_sizes_pfx_sum;
    u2u_t size_to_num_cycles; // memo
};

Level::Level(istream& fi) : cycle_max_size(0)
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
            if (cycle_max_size > level)
            {
                n_swaps = 1; // cut
            }
            else
            {
                n_swaps = gen_level(level);
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
            if (cycle_max_size < sz)
            {
                cycle_max_size = sz;
            }
        }
    }
    if (dbg_flags & 0x1) { print_cycles(); }
    set_flat_cycles_sizes();
}

void Level::set_flat_cycles_sizes()
{
    flat_cycle_sizes_pfx_sum.push_back(0);
    for (u_t sz = 1; sz <= cycle_max_size; ++sz)
    {
        size_t nsz = sz_cycles[sz].size();
        flat_cycle_sizes.insert(flat_cycle_sizes.end(), nsz, sz);
        for (u_t k = 0; k < nsz; ++k)
        {
            flat_cycle_sizes_pfx_sum.push_back(
                flat_cycle_sizes_pfx_sum.back() + sz);
        }
    }
}

u_t Level::gen_level(u_t level)
{
    u_t n_cycles = 0;
    u_t pending = level;
    bool full_consume = true;
    u_t top_flat_index = flat_cycle_sizes_pfx_sum.size() - 1;
    for (u_t sz = cycle_max_size; full_consume && (pending > 0); --sz)
    {
        full_consume = pending >= sz * sz_cycles[sz].size();
        u_t q = pending / sz;
        u_t nc = min<u_t>(q, sz_cycles[sz].size());
        pending -= nc * sz;
        n_cycles += nc;
        top_flat_index -= nc;
    }
    u_t n_swaps = n_cycles - 1;
    if  (pending > 0)
    {
        if (sub_sum_exists(pending, top_flat_index, pending))
        {
            n_swaps = n_cycles;
        }
        else
        {
            n_swaps = n_cycles + 1;
        }
    }
    return n_swaps;
}

bool Level::sub_sum_exists(u_t target, u_t top_idx, u_t max_summands)
{
    bool exists = false;
    if ((max_summands > 0) && (target <= flat_cycle_sizes_pfx_sum[top_idx + 1]))
    {
        const u_t top = flat_cycle_sizes[top_idx];
        if (top == target)
        {
            exists = true;
        }
        else
        {
             --top_idx;
             // selecting top
             exists = sub_sum_exists(target - top, top_idx, max_summands - 1);
             // skipping top
             exists = exists || sub_sum_exists(target, top_idx, max_summands);
        }
    }
    return exists;
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
