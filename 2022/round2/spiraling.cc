// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <array>
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
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;

static unsigned dbg_flags;

class Spiraling
{
 public:
    Spiraling(istream& fi);
    Spiraling(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    void build_house();
    void advance(u_t i, u_t j, u_t moves, vau2_t& cuts);
    void pre_advance(u_t curr, u_t i, u_t j, u_t moves, vau2_t& cuts);
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
    if (curr + 1 <= next)
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

void Spiraling::solve()
{
    solve_naive();
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
    if (dbg_flags & 0x100) { save_case("spiraling-curr.in"); }
    if (small)
    {
        Spiraling p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Spiraling p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("spiraling-fail.in");
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
