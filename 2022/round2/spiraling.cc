// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;

static unsigned dbg_flags;

string strint(u_t n, u_t minwidth=0)
{
    ostringstream os;
    os << n;
    string ret = os.str();
    if (ret.size() < minwidth)
    {
        ret = string(minwidth - ret.size(), ' ') + ret;
    }
    return ret;
}

class Spiraling
{
 public:
    Spiraling(istream& fi);
    Spiraling(u_t _N, u_t _K) : N(_N), K(_K) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    const vau2_t& get_solution() const { return solution; }
 private:
    void build_house();
    void advance(u_t i, u_t j, u_t moves, vau2_t& cuts);
    void pre_advance(u_t curr, u_t i, u_t j, u_t moves, vau2_t& cuts);
    void print_house_solution();
    u_t N, K;
    vau2_t solution;
    vvu_t house;
};

Spiraling::Spiraling(istream& fi)
{
    fi >> N >> K;
}

void Spiraling::solve_naive()
{
    build_house();
    vau2_t cuts;
    advance(0, 0, 0, cuts);
    if (dbg_flags & 0x2) { print_house_solution(); }
    if (dbg_flags & 0x4) { 
        cerr << "N=" << N << ", K=" << K << ", cuts: ";
        for (const au2_t& cut: solution) {
            cerr << ' ' << (cut[1] - cut[0]); } cerr << '\n';
    }
}

void Spiraling::build_house()
{
    house = vvu_t(N, vu_t(N, 0));
    u_t num = 1;
    for (u_t level = 0; 2*level < N; ++level)
    {
        int i = level, j = level;
        for ( ; (j < int(N)) && (house[i][j] == 0); ++j, ++num) // >
        {
             house[i][j] = num;
        }
        for (++i, --j; (i < int(N)) && (house[i][j] == 0); ++i, ++num) // V
        { 
            house[i][j] = num;
        }
        for (--i, --j; (j >= 0) && (house[i][j] == 0);
             --j, ++num) // <
        {
            house[i][j] = num;
        }
        for (--i, ++j; house[i][j] == 0; --i, ++num) // ^
        {
            house[i][j] = num; 
        }
    }
    if (dbg_flags & 0x1) {
        cerr << "house:\n";
        for (const vu_t& row: house) {
            for (u_t c: row) { cerr << (c < 10 ? "  " : " ") << c; }
            cerr << '\n';
        }
    }
}

void Spiraling::advance(u_t i, u_t j, u_t moves, vau2_t& cuts)
{
    if (solution.empty())
    {
        const u_t curr = house[i][j];
        const u_t mid = N/2;
        if ((i == mid) && (j == mid))
        {
            if (moves == K)
            {
                solution = cuts;
            }
        }
        else if ((curr < N*N) && (moves < K))
        {
            if ((j + 1 < N) && (curr < house[i][j + 1]))
            {
                 pre_advance(curr, i, j + 1, moves, cuts);
            }
            if ((i + 1 < N) && (curr < house[i + 1][j]))
            {
                 pre_advance(curr, i + 1, j, moves, cuts);
            }
            if ((j > 0) && (curr < house[i][j - 1]))
            {
                 pre_advance(curr, i, j - 1, moves, cuts);
            }
            if ((i > 0) && (curr < house[i - 1][j]))
            {
                 pre_advance(curr, i - 1, j, moves, cuts);
            }
        }
    }
}


void Spiraling::pre_advance(u_t curr, u_t i, u_t j, u_t moves, vau2_t& cuts)
{
    const u_t next = house[i][j];
    if (solution.empty() && (curr + 1 <= next))
    {
        if (curr + 1 == next)
        {
            advance(i, j, moves + 1, cuts);
        }
        else if (cuts.size() < (N*N - 1) - K)
        {
            cuts.push_back(au2_t{curr, next});
            advance(i, j, moves + 1, cuts);
            cuts.pop_back();
        }
    }
}

void Spiraling::print_house_solution()
{
    cerr << "house solution: N="<<N << ", K="<<K;
    if (solution.size() == 0) {
       cerr << " None\n";
    } else {
        cerr << '\n';
        const u_t nw = strint(N*N).size();
        for (const vu_t& row: house) {
            for (u_t n: row) {
                char c_cut = ' ';
                for (const au2_t& cut: solution) {
                    if (cut[0] == n) { c_cut = '*'; }
                }
                cerr << strint(n, nw) << c_cut << ' ';
            }
            cerr << '\n';
        }
    }
}

