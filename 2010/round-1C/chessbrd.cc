// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <set>
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
          T& get(unsigned r, unsigned c)       { return _a[rc2i(r, c)]; }
    void put(unsigned r, unsigned c, const T &v) const { _a[rc2i(r, c)] = v; }
  private:
    T *_a;
};

class Cell
{
 public:
   Cell() :
       board_size(1),
       rayr(0), rayb(0),
       color(0)
   {}
   u_t board_size;
   u_t rayr, rayb;
   unsigned color: 2;
};

typedef Matrix<Cell> mtxcell_t;

class SubBoard
{
 public:
   SubBoard(u_t vsz=0, u_t vi=0, u_t vj=0) : size(vsz), i(vi), j(vj) {}
   u_t size, i, j;
};

bool operator<(const SubBoard& sb0, const SubBoard& sb1)
{
    bool lt =
        (sb0.size > sb1.size) ||
        ((sb0.size == sb1.size) &&
           ((sb0.i < sb1.i) ||
                ((sb0.i == sb1.i) && (sb0.j < sb1.j))));
    return lt;
}

typedef set<SubBoard> set_subrd_t;

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
    void print_subsize(ostream& fo=cerr) const;
    void print_rb(ostream& fo=cerr) const;
    u_t find_max_board(u_t &xi, u_t &xj) const;
    void sub_board_clear(u_t xi, u_t xj, u_t size);
    void init_sub_boards();
    void init_rays();
    void cell_calc_add_sub_board(u_t row, u_t col);
    void solution_add_size(u_t size, u_t = 1);
    mtxcell_t *mtxcell;
    set_subrd_t sub_boards;
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

void ChessBoard::print_subsize(ostream &fo) const
{
    fo << "{ sub-sizes\n";
    for (u_t row = 0; row < mtxcell->m; ++row)
    {
        for (u_t col = 0; col < mtxcell->n; ++col)
        {
            const Cell &cell = mtxcell->get(row, col);
            fo << ' ' << cell.board_size;
        }
        fo << '\n';
    }
    fo << "}\n";
}

