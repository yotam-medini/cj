// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned char uc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<int> vi_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;
typedef unordered_map<int, u_t> i_to_u_t;
typedef unordered_map<u_t, ull_t> u_to_ull_t;

static unsigned dbg_flags;

string hexstr(u_t x)
{
    ostringstream os;
    os << hex << x;
    return os.str();
}

class Ball
{
 public:
    Ball(ll_t _x=0, uc_t _shape=0): x(_x), shape(_shape) {}
    tuple<ll_t, uc_t> as_tuple() const { return make_tuple(x, shape); }
    ll_t x;
    uc_t shape;
};
bool operator<(const Ball& b0, const Ball& b1)
{
    bool lt = b0.as_tuple() < b1.as_tuple();
    return lt;
}
typedef vector<Ball> vball_t; // not matched with another 

class PositiveMatcher
{
 public:
    PositiveMatcher(const vball_t& _balls, ull_t _c) :
        balls(_balls), n_balls(_balls.size()), c(_c) {}
    ull_t solve();
 private:
    void set_blocks_start();
    ull_t solve_upto(u_t i);
    ull_t solve_upto_low(u_t i);
    vball_t balls;
    const u_t n_balls;
    const ull_t c;
    vu_t shape_indices[2];
    vi_t index_to_shape_index[2];
    u_t shape_index_above_c[2];
    vi_t blocks_start; // k of the analysis
    vull_t pfx_sum[2];
    vull_t pfx_sum_evens[2];
    vull_t dp;
};

ull_t PositiveMatcher::solve()
{
    sort(balls.begin(), balls.end());
    index_to_shape_index[0].reserve(n_balls);
    index_to_shape_index[1].reserve(n_balls);
    shape_index_above_c[0] = shape_index_above_c[1] = u_t(-1);
    for (u_t i = 0; i < n_balls; ++i)
    {
        const u_t shape = balls[i].shape;
        const u_t other = 1 - shape;
        const vu_t& other_indices = shape_indices[other];
        const int other_index = int(other_indices.size()) - 1;
        index_to_shape_index[shape].push_back(shape_indices[shape].size());
        index_to_shape_index[other].push_back(other_index);
        shape_indices[shape].push_back(i);
        if ((shape_index_above_c[shape] == u_t(-1)) && (balls[i].x >= ll_t(c)))
        {
            shape_index_above_c[shape] = i;
        }
    }
    set_blocks_start();
    for (u_t s: {0, 1})
    {
        pfx_sum[s].reserve(n_balls + 1);
        pfx_sum[s].push_back(0);
        vull_t& pfx_ses = pfx_sum_evens[s];
        pfx_ses.push_back(0);
        bool even = true;
        for (const Ball& ball: balls)
        {
            if (ball.shape == s)
            {
                if (even) { pfx_ses.push_back(pfx_ses.back() + ball.x); }
                even = !even;
            }
        }        
    }
    for (const Ball& ball: balls)
    {
        const u_t shape = ball.shape;
        const u_t other = 1 - shape;
        pfx_sum[shape].push_back(pfx_sum[shape].back() + ball.x);
        pfx_sum[other].push_back(pfx_sum[other].back());
    }
    dp = vull_t(balls.size() + 1, ull_t(-1));

    // avoid deep recursion
    u_t eq_shape = 0;
    for ( ; (eq_shape < n_balls) && (balls[eq_shape].shape == balls[9].shape);
        ++eq_shape) {}
    for (u_t i = eq_shape; i < n_balls; ++i)
    {
        (void)solve_upto(i);
    }

    ull_t seconds = solve_upto(n_balls);
    return seconds;
}

void PositiveMatcher::set_blocks_start()
{
    blocks_start.reserve(n_balls + 1);
    i_to_u_t balance_to_last_index;
    int balance = 0;
    for (ull_t i = 0; i <= n_balls; ++i)
    {
        if (dbg_flags & 0x1) { cerr << " balance["<< i << "]="<<balance; }
        i_to_u_t::iterator iter = balance_to_last_index.find(balance);
        if (iter == balance_to_last_index.end())
        {
            blocks_start.push_back(-1);
            balance_to_last_index.insert(iter, make_pair(balance, i));
        }
        else
        {
            blocks_start.push_back(iter->second);
            iter->second = i;
        }
        balance += (i < n_balls ? (balls[i].shape == 0 ? 1 : -1) : 0);
    }
    if (dbg_flags & 0x1) { cerr << "\n blocks_start:";
        for (u_t i = 0; i <= n_balls; ++i) {
             cerr << " [" << i << "]=" << blocks_start[i]; } cerr << '\n'; }
}

