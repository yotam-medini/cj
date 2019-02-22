// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>
#include <cstring>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned u_t;
typedef pair<u_t, u_t> uu_t;
typedef vector<uu_t> vuu_t;
typedef unsigned long ul_t;
// typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class BaseMatrix
{
  public:
    BaseMatrix(unsigned _m, unsigned _n) : m(_m), n(_n) {}
    const unsigned m; // rows
    const unsigned n; // columns
  protected:
    unsigned rc2i(unsigned r, unsigned c) const 
    {
        unsigned ret = r*n + c;
        return ret;
    }
    void i2rc(unsigned &r, unsigned &c, unsigned i) const 
    {
        r = i / n;
        c = i % n;
    }
};

template <class T>
class Matrix : public BaseMatrix
{
  public:
    Matrix(unsigned _m=1, unsigned _n=1) :
        BaseMatrix(_m, _n), _a(new T[m *n]) {}
    virtual ~Matrix() { delete [] _a; }
    const T& get(unsigned r, unsigned c) const { return _a[rc2i(r, c)]; }
    void put(unsigned r, unsigned c, const T &v) const { _a[rc2i(r, c)] = v; }
  private:
    T *_a;
};

class Cell
{
 public:
   Cell() :
       board_size(1),
       lrbt{0,0,0,0},
       color(0)
   {}
   u_t board_size;
   u_t lrbt[4]; // consitent rays
   unsigned color: 2;
};

typedef Matrix<Cell> mtxcell_t;
  
class ChessBoard
{
 public:
    ChessBoard(istream& fi);
    ~ChessBoard() { delete mtxcell; }
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t cell_board_size(u_t i, u_t j) const;
    void print_board(ostream& fo=cerr) const;
    u_t find_max_board(u_t &xi, u_t &xj) const;
    void sub_board_clear(u_t xi, u_t xj, u_t size);
    mtxcell_t *mtxcell;
    vuu_t solution;
};

ChessBoard::ChessBoard(istream& fi) : mtxcell(0)
{
    Cell cell;
    u_t m, n;
    fi >> m >> n;
    mtxcell = new mtxcell_t(m, n);
    for (u_t row = 0; row < m; ++row)
    {
        string s;
        fi >> s;
        for (u_t si = 0, col = 0; col < n; ++si, col += 4)
        {
            static const char *cshex = "0123456789ABCDEF";
            char c = s[si];
            u_t uhex = strchr(cshex, c) - cshex;
            for (u_t csi = 0; csi < 4; ++csi)
            {
                cell.color = (uhex & (1u << csi)) != 0;
                mtxcell->put(row, col + (3 - csi), cell);
            }
        }
    }
    if (dbg_flags & 0x1) { print_board(); }
}

void ChessBoard::print_board(ostream &fo) const
{
    fo << "{\n";
    for (u_t row = 0; row < mtxcell->m; ++row)
    {
        for (u_t col = 0; col < mtxcell->n; ++col)
        {
            const Cell &cell = mtxcell->get(row, col);
            fo << " *-?"[cell.color];
        }
        fo << '\n';
    }
    fo << "}\n";
}

void ChessBoard::solve_naive()
{
    u_t pending = mtxcell->m * mtxcell->n;
    while (pending > 0)
    {
        u_t i, j, size;
        size = find_max_board(i, j);
        if ((solution.empty()) || (solution.back().first > size))
        {
            solution.push_back(uu_t(size, 1));
        }
        else
        {
            ++(solution.back().second);
        }
        pending -= (size * size);
        sub_board_clear(i, j, size);
    }
}

void ChessBoard::solve()
{
    solve_naive();
}

void ChessBoard::print_solution(ostream &fo) const
{
    fo << ' ' << solution.size();
    for (u_t i = 0; i < solution.size(); ++i)
    {
        const uu_t &size_count = solution[i];
        fo << '\n' << size_count.first << ' ' << size_count.second;
    }
}

u_t ChessBoard::cell_board_size(u_t i, u_t j) const
{
    u_t size = 0;
    const mtxcell_t &m = *mtxcell;
    u_t color = m.get(i, j).color;
    if (color < 2)
    {
        size = 1;
        bool agree = true;
        unsigned asize = 1;
        while (agree && (i + asize < m.m) && (j + asize < m.n))
        {
            u_t expect = (asize % 2 == 0 ? color : 1 - color);
            for (u_t k = 0; agree && (k <= asize); ++k)
            {
                u_t color_row = m.get(i + asize, j + k).color;
                u_t color_col = m.get(i + k, j + asize).color;
                agree = (color_row == expect) && (color_col == expect);
                expect = 1 - expect;
            }
            if (agree)
            {
                size = ++asize;
            }
        }
    }
    return size;
}

u_t ChessBoard::find_max_board(u_t &xi, u_t &xj) const
{
    const mtxcell_t &m = *mtxcell;
    u_t size = 0;
    for (u_t r = 0; r < m.m; ++r)
    {
        for (u_t c = 0; c < m.n; ++c)
        {
            u_t subsize = cell_board_size(r, c);
            if (size < subsize)
            {
                size = subsize;
                xi = r;
                xj = c;
            }
        }
    }
    return size;
}

void ChessBoard::sub_board_clear(u_t i, u_t j, u_t size)
{
    mtxcell_t &m = *mtxcell;
    for (u_t di = 0; di < size; ++di)
    {
        for (u_t dj = 0; dj < size; ++dj)
        {
            Cell cell = m.get(i + di, j + dj);
            cell.color = 2;
            m.put(i + di, j + dj, cell);
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

    void (ChessBoard::*psolve)() =
        (naive ? &ChessBoard::solve_naive : &ChessBoard::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        ChessBoard chessBoard(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (chessBoard.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        chessBoard.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
