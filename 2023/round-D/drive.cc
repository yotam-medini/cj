// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>
#include <cctype>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<string> vs_t;
typedef array<char, 2> ac2_t;
typedef vector<ac2_t> vac2_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;
typedef vector<vau2_t> v_vau2_t;
typedef vector<v_vau2_t> vv_vau2_t;

static unsigned dbg_flags;

class Drive
{
 public:
    Drive(istream& fi);
    Drive(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    void build_adjs();
    void ij_build_adjs(u_t i, u_t j);
    void ij_solve(u_t i, u_t j);
    char Gij_di(const au2_t& uij, u_t dim, u_t dimi) const
    {
        au2_t ij(uij);
        ij[dim] = dimi;
        const char c = G[ij[0]][ij[1]];
        return c;
    }
    u_t R, C;
    vs_t G;
    vac2_t solution;
    vv_vau2_t ij_adjs;
};

Drive::Drive(istream& fi)
{
    fi >> R >> C;
    G.reserve(R);
    while (G.size() < R)
    {
        G.push_back(string());
        fi >> G.back();
    }
}

void Drive::solve_naive()
{
    build_adjs();
    for (u_t i = 0; i < R; ++i)
    {
        for (u_t j = 0; j < C; ++j)
        {
            const char c = G[i][j];
            if (islower(c))
            {
                ij_solve(i, j);
            }
        }
    }
    sort(solution.begin(), solution.end());
}

void Drive::build_adjs()
{
    ij_adjs.assign(R, v_vau2_t());
    for (u_t i = 0; i < R; ++i)
    {
         ij_adjs[i].assign(C, vau2_t());
    }
    for (u_t i = 0; i < R; ++i)
    {
        for (u_t j = 0; j < C; ++j)
        {
            const char c = G[i][j];
            if (islower(c) || (c == '.'))
            {
                ij_build_adjs(i, j);
            }
        }
    }
}

void Drive::ij_build_adjs(u_t i, u_t j)
{
    vau2_t& adjs = ij_adjs[i][j];
    const au2_t ij{i, j};
    for (u_t dim: {0, 1})
    {
        const u_t L = (dim == 0 ? R : C);
        const u_t z0 = ij[dim];
        bool wall_low = (z0 == 0);
        bool wall_high = (z0 == L- 1);
        au2_t adj(ij);
        int z;

        if (z0 > 0)
        {
            for (z = z0 - 1; (z >= 0) && (Gij_di(ij, dim, z) == '.'); --z) {}
            wall_low = (z < 0) || (Gij_di(ij, dim, z) == '#');
        }
        if (z0 < L - 1)
        {
            for (z = z0 + 1; (z < int(L)) && (Gij_di(ij, dim, z) == '.'); ++z)
            { ; }
            wall_high = (z == int(L)) || (Gij_di(ij, dim, z) == '#');
        }
        if (wall_high)
        {
            for (z = z0 - 1; (z >= 0) && (Gij_di(ij, dim, z) == '.'); --z)
            {
                adj[dim] = z;
                adjs.push_back(adj);
            }
            if ((z >= 0) && isupper(Gij_di(ij, dim, z)))
            {
                adj[dim] = z;
                adjs.push_back(adj);
            }
       }
       if (wall_low)
       {
            for (z = z0 + 1; (z < int(L)) && (Gij_di(ij, dim, z) == '.'); ++z)
            {
                adj[dim] = z;
                adjs.push_back(adj);
            }
            if ((z < int(L)) && isupper(Gij_di(ij, dim, z)))
            {
                adj[dim] = z;
                adjs.push_back(adj);
            }
        }
    }
}

void Drive::ij_solve(u_t i, u_t j)
{
    vector<vector<bool>> visited(R, vector<bool>(C, false));
    deque<au2_t> q;
    const char a = G[i][j];
    q.push_back(au2_t{i, j});
    visited[i][j] = true;
    while (!q.empty())
    {
        const au2_t curr = q.front();
        q.pop_front();
        for (const au2_t& adj: ij_adjs[curr[0]][curr[1]])
        {
            if (!visited[adj[0]][adj[1]])
            {
                visited[adj[0]][adj[1]] = true;
                const char c = G[adj[0]][adj[1]];
                if (isupper(c))
                {
                    solution.push_back(ac2_t{a, c});
                }
                else
                {
                    q.push_back(adj);
                }
            }
        }
    }
}

void Drive::solve()
{
    solve_naive();
}

void Drive::print_solution(ostream &fo) const
{
    for (const ac2_t& aA: solution)
    {
        fo << ' ' << aA[0] << aA[1];
    }
    if (solution.empty())
    {
        fo << " NONE";
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

    void (Drive::*psolve)() =
        (naive ? &Drive::solve_naive : &Drive::solve);
    if (solve_ver == 1) { psolve = &Drive::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Drive drive(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
           fpos_last = fpos;
        }

        (drive.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        drive.print_solution(fout);
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
    if (dbg_flags & 0x100) { save_case("drive-curr.in"); }
    if (small)
    {
        Drive p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Drive p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("drive-fail.in");
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
