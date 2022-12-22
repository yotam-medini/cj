// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <deque>
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
typedef array<u_t, 3> au3_t;
typedef vector<au3_t> vau3_t;

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
    Parcels(const vs_t& rows) :
       R(rows.size()), C(rows[0].size()), rows_offices(rows), solution(0) {};
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    u_t get_solution() const { return solution; }
 private:
    u_t max_time() const;
    void compute_dists();
    void compute_alt_dists(u_t i, u_t j);
    u_t n_offices() const;
    u_t R, C;
    vs_t rows_offices;
    u_t solution;
    vvu_t dists;
    vvu_t alt_dists;
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
    u_t ibest = R, jbest = C;
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
                ibest = i; jbest = j;
            }
            rows_offices[i][j] = save;
        }
    }
    solution = min_max;
    if (dbg_flags & 0x1) {
        cerr << "best @ (" << ibest << ", " << jbest << ")\n"; }
}

void Parcels::solve()
{
    u_t ibest = R, jbest = C;
    compute_dists();
    vau3_t dijs; dijs.reserve(R*C);
    for (u_t i = 0; i < R; ++i)
    {
        for (u_t j = 0; j < C; ++j)
        {
            dijs.push_back(au3_t{dists[i][j], i, j});
        }
    }
    sort(dijs.begin(), dijs.end());
    const au3_t& dij_max = dijs.back();
    const int dmax = dij_max[0], imax = dij_max[1], jmax = dij_max[2];
    const int dmax_half = (dmax + 1)/2;
    if (dbg_flags & 0x1) {
        cerr << "dmax="<<dmax << " @ ("<<imax << ", " << jmax << ")\n"; }
    const int iend = min<int>(R - 1, imax + dmax_half);

    u_t min_max = R + C;
    for (int i = max(0, imax - dmax_half); i <= iend; ++i)
    {
        const int di = (imax < i ? i - imax : imax - i);
        const int dj_max = dmax_half - di;
        const int jend = min<int>(C - 1, jmax + dj_max);
        for (int j = max(0, jmax - dj_max); j <= jend; ++j)
        {
            compute_alt_dists(i, j);
            u_t tmax = 0;
            for (int k = R*C - 1; k >= 1; --k)
            {
                const au3_t& dij = dijs[k];
                const u_t dold = dij[0], iold = dij[1], jold = dij[2];
                const u_t altd = alt_dists[iold][jold];
                tmax = max(tmax, altd);
                if (altd == dold) // end of improvement
                {
                    k = -1;
                }
            }
            if (min_max > tmax)
            {
                min_max = tmax;
                ibest = i; jbest = j;
            }
        }
    }
#if 0
    for (u_t i = 0; i < R; ++i)
    {
        for (u_t j = 0; j < C; ++j)
        {
            compute_alt_dists(i, j);
            u_t tmax = 0;
            for (int k = R*C - 1; k >= 1; --k)
            {
                const au3_t& dij = dijs[k];
                const u_t dold = dij[0], iold = dij[1], jold = dij[2];
                const u_t altd = alt_dists[iold][jold];
                tmax = max(tmax, altd);
                if (altd == dold) // end of improvement
                {
                    k = -1;
                }
            }
            min_max = min(min_max, tmax);
        }
    }
#endif
    solution = min_max;
    if (dbg_flags & 0x1) {
        cerr << "best @ (" << ibest << ", " << jbest << ")\n"; }
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

void Parcels::compute_alt_dists(u_t ai, u_t aj)
{
    alt_dists = dists;
    deque<Cell> q;
    q.push_back(Cell(0, ai, aj));    
    while (!q.empty())
    {
        Cell cell = q.front();
        const u_t d = cell.dist;
        const u_t i = cell.ij[0], j = cell.ij[1];
        q.pop_front();
        if (alt_dists[i][j] > d)
        {
            alt_dists[i][j] = d;
            const u_t d1 = d + 1;
            if (i > 0) { q.push_back(Cell(d1, i - 1, j)); }
            if (i + 1 < R) { q.push_back(Cell(d1, i + 1, j)); }
            if (j > 0) { q.push_back(Cell(d1, i, j - 1)); }
            if (j + 1 < C) { q.push_back(Cell(d1, i, j + 1)); }
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

static void save_case(const char* fn, const vs_t& rows)
{
    const u_t R = rows.size(), C = rows[0].size();
    ofstream f(fn);
    f << "1\n" << R << ' ' << C << '\n';
    for (const string& row: rows)
    {
        f << row << '\n';
    }
    f.close();
}

static int test_case(const vs_t& rows)
{
    int rc = 0;
    const u_t R = rows.size(), C = rows[0].size();
    u_t solution(-1), solution_naive(-1);
    bool small = (R <= 10) && (C <= 10);
    if (dbg_flags & 0x100) { save_case("parcels-curr.in", rows); }
    if (small)
    {
        Parcels p(rows);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Parcels p(rows);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("parcels-fail.in", rows);
    }
    if (rc == 0) { cerr << "  ..." <<
        (small ? " (small) " : " (large) ") << R << 'x' << C << " --> " <<
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
    const u_t Omin = strtoul(argv[ai++], nullptr, 0);
    const u_t Omax = strtoul(argv[ai++], nullptr, 0);
    cerr << "n_tests=" << n_tests <<
        ", Rmin=" << Rmin << ", Rmax=" << Rmax <<
        ", Cmin=" << Cmin << ", Cmax=" << Cmax <<
        ", Omin=" << Omin << ", Omax=" << Omax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        const u_t R = rand_range(Rmin, Rmax);
        const u_t C = rand_range(Cmin, Cmax);
        const u_t RC = R*C;
        const u_t O = rand_range(Omin, min(Omax, RC));
        vector<bool> offices(RC, false);
        fill_n(offices.begin(), O, true);
        vs_t rows; rows.reserve(R);
        while (rows.size() < R)
        {
            string row;
            while (row.size() < C)
            {
                u_t i = rand() % offices.size();
                row.push_back(offices[i] ? '1' : '0');
                offices[i] = offices.back();
                offices.pop_back();
            }
            rows.push_back(row);
        }
        rc = test_case(rows);
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
