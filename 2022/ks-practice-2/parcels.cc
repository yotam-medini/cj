// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <set>
#include <tuple>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<string> vs_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;

static unsigned dbg_flags;

class Cell
{
 public:
    Cell(u_t d=0, u_t i=0, u_t j=0) : dist(d), ij{i, j} {}
    u_t dist;
    u_t ij[2];
};
bool operator<(const Cell& c0, const Cell& c1)
{
    bool lt = 
        make_tuple(c0.dist, c0.ij[0], c0.ij[1]) <
        make_tuple(c1.dist, c1.ij[0], c1.ij[1]);
    return lt;
}
typedef set<Cell> setcell_t;


class BFS
{
 public:
    BFS(u_t i=0, u_t j=0) : origin{i, j} {}
    void insert(u_t d, u_t i, u_t j)
    {
        q.insert(q.end(), (Cell(d, i, j)));
    }
    u_t origin[2];
    setcell_t q;
};

class Parcels
{
 public:
    Parcels(istream& fi);
    Parcels(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    u_t max_time() const;
    void compute_dists();
    u_t n_offices() const;
    u_t R, C;
    vs_t rows_offices;
    u_t solution;
    vvu_t dists;
};

Parcels::Parcels(istream& fi) : solution(0)
{
    fi >> R >> C;
    rows_offices.reserve(R);
    while (rows_offices.size() < R)
    {
        string s;
        fi >> s;
        rows_offices.push_back(s);
    }
}

void Parcels::solve_naive()
{
    u_t min_max = R + C;
    for (u_t i = 0; i < R; ++i)
    {
        for (u_t j = 0; j < C; ++j)
        {
            char save = rows_offices[i][j];
            rows_offices[i][j] = '1';
            u_t tmax = max_time();
            if (min_max > tmax)
            {
                min_max = tmax;
            }
            rows_offices[i][j] = save;
        }
    }
    solution = min_max;
}

void Parcels::solve()
{
    compute_dists();
}

u_t Parcels::max_time() const
{
    u_t tmax = 0;
    for (u_t i = 0; i < R; ++i)
    {
        for (u_t j = 0; j < C; ++j)
        {
            u_t tmin = R + C;
            for (u_t r = 0; r < R; ++r)
            {
                const u_t dr = (i < r ? r - i : i - r);
                for (u_t c = 0; c < C; ++c)
                {
                    if (rows_offices[r][c] == '1')
                    {
                        const u_t dc = (j < c ? c - j : j - c);
                        u_t t = dr + dc;
                        if (tmin > t)
                        {
                            tmin = t;
                        }
                    }
                }
            }
            if (tmax < tmin)
            {
                tmax = tmin;
            }
        }
    }
    return tmax;
}

void Parcels::compute_dists()
{
    dists = vvu_t(R, vu_t(C, R+C));
    vector<BFS> bfss;  bfss.reserve(n_offices());
    for (u_t i = 0; i < R; ++i)
    {
        const string& row = rows_offices[i];
        for (u_t j = 0; j < C; ++j)
        {
            if (row[j] == '1')
            {
                bfss.push_back(BFS(i, j));
                BFS& bfs = bfss.back();
                bfs.q.insert(bfs.q.end(), Cell(0, i, j));
            }
        }
    }
    while (!bfss.empty())
    {
        for (BFS& bfs: bfss)
        {
            Cell cell = *(bfs.q.begin());
            const u_t d = cell.dist;
            const u_t i = cell.ij[0], j = cell.ij[1];
            bfs.q.erase(bfs.q.begin());
            if (dists[i][j] > d)
            {
                dists[i][j] = d;
                const u_t d1 = d + 1;
                if (i > 0) { bfs.insert(d1, i - 1, j); }
                if (i + 1 < R) { bfs.insert(d1, i + 1, j); }
                if (j > 0) { bfs.insert(d1, i, j - 1); }
                if (j + 1 < C) { bfs.insert(d1, i, j + 1); }
            }
        }
        for (u_t i = 0; i < bfss.size(); )
        {
            if (bfss[i].q.empty())
            {
                bfss[i] = bfss.back();
                bfss.pop_back();
            }
            else
            {
                ++i;
            }
        }
    }
}

u_t Parcels::n_offices() const
{
    u_t n = 0;
    for (const string& s: rows_offices)
    {
        n += count(s.begin(), s.end(), '1');
    }
    return n;
}

void Parcels::print_solution(ostream &fo) const
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

    void (Parcels::*psolve)() =
        (naive ? &Parcels::solve_naive : &Parcels::solve);
    if (solve_ver == 1) { psolve = &Parcels::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Parcels parcel(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (parcel.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        parcel.print_solution(fout);
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
    if (dbg_flags & 0x100) { save_case("parcel-curr.in"); }
    if (small)
    {
        Parcels p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Parcels p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("parcel-fail.in");
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
