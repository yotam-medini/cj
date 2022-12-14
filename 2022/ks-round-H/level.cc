// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
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
typedef vector<vu_t> vvu_t;
typedef vector<vvu_t> vvvu_t;
typedef vector<bool> vb_t;
typedef unordered_map<u_t, u_t> u2u_t;

static unsigned dbg_flags;

class Level
{
 public:
    Level(istream& fi);
    Level(const vu_t& perm);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    const vu_t& get_solution() const { return solution; }
 private:
    void set_cycles();
    void set_flat_cycles_sizes();
    u_t gen_level(u_t level);
    void print_cycles() const;
    bool sub_sum_exists(u_t target, u_t top_idx, u_t max_summands);
    void improve(const vu_t& distinct_cycle_sizes, const vu_t& cnt);
    u_t optinal_swaps(
        const vu_t& pre_dp, 
        const deque<u_t>& dq,
        u_t j,
        u_t sz) const;
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

Level::Level(const vu_t& perm) :
    N(perm.size()),
    _P(perm),
    cycle_max_size(0)
{
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
    // u_t top_flat_index = flat_cycle_sizes_pfx_sum.size() - 1;
    for (u_t sz = cycle_max_size; full_consume && (pending > 0); --sz)
    {
        full_consume = pending >= sz * sz_cycles[sz].size();
        u_t q = pending / sz;
        u_t nc = min<u_t>(q, sz_cycles[sz].size());
        pending -= nc * sz;
        n_cycles += nc;
        // top_flat_index -= nc;
    }
    u_t n_swaps = n_cycles - 1;
    if  (pending > 0)
    {
        if (sub_sum_exists(level, flat_cycle_sizes.size() - 1, n_cycles + 1))
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

void Level::solve()
{
    set_cycles();
    vu_t distinct_cycle_sizes, cnt;
    for (u_t sz = 1; sz <= N; ++sz)
    {
        if (!sz_cycles[sz].empty())
        {
            distinct_cycle_sizes.push_back(sz);
            cnt.push_back(sz_cycles[sz].size());
        }
    }

    // greedy
    for (u_t target_sz = 1; target_sz <= N; ++target_sz)
    {
        u_t down_total = 0, take = 0;
        for (u_t i = distinct_cycle_sizes.size() - 1; down_total < target_sz;
            --i)
        {
            const u_t sz = distinct_cycle_sizes[i];
            const u_t need = target_sz - down_total;
            const u_t q = (need + (sz - 1)) / sz;
            const u_t itake = min(q, cnt[i]);
            down_total += itake * sz;
            take += itake;
        }
        const u_t swaps = (down_total == target_sz ? take - 1 : take);
        solution.push_back(swaps);
    }

    improve(distinct_cycle_sizes, cnt);
}

static void min_by(u_t& v, u_t x)
{
    if (v > x)
    {
        v = x;
    }
}

void Level::improve(const vu_t& distinct_cycle_sizes, const vu_t& cnt)
{
    const u_t n_distinct = distinct_cycle_sizes.size();
    vvu_t dp(n_distinct + 1, vu_t(N + 1, N));
    for (u_t i = 0; i <= n_distinct; ++i) { dp[i][0] = 0; }
    // dp[0] = vu_t(N + 1, N);
    for (u_t i = 1; i <= n_distinct; ++i)
    {
        const u_t sz = distinct_cycle_sizes[i - 1];
        const u_t sz_total = sz * cnt[i - 1];
        const vu_t& dp_pre = dp[i - 1];
        dp[i][sz] = 1;
        for (u_t jmod = 0; jmod < sz; ++jmod)
        {
            deque<u_t> dq;
            for (u_t j = jmod; j < N; j += sz)
            {
                min_by(dp[i][j], dp_pre[j]);
                // u_t opt = 0;
                if ((j > 0) && !dq.empty())
                {
                    // opt = optinal_swaps(dp_pre, dq, j, sz);
                    const u_t jf = dq.front();
                    const u_t dj = j - jf;
                    const u_t v = dj/sz;
                    const u_t dp_pre_jf = dp_pre[jf];
                    // const u_t add = (dp_pre_jf == 0) ? (v == 0 ? 0 : v - 1) : v;
                    const u_t add = v;
                    const u_t opt = dp_pre_jf + add;
                    min_by(dp[i][j], opt);
                }
                while ((!dq.empty()) && (dq.front() + sz_total <= j))
                {
                    dq.pop_front();
                }
                //    (optinal_swaps(dp_pre, dq, dq.back(), sz) >=
                //     optinal_swaps(dp_pre, dq, j, sz) ))
                while ((!dq.empty()) && (dp_pre[j] <= dp_pre[dq.back()]))
                {
                    dq.pop_back();
                }
                dq.push_back(j);
            }
        }
        if (dbg_flags & 0x1) { 
            cerr << "dp[" << i << "] = ";
            const vu_t dpe = dp.back();
            copy(dpe.begin(), dpe.end(), ostream_iterator<u_t>(cerr, " "));
            cerr << '\n'; }
    }
    for (u_t i = 0; i < N; ++i)
    {
        min_by(solution[i], dp[n_distinct][i + 1] - 1);
    }
}

u_t Level::optinal_swaps(
    const vu_t& pre_dp, 
    const deque<u_t>& dq,
    u_t j,
    u_t sz) const
{
    const u_t j0 = dq.front();
    const u_t v = (j - j0)/sz;
    const u_t pre_dp_j0 = pre_dp[j0];
    const u_t add = pre_dp_j0 == 0 ? (v == 0 ? 0 : v - 1) : v;
    const u_t opt = pre_dp_j0 + add;
    return opt;
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

static void save_case(const char* fn, const vu_t& perm)
{
    ofstream f(fn);
    f << "1\n" << perm.size() << '\n';
    const char* spc = "";
    for (u_t x: perm) { f << spc << x; spc = " "; }
    f << '\n';
    f.close();
}

static int test_case(const vu_t& perm)
{
    int rc = 0;
    vu_t solution, solution_naive;
    const u_t N = perm.size();
    bool small = (N <= 10);
    if (dbg_flags & 0x100) { save_case("level-curr.in", perm); }
    if (small)
    {
        Level p{perm};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Level p{perm};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = [";
        copy(solution.begin(), solution.end(),
            ostream_iterator<u_t>(cerr, " "));
        cerr << "] != ";
        copy(solution_naive.begin(), solution_naive.end(),
            ostream_iterator<u_t>(cerr, " "));
        cerr << "] = solution_naive\n";
        save_case("level-fail.in", perm);
    }
    if (rc == 0) { cerr << "  ... N=" << N << 
        (small ? " (small) " : " (large) ");
        if (small) {
            copy(perm.begin(), perm.end(),
               ostream_iterator<u_t>(cerr, " "));
            cerr << " --> ";
            copy(solution.begin(), solution.end(),
               ostream_iterator<u_t>(cerr, " "));
        }
        cerr << '\n'; }

    return rc;
}

static int test_permute(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    if (string(argv[ai]) == string("-debug"))
    {
        dbg_flags = strtoul(argv[ai + 1], nullptr, 0);
        ai += 2;
    }
    const u_t Nmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Nmax = strtoul(argv[ai++], nullptr, 0);
    cerr << ", Nmin=" << Nmin << ", Nmax=" << Nmax << '\n';
    for (u_t N = Nmin, ti = 0; (rc == 0) && (N <= Nmax); ++N)
    {
        vu_t perm; 
        while (perm.size() < N) { perm.push_back(perm.size() + 1); }
        for (bool more = true; more && (rc == 0);
            more = next_permutation(perm.begin(), perm.end()), ++ti)
        {
            cerr << "Tested: " << ti << '\n';
            rc = test_case(perm);
        }
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test_permute(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
