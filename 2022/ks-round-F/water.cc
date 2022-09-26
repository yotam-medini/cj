// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <array>
#include <fstream>
#include <iostream>
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

class Water
{
 public:
    Water(istream& fi);
    Water(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    u_t N, Q;
    vau2_t pipes;
    vu_t queries;
    vvu_t cc; // container connections;
    u_t solution;
};

Water::Water(istream& fi) : solution(0)
{
    fi >> N >> Q;
    pipes.reserve(N - 1);
    while (pipes.size() < N - 1)
    {
        au2_t pipe;
        fi >> pipe[0] >> pipe[1];
        pipes.push_back(pipe);
    }
    queries.reserve(Q);
    while (queries.size() < Q)
    {
        u_t q;
        fi >> q;
        queries.push_back(q);
    }
}

void Water::solve_naive()
{
    cc = vvu_t(N + 1, vu_t());
    for (const au2_t& pipe: pipes)
    {
        cc[pipe[0]].push_back(pipe[1]);
        cc[pipe[1]].push_back(pipe[0]);
    }
    u_t water_left = Q;
    vector<bool> filled = vector<bool>(N + 1, false);
    vu_t curr_level;
    curr_level.push_back(1);
    filled[1] = true;
    while ((water_left > 0) && (curr_level.size() <= water_left))
    {
        const u_t level_fill = curr_level.size();
        solution += level_fill;
        water_left -= level_fill;
        vu_t next_level;
        for (u_t c: curr_level)
        {
            for (u_t con: cc[c])
            {
                if (!filled[con])
                {
                    next_level.push_back(con);
                }
            }
        }
        swap(curr_level, next_level);
    }
}

void Water::solve()
{
    solve_naive();
}

void Water::print_solution(ostream &fo) const
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

    void (Water::*psolve)() =
        (naive ? &Water::solve_naive : &Water::solve);
    if (solve_ver == 1) { psolve = &Water::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Water water(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (water.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        water.print_solution(fout);
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

static int test_case(int argc, char ** argv)
{
    int rc = rand_range(0, 1);
    ull_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (small)
    {
        Water p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Water p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution="<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        ofstream f("water-fail.in");
        f << "1\n";
        f.close();
    }
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
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