ull_t PositiveMatcher::solve_upto(u_t i)
{
    ull_t seconds = dp[i];
    if (seconds == ull_t(-1))
    {
        seconds = (i == 0 ? 0 : 2*balls[i - 1].x);
        if (i < 2)
        {
            ;
        }
        else if (i == 2)
        {
            if (balls[0].shape == balls[1].shape)
            {
                seconds += min<ull_t>(2*balls[0].x, c);
            }
        }
        else if (balls[i - 1].shape != balls[i - 2].shape)
        {
            seconds += solve_upto(i - 2);
        }
        else
        {
            const ull_t seconds_alt1 = 2*balls[i - 1].x + solve_upto(i - 1);
            const ull_t seconds_alt2 =
                2*balls[i - 1].x + min<ull_t>(2*balls[i - 2].x, c) + 
                solve_upto(i - 2);
            const ull_t seconds_alt = min(seconds_alt1, seconds_alt2);
            const int k = blocks_start[i];
            const u_t shape = balls[i - 1].shape;
            if (k != -1)
            {
                seconds = 2*(pfx_sum[shape][i] - pfx_sum[shape][k]);
                seconds += solve_upto(k);
            }
            else
            {
                seconds = solve_upto_low(i);
            }
            seconds = min(seconds, seconds_alt);
        }
        dp[i] = seconds;
    }
    return seconds;
}

ull_t PositiveMatcher::solve_upto_low(u_t i)
{
    const u_t shape = balls[i - 1].shape;
    int ishape = index_to_shape_index[shape][i - 1];
    int iother = index_to_shape_index[1 - shape][i - 1];
    ull_t seconds = 0, seconds_naive = 0;;
    if (dbg_flags & 0x2) {
        while (iother >= 0)
        {
            seconds += 2*balls[shape_indices[shape][ishape]].x;
            --ishape;
            --iother;
        }
        ishape = shape_indices[shape][ishape];
        while ((ishape >= 1) && (balls[ishape - 1].x > ll_t(c)))
        {
            seconds += 2*balls[ishape].x + c;
            ishape -= 2;
        }
        while (ishape >= 0)
        {
            seconds += 2*balls[ishape].x;
            --ishape;
        }
        seconds_naive = 0;
    }
    u_t ishape_next = ishape - iother;
    ull_t seconds_2shapes = pfx_sum[shape][ishape] -
        pfx_sum[shape][ishape_next];
    seconds = seconds_2shapes;
    if ((dbg_flags & 0x2) && (seconds != seconds_naive)) {
        cerr << "Inconsistency: seconds_naive=" << seconds_naive <<
            ", seconds=" << seconds << '\n';
    }
    return seconds;
}

