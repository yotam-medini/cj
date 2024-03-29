// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include <deque>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<string> vs_t;
typedef vector<char> vc_t;
typedef vector<vc_t> vvc_t;

static unsigned dbg_flags;

class Move
{
 public:
    Move(int _i=0, int _j=0, char _step=' '): i(_i), j(_j), step(_step) {}
    int i, j;
    char step;
};

class Hamilton
{
 public:
    Hamilton(istream& fi);
    Hamilton(const vs_t& _B) : R(_B.size()), C(_B[0].size()), B(_B), size(0) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    const string& get_solution() const { return solution; }
 private:
    void b_to_cells();
    void back_trace(string& prefix, int i, int j);
    void set_4cell(int i, int j, const char** s2);
    void fill_solution();
    void print_cells() const;
    int R, C;
    vs_t B;
    string solution;
    vs_t cells;
    u_t size;
};

Hamilton::Hamilton(istream& fi) : size(0)
{
    fi >> R >> C;
    copy_n(istream_iterator<string>(fi), R, back_inserter(B));
}

void Hamilton::solve_naive()
{
    b_to_cells();
    string prefix;
    cells[0][0] = 'H';
    back_trace(prefix, 0, 0);
}

void Hamilton::b_to_cells()
{
    cells.reserve(2*R);
    for (const string& brow: B)
    {
        string crow; crow.reserve(2*C);
        for (char b: brow)
        {
            if (b == '*') { ++size; }
            crow.push_back(b);
            crow.push_back(b);
        }
        cells.push_back(crow);
        cells.push_back(crow);
    }
    size *= 4;
}

void Hamilton::back_trace(string& prefix, int i, int j)
{
    const u_t sz = prefix.size();
    if ((dbg_flags & 0x10) && ((sz & (sz - 1)) == 0)) {
         cerr << "prefix sz=" << sz << ' ' << prefix << '\n'; }
    if (prefix.size() == size && (i == 0) && (j == 0))
    {
        solution = prefix;
    }
    else
    {
        for (u_t ci = 0; (solution.empty()) && (ci < 4); ++ci)
        {
            char cstep = "SENW"[ci];
            int i1 = i, j1 = j;
            switch (cstep)
            {
             case 'S':
                 ++i1;
                 break;
             case 'E':
                 ++j1;
                 break;
             case 'N':
                 --i1;
                 break;
             case 'W':
                 --j1;
                 break;
            }
            if (((0 <= i1) && (i1 < 2*R) && (0 <= j1) && (j1 < 2*C) &&
                (cells[i1][j1] == '*')) ||
                ((i1 == 0) && (j1 == 0) && (prefix.size() + 1 == size)))
            {
                cells[i1][j1] = 'H';
                prefix.push_back(cstep);
                back_trace(prefix, i1, j1);
                prefix.pop_back();
                cells[i1][j1] = '*';
            }
        }
    }
}

void Hamilton::solve()
{
    static const char* SENW[2] {
        "SW",
        "EN"
    };;
    const int R2 = 2*R, C2 = 2*C;
    b_to_cells();
    set_4cell(0, 0, SENW);
    u_t n_cell_set = 4;

    // BFS
    deque<Move> q;
    if ((2 < R2) && cells[2][0] == '*')
    {
        cells[2][0] = '?';
        q.push_back(Move(0, 0, 'S'));
    }
    if ((2 < C2) && cells[0][2] == '*')
    {
        cells[0][2] = '?';
        q.push_back(Move(0, 0, 'E'));
    }
    while (!q.empty())
    {
        const Move& move = q.front();
        int ito = move.i, jto = move.j;
        const char *from[2], *to[2];
        switch (move.step)
        {
         case 'S':
             ++ito;             
             from[0] = "  "; to[0] = "SN";
             from[1] = "S "; to[1] = "EN";
             break;
         case 'E':
             ++jto;
             from[0] = "  "; to[0] = "WW";
             from[1] = " E"; to[1] = "EN";
             break;
         case 'N':
             --ito;
             from[0] = " N"; to[0] = "SW";
             from[1] = "  "; to[1] = "SN";
             break;
         case 'W':
             --jto;
             from[0] = "W "; to[0] = "SW";
             from[1] = "  "; to[1] = "EE";
             break;
        }
        set_4cell(move.i, move.j, from);
        set_4cell(ito, jto, to);
        q.pop_front();
        n_cell_set += 4;
        for (char step: "SENW")
        {
            int inext = ito, jnext = jto;
            switch (step)
            {
             case 'S':
                ++inext;
                break;
             case 'E':
                ++jnext;
                break;
             case 'N':
                --inext;
                break;
             case 'W':
                --jnext;
                break;
            }
            if ((0 <= inext) && (inext < R) && (0 <= jnext) && (jnext < C))
            {
                 const int inext2 = 2*inext, jnext2 = 2*jnext;
                 if (cells[inext2][jnext2] == '*')
                 {
                     cells[inext2][jnext2] = '?';
                     q.push_back(Move(ito, jto, step));
                 }
            }
        }
    }
    if (dbg_flags & 0x2) { print_cells(); }
    if (n_cell_set == size)
    {
        fill_solution();
    }
}