void Spiraling::solve()
{
    const u_t N2 = N*N;
    u_t skip = (N2 - 1) - K;
    if ((K >= N - 1) && (skip % 2 == 0))
    {
        u_t ring = N/2;
        while (skip > 0)
        {
            const u_t max_skip = 8*ring - 2;
            const u_t s = min(skip,  max_skip);
            const u_t cut_ring = s/8 + 1;
            const u_t side = 2*cut_ring + 1;
            const u_t side2 = side*side;
            const u_t ring_start = (N2 - side2) + 2;
            const u_t qside = 3 - (s % 8)/2;
            const u_t cut_from = ring_start + qside*(side - 1);
            const au2_t cut{cut_from, cut_from + s + 1};
            solution.push_back(cut);
            skip -= s;
            ring = cut_ring - 1;
        }
        if (dbg_flags & 0x2) {
            build_house();
            print_house_solution();
        }
    }
}

void Spiraling::print_solution(ostream &fo) const
{
    if (solution.empty())
    {
        fo << ' ' << "IMPOSSIBLE";
    }
    else
    {
        fo << ' ' << solution.size();
        for (const au2_t& cut: solution)
        {
            fo << '\n' << cut[0] << ' ' << cut[1];
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

    void (Spiraling::*psolve)() =
        (naive ? &Spiraling::solve_naive : &Spiraling::solve);
    if (solve_ver == 1) { psolve = &Spiraling::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Spiraling spiraling(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (spiraling.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        spiraling.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static void save_case(const char* fn, u_t N, u_t K)
{
    ofstream f(fn);
    f << "1\n" << N << ' ' << K << '\n';
    f.close();
}

static u_t solution_skip(const vau2_t cuts)
{
    u_t skip = 0;
    for (const au2_t cut: cuts)
    {
        skip += (cut[1] - 1 - cut[0]);
    }
    return skip;
}

static int test_case(u_t N, u_t K)
{
    int rc = 0;
    vau2_t solution, solution_naive;
    const u_t skip_expected = N*N - 1 - K;
    u_t skip = 0, skip_naive = 0;
    bool small = N < 0x10;
    if (dbg_flags & 0x100) { save_case("spiraling-curr.in", N, K); }
    if (small)
    {
        Spiraling p(N, K);
        p.solve_naive();
        solution_naive = p.get_solution();
        skip_naive = solution_skip(solution_naive);
    }
    {
        if (dbg_flags & 0x200)
        {
            solution = solution_naive;
        }
        else
        {
            Spiraling p(N, K);
            p.solve();
            solution = p.get_solution();
            skip = solution_skip(solution);
        }
    }
    if (small)
    {
        if (solution.empty() != solution_naive.empty()) {
            cerr << "solutions not consistent empty\n";
        }
        else if (!solution.empty())
        {
            if ((skip != skip_naive) || (skip != skip_expected))
            {
                rc = 1;
                cerr << "Failed: skip_expected=" << skip_expected <<
                    ", skip = " << skip << " != " << skip_naive <<
                    " = skip_naive\n";
                save_case("spiraling-fail.in", N, K);
            }
        }
        if (rc != 0)
        {
            save_case("spiraling-fail.in", N, K);
        }
    }
    if (rc == 0) { cerr << " N="<<N << ", K="<<K <<
        (small ? " (small) " : " (large) ") << " --> " <<
        solution.size() << '\n'; }
    return rc;
}

static int test_small(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    if (string(argv[ai]) == string("-debug"))
    {
        dbg_flags = strtoul(argv[ai + 1], nullptr, 0);
        ai += 2;
    }
    const u_t Nmax = strtoul(argv[ai++], nullptr, 0);
    cerr << "Nmax=" << Nmax <<
        '\n';
    for (u_t N = 3, ti = 0; (rc == 0) && (N <= Nmax); N += 2)
    {
        for (u_t K = 1; (rc == 0) && (K < N*N - 1); ++K)
        {
            cerr << "Tested: " << ti << '\n';
            rc = test_case(N, K);
        }
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test_small(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
