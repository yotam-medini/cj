// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <array>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;
typedef pair<int, int> ii_t;
typedef vector<ii_t> vii_t;
typedef array<vii_t, 4> a4vii_t;
typedef set<ii_t> setii_t;
typedef map<ii_t, a4vii_t> ii2a4vii_t;

static unsigned dbg_flags;

static bool is_good_jump(const ii_t &cell0, const ii_t &cell1)
{
    int x0 = cell0.first;
    int y0 = cell0.second;
    int x1 = cell1.first;
    int y1 = cell1.second;
    bool ok = (x0 != x1) && (y0 != y1);
    ok = ok && ((x0 + y0) != (x1 + y1));
    ok = ok && ((x0 - y0) != (x1 - y1));
    return ok;
}

class PylonSmall
{
 public:
    PylonSmall(int vr=0, int vc=0) : r(vr), c(vc) {}
    void solve_naive();
    void get_solution(vii_t &s) const { s = solution; }
 private:
    int r, c;
    vii_t solution;
    vii_t candidate;
    setii_t pending;
    bool advance();
};

void PylonSmall::solve_naive()
{
    for (int x = 0; x < c; ++x)
    {
        for (int y = 0; y < r; ++y)
        {
            pending.insert(pending.end(), (ii_t(y, x)));
        }
    }
    advance();    
}

bool PylonSmall::advance()
{
    bool found = pending.empty();
    //   !((r == 4) && (c == 4) && (candidate[0] == ii_t(0, 0)));
    if (found)
    {
        solution = candidate;
    }

    const ii_t curr = (candidate.empty()
        ? ii_t(-1, r + c + 1) : candidate.back());
    vii_t lpending(pending.begin(), pending.end());
    if ((r == 4) && (c == 4) && (lpending.size() == 16))
    {
        swap(lpending[0], lpending[1]);
    }
    for (vii_t::const_iterator i = lpending.begin(), e = lpending.end(); 
        solution.empty() && (i != e); ++i)
    {
        const ii_t cell = *i;
        bool good_jump = is_good_jump(curr, cell);
        if (good_jump)
        {
            candidate.push_back(cell);
            pending.erase(setii_t::value_type(cell));
            advance();
            pending.insert(pending.end(), cell);
            candidate.pop_back();
        }
    }

    return found;
}

class Pylon
{
 public:
    Pylon(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    static void init();
    void build();
    void build_rows(int row, int n_rows);
    void add_block(int row, int col, int n_rows, int n_cols);
    static ii2a4vii_t small_solutions;
    int r, c;
    vii_t solution;
};
ii2a4vii_t Pylon::small_solutions;

Pylon::Pylon(istream& fi)
{
    fi >> r >> c;
}

void Pylon::init()
{
    vii_t sol0;
    a4vii_t sols4;
    for (int m = 2; m <= 4; ++m)
    {
        int nend = (m == 2 ? 9 : (m == 3 ? 7 : 4));
        for (int n = m; n <= nend; ++n)
        {
            PylonSmall ps(m, n);
            ps.solve_naive();
            ps.get_solution(sol0);
            for (int bx = 0, si = 0; bx < 2; ++bx)
            {
                for (int by = 0; by < 2; ++by, ++si)
                {
                    vii_t s(sol0);
                    for (ii_t &xy: s)
                    {
                        if (bx)
                        {
                            xy.first = m - xy.first - 1;
                        }
                        if (by)
                        {
                            xy.second = n - xy.second - 1;
                        }
                    }
                    sols4[si] = s;
                }
            }
            ii2a4vii_t::value_type v(ii_t(m, n), sols4);
            small_solutions.insert(small_solutions.end(), v);
        }
    }
}

void Pylon::solve_naive()
{
    PylonSmall ps(r, c);
    ps.solve_naive();
    ps.get_solution(solution);
}

void Pylon::solve()
{
    if (small_solutions.empty())
    {
        init();
    }
    bool flip = false;
    if (r > c)
    {
        flip = true;
        swap(r, c);
    }
    auto w = small_solutions.find(ii_t(r, c));
    if (w != small_solutions.end())
    {
        solution = (*w).second[0];
    }
    else
    {
        build();        
    }
    if (flip)
    {
        swap(r, c);
        for (ii_t &xy: solution)
        {
            swap(xy.first, xy.second);
        }
    }
}

void Pylon::build()
{
    if (r > 1)
    {
        int rows_pending = r;
        int row = 0;
        for (; rows_pending > 3; row +=2, rows_pending -=2)
        {
            build_rows(row, 2);
        }
        // rows_pending in {0, 1, 2}
        if (rows_pending > 0)
        {
            build_rows(row, rows_pending);
        }
    }
}

void Pylon::build_rows(int row, int n_rows)
{
    int col_pending = c;
    int col = 0;
    int pending_limit = 9;
    int col_step = 5;
    if (n_rows == 3)
    {
        pending_limit = 7;
        col_step = 4;
    }
    for (; col_pending > pending_limit; 
        col += col_step, col_pending -= col_step)
    {
         add_block(row, col, n_rows, col_step);
    }
    if (col_pending > 0)
    {
         add_block(row, col, n_rows, col_pending);
    }
}

void Pylon::add_block(int row, int col, int n_rows, int n_cols)
{
    ii2a4vii_t::const_iterator w = small_solutions.find(ii_t(n_rows, n_cols));
    if (w == small_solutions.end())
    {
        cerr << "No small solution row="<<row << ", col="<<col << "\n";
        exit(1);
    }
    const a4vii_t &sol4 = (*w).second;
    int si = 0;
    if (!solution.empty())
    {
        ii_t last_cell(solution.back()), candid;
        si = -1;
        do
        {
            ++si;
            const ii_t &c0 = sol4[si][0];
            candid = ii_t(row + c0.first, col + c0.second);
        } while ((si < 4) && !is_good_jump(last_cell, candid));

        if (si == 4)
        {
            cerr << "No good jump row="<<row << ", col="<<col << "\n";
            exit(1);
        }
    }
    const vii_t &block = sol4[si];
    for (const ii_t &bcell: block)
    {
        ii_t cell(row + bcell.first, col + bcell.second);
        solution.push_back(cell);
    }
}

void Pylon::print_solution(ostream &fo) const
{
    if (solution.empty())
    {
        fo << " IMPOSSIBLE";
    }
    else
    {
        fo << " POSSIBLE";
        for (const ii_t cell: solution)
        {
            fo << "\n" << cell.first + 1 << " " << cell.second + 1;
        }
    }
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
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

    void (Pylon::*psolve)() =
        (naive ? &Pylon::solve_naive : &Pylon::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Pylon pylon(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (pylon.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        pylon.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