void Hamilton::set_4cell(int i, int j, const char** s2)
{
    const int i2 = 2*i, j2 = 2*j;
    for (int ri: {0, 1})
    {
        const char* s = s2[ri];
        for (int cj: {0, 1})
        {
            if (s[cj] != ' ')
            {
                cells[i2 + ri][j2 + cj] = s[cj];
            }
        }
    }
}

void Hamilton::fill_solution()
{
    solution.reserve(size);
    int i = 0, j = 0;
    while (solution.size() < size)
    {
        char step = cells[i][j];
        solution.push_back(step);
        switch (step)
        {
         case 'S':
            ++i;
            break;
         case 'E':
            ++j;
            break;
         case 'N':
            --i;
            break;
         case 'W':
            --j;
            break;
        }
    }
}

void Hamilton::print_cells() const
{
    cerr << "{ cells:\n";
    for (const string& row: cells)
    {
        cerr << "  " << row << '\n';
    }
    cerr << "}\n";
}

void Hamilton::print_solution(ostream &fo) const
{
    fo << ' ' << (solution.empty() ? string("IMPOSSIBLE") : solution);
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

    void (Hamilton::*psolve)() =
        (naive ? &Hamilton::solve_naive : &Hamilton::solve);
    if (solve_ver == 1) { psolve = &Hamilton::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Hamilton hamilton(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (hamilton.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        hamilton.print_solution(fout);
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

static void save_case(const char* fn, const vs_t& B)
{
    ofstream f(fn);
    const u_t R = B.size(), C = B[0].size();
    f << "1\n" << R << ' ' << C << '\n';
    for (const string& row: B)
    {
        f << row << '\n';
    }
    f.close();
}

static int test_case(const vs_t& B)
{
    int rc = 0;
    const u_t R = B.size(), C = B[0].size();
    string solution, solution_naive;
    bool small = R*C < 15;
    if (dbg_flags & 0x100) { save_case("hamilton-curr.in", B); }
    if (small)
    {
        Hamilton p(B);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Hamilton p(B);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution.empty() != solution_naive.empty()))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
    }
    if (rc == 0)
    {
        const string::const_iterator sb = solution.begin(), se = solution.end();
        if (count(sb, se, 'S') != count(sb, se, 'N'))
        {
            rc = 1;
            cerr << "#(S)!=#(N) @ " << solution << '\n';
        }
        if (count(sb, se, 'E') != count(sb, se, 'W'))
        {
            rc = 1;
            cerr << "#(E)!=#(W) @ " << solution << '\n';
        }
    }
    if (rc != 0)
    {
        save_case("hamilton-fail.in", B);
    }
    if (rc == 0) { cerr << "  ..." <<
        (small ? " (small) " : " (large) ") << R << ' ' << C <<  " --> " <<
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
    const u_t Rmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Rmax = strtoul(argv[ai++], nullptr, 0);
    const u_t Cmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Cmax = strtoul(argv[ai++], nullptr, 0);
    const u_t Bmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Bmax = strtoul(argv[ai++], nullptr, 0);
    cerr << "n_tests=" << n_tests <<
        ", Rmin=" << Rmin << ", Rmax=" << Rmax <<
        ", Cmin=" << Cmin << ", Cmax=" << Cmax <<
        ", Bmin=" << Bmin << ", Bmax=" << Bmax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t R = rand_range(Rmin, Rmax);
        u_t C = rand_range(Cmin, Cmax);
        u_t n_building = min(rand_range(Bmin, Bmax), R*C - 1);
        vs_t B; B.reserve(R);
        u_t pending_building = n_building;
        u_t  pending_empty = R*C - pending_building;
        while (B.size() < R)
        {
            string row; row.reserve(C);
            while (row.size() < C)
            {
                u_t i = rand() % (pending_building + pending_empty);
                if (B.empty() && row.empty())
                {
                    i = R*C - 1; // force empty 
                }
                char eb = '?';
                if (i < pending_building)
                {
                    eb = '#';
                    --pending_building;
                }
                else
                {
                    eb = '*';
                    --pending_empty;
                }
                row.push_back(eb);
            }
            B.push_back(row);
        }
        rc = test_case(B);
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
