// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned char uc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
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
typedef vector<Ball> vball_t;

class IOBot
{
 public:
    IOBot(istream& fi);
    IOBot(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
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
        ll_t x; uc_t shape;
        fi >> x >> shape;
        balls.push_back(Ball(x, shape));
    }
}

void IOBot::solve_naive()
{
    solution = numeric_limits<ull_t>::max();    
    uc_t subset_mask = 0;
    for (u_t i = 0; i < N; ++i)
    {
        subset_mask |= (1u << i);
    }
    if (N % 2 == 0)
    {
        solution = compute_subset_solution(subset_mask);
    }
    else
    {
        for (u_t i = 0; i < N; ++i)
        {
            ull_t absx = abs(balls[i].x);
            u_t even_mask = subset_mask ^ (1u << i);
            ull_t even_solution = compute_subset_solution(even_mask);
            ull_t sub_solution = even_solution + 2*absx;
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
    solve_naive();
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
    if (dbg_flags & 0x100) { save_case("iobot-curr.in"); }
    if (small)
    {
        IOBot p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        IOBot p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("iobot-fail.in");
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