class IOBot
{
 public:
    IOBot(istream& fi);
    IOBot(const vball_t& _balls, ull_t _C) :
        N(_balls.size()), C(_C), balls(_balls), solution(0)  {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
 private:
    ull_t compute_subset_solution(u_t subset_mask);
    ull_t compute_subset_solution_ij(u_t subset_mask, u_t i, u_t j);
    u_t N;
    ull_t C;
    vball_t balls;
    ull_t solution;
    // naive
    u_to_ull_t subset_solution; // memo
    // non-naive
};

IOBot::IOBot(istream& fi) : solution(0)
{
    fi >> N >> C;
    balls.reserve(N);
    while (balls.size() < N)
    {
        ll_t x; u_t shape;
        fi >> x >> shape;
        balls.push_back(Ball(x, shape));
    }
}

void IOBot::solve_naive()
{
    solution = numeric_limits<ull_t>::max();    
    u_t full_mask = (1u << N) - 1;
    for (u_t singles_mask = 0; singles_mask <= full_mask; ++singles_mask)
    {
        u_t singles_count = 0;
        ull_t singles_seconds = 0;
        for (u_t i = 0; i < N; ++i)
        {
            if ((1u << i) & singles_mask)
            {
                ++singles_count;
                singles_seconds += 2*abs(balls[i].x);
            }
        }
        if ((singles_count % 2) == (N % 2))
        {
            u_t even_mask = full_mask & (~singles_mask);
            ull_t even_solution = compute_subset_solution(even_mask);
            ull_t sub_solution = singles_seconds + even_solution;
            if (solution > sub_solution)
            {
                solution = sub_solution;
            }
        }
    }
}

ull_t IOBot::compute_subset_solution(u_t subset_mask)
{
    auto iter = subset_solution.find(subset_mask);
    ull_t ret = numeric_limits<ull_t>::max();
    if (iter == subset_solution.end())
    {
        if (subset_mask == 0)
        {
            ret = 0;
        }
        else
        {
            for (u_t i = 0; i < N; ++i)
            {
                if (subset_mask & (1u << i))
                {
                    for (u_t j = i + 1; j < N; ++j)
                    {
                        if (subset_mask & (1u << j))
                        {
                            ull_t ij_solution = compute_subset_solution_ij(
                                subset_mask, i, j);
                            if (ret > ij_solution)
                            {
                                ret = ij_solution;
                            }
                        }
                    }
                }
            }
        }
        if (dbg_flags & 0x1) { cerr << "subset_mask="<<hexstr(subset_mask) <<
             ", ret="<<ret<<'\n'; }
        iter = subset_solution.insert(
            iter, u_to_ull_t::value_type{subset_mask, ret});
    }
    ret = iter->second;
    return ret;
}

ull_t IOBot::compute_subset_solution_ij(u_t subset_mask, u_t i, u_t j)
{
    const Ball iball = balls[i];
    const Ball jball = balls[j];
    ull_t x_price = 0;
    if ((iball.x > 0) == (jball.x > 0))
    {
        x_price = max(abs(iball.x), abs(jball.x));
    }
    else
    {
        x_price = abs(iball.x) + abs(jball.x);
    }
    ull_t c_price = (iball.shape == jball.shape ? C : 0);
    const u_t ij_mask = (1u << i) | (1u << j);
    const u_t subset_mask_drop_ij = subset_mask & ~ij_mask;
    ull_t sub_solution = compute_subset_solution(subset_mask_drop_ij);
    ull_t ret = 2*x_price + c_price + sub_solution;
    return ret;
}

void IOBot::solve()
{
    vball_t positive_balls[2];
    for (const Ball& ball: balls)
    {
        if (ball.x >= 0)
        {
            positive_balls[0].push_back(ball);
        }
        else
        {
            positive_balls[1].push_back(Ball(-ball.x, ball.shape));
        }
    }
    for (const vball_t& pballs: positive_balls)
    {
        if (!pballs.empty())
        {
            solution += PositiveMatcher(pballs, C).solve();
        }
    }
}

void IOBot::print_solution(ostream &fo) const
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

    void (IOBot::*psolve)() =
        (naive ? &IOBot::solve_naive : &IOBot::solve);
    if (solve_ver == 1) { psolve = &IOBot::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        IOBot iobot(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (iobot.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        iobot.print_solution(fout);
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

static void save_case(const char* fn, const vball_t& balls, ull_t C)
{
    ull_t N = balls.size();
    ofstream f(fn);
    f << "1\n" << N << ' ' << C << '\n';
    for (const Ball& ball: balls)
    {
        f << ball.x << ' ' << u_t(ball.shape) << '\n';
    }
    f.close();
}

static int test_case(const vball_t& balls, ull_t C)
{
    int rc = 0;
    ull_t solution(-1), solution_naive(-1);
    const ull_t N = balls.size();
    bool small = (N <= 12);
    if (dbg_flags & 0x100) { save_case("iobot-curr.in", balls, C); }
    if (small)
    {
        IOBot p(balls, C);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        IOBot p(balls, C);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("iobot-fail.in", balls, C);
    }
    if (rc == 0) { cerr << "  ..." <<
        (small ? " (small) " : " (large) ") <<
        "N="<<N << ", C="<<C << " --> " << solution << '\n'; }
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
    const ll_t Xmin = strtol(argv[ai++], nullptr, 0);
    const ll_t Xmax = strtol(argv[ai++], nullptr, 0);
    const ull_t Cmax = strtoul(argv[ai++], nullptr, 0);
    const ull_t dx = Xmax - Xmin;
    cerr << "n_tests=" << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        ", Xmin=" << Xmin << ", Xmax=" << Xmax <<
        ", Cmax=" << Cmax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        const ull_t N = min<ull_t>(rand_range(Nmin, Nmax), Xmax + 1 - Xmin);
        const ull_t C = rand_range(0, Cmax);
        vball_t balls; balls.reserve(N);
        unordered_set<ll_t> xset;
        while (balls.size() < N)
        {
            bool found = false;
            ll_t X;
            while (!found)
            {
                X = Xmin + rand_range(0, dx);
                found = (xset.find(X) == xset.end());
            }
            xset.insert(xset.end(), X);
            uc_t shape = rand() % 2;
            balls.push_back(Ball(X, shape));
        }
        rc = test_case(balls, C);
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