void ChessBoard::print_rb(ostream &fo) const
{
    fo << "{ Right/Bottom\n";
    for (u_t row = 0; row < mtxcell->m; ++row)
    {
        for (u_t col = 0; col < mtxcell->n; ++col)
        {
            const Cell &cell = mtxcell->get(row, col);
            fo << ' ' << cell.rayr << '/' << cell.rayb;
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
        if (dbg_flags & 0x1) { cerr << 
            "Sub: size="<<size<< " @("<<i<<", "<<j<<")\n"; }
        solution_add_size(size);
        pending -= (size * size);
        sub_board_clear(i, j, size);
    }
}

void ChessBoard::solution_add_size(u_t size, u_t n)
{
    if ((solution.empty()) || (solution.back().first > size))
    {
        solution.push_back(uu_t(size, n));
    }
    else
    {
        solution.back().second += n;
    }
}

void ChessBoard::solve()
{
    // solve_naive();
    init_sub_boards();
    mtxcell_t &m = *mtxcell;
    u_t pending = m.m * m.n;
    u_t size = m.m + m.n + 1; // infinity
    while ((pending > 0) && (size > 1))
    {
        const SubBoard best = *sub_boards.begin();
        size = best.size;
        if (dbg_flags & 0x1) { cerr << 
            "Sub: size="<<size<< " @("<<best.i<<", "<<best.j<<")\n"; }
        solution_add_size(size);
        pending -= (size * size);
        // uf size > 1 ...
        // clear
        u_t rb = best.i > size ? best.i - size : 0;
        u_t cb = best.j > size ? best.j - size : 0;
        for (u_t r = rb; r < best.i + size; ++r)
        {
            for (u_t c = cb; c < best.j + size; ++c)
            {
                Cell &cell = m.get(r, c);
                if (cell.board_size > 0)
                {
                    SubBoard sold(cell.board_size, r, c);
                    u_t ndel = sub_boards.erase(sold);
                    if (ndel == 0) {
                        cerr << "software error"; exit(1);
                    }
                }
            }
        }
        for (u_t r = best.i; r < best.i + size; ++r)
        {
            for (u_t c = best.j; c < best.j + size; ++c)
            {
                Cell &cell = m.get(r, c);
                cell.board_size = 0;
                cell.rayr = cell.rayb = 0;
                cell.color = 2; // udnef
            }
        }
        // fix rays
        for (u_t r = best.i; r < best.i + size; ++r)
        {
            if ((best.j > 0) && m.get(r, best.j - 1).rayr > 1)
            {
                for (u_t k = 0; (k <= best.j) && m.get(r, best.j - k).rayr != 1;
                    ++k)
                {
                    m.get(r, best.j - k).rayr = k;
                }
            }
        }
        for (u_t c = best.j; c < best.j + size; ++c)
        {
            if ((best.i > 0) && m.get(best.i - 1, c).rayb > 1)
            {
                for (u_t k = 0; (k <= best.i) && m.get(best.i - k, c).rayb != 1;
                    ++k)
                {
                    m.get(best.i - k, c).rayb = k;
                }
            }
        }
        // recalc some sub-boards
        for (u_t r = best.i + size; r > rb; )
        {
            --r;
            u_t ce = best.j + (r < best.i ? size : 0);
            for (u_t c = cb; c < ce; ++c)
            {
                cell_calc_add_sub_board(r, c);
            }
        }
        if (dbg_flags & 0x4) { print_board(); print_subsize(); }
    }
    if (pending > 0)
    {
        solution_add_size(1, pending);
    }
}

void ChessBoard::init_sub_boards()
{
    init_rays();
    mtxcell_t &m = *mtxcell;
    for (u_t row = m.m; row > 0; )
    {
        --row;
        for (u_t col = 0; col < m.n; ++col)
        {
            cell_calc_add_sub_board(row, col);
        }
    }
    if (dbg_flags & 0x2) { print_rb(); print_subsize(); }
}

void ChessBoard::init_rays()
{
    mtxcell_t &m = *mtxcell;
    for (u_t row = 0; row < m.m; ++row)
    {
        for (u_t col = 0; col < m.n; )
        {
            u_t col_b = col;
            u_t color = m.get(row, col).color;
            u_t expect = color;
            while ((col < m.n) && (m.get(row, col).color == expect))
            {
                expect = 1 - expect;
                ++col;
            }
            u_t col_x = col;
            u_t n_same = 0;
            while (col_x > col_b)
            {
                --col_x;
                Cell &cell = m.get(row, col_x);
                cell.rayr = ++n_same;
            }
        }
    }

    for (u_t col = 0; col < m.n; ++col)
    {
        for (u_t row = 0; row < m.m;)
        {
            u_t row_b = row;
            u_t color = m.get(row, col).color;
            u_t expect = color;
            while ((row < m.m) && (m.get(row, col).color == expect))
            {
                expect = 1 - expect;
                ++row;
            }
            u_t row_x = row;
            u_t n_same = 0;
            while (row_x > row_b)
            {
                --row_x;
                Cell &cell = m.get(row_x, col);
                cell.rayb = ++n_same;
            }
        }
    }
}

void ChessBoard::cell_calc_add_sub_board(u_t row, u_t col)
{
    mtxcell_t &m = *mtxcell;
    Cell &cell = m.get(row, col);
    u_t subsize = 0;
    if ((row + 1 < m.m) && (col + 1 < m.n))
    {
        const Cell &subcell = m.get(row + 1, col + 1);;
        subsize = subcell.color == cell.color ? subcell.board_size : 0;
    }
    cell.board_size = min(subsize + 1, min(cell.rayr, cell.rayb));
    SubBoard sb(cell.board_size, row, col);
    sub_boards.insert(sub_boards.end(), sb);
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
