// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
#include <map>
// #include <vector>
#include <utility>

#include <cstdlib>
#include <array>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<u_t, 2> u2_t;
// typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class BaseWHMatrix
{
  public:
    BaseWHMatrix(unsigned _w, unsigned _h) : w(_w), h(_h) {}
    unsigned w; // width
    unsigned h; // hight
    unsigned size() const { return w*h; }
  protected:
    unsigned xy2i(unsigned x, unsigned y) const 
    {
        unsigned ret = h*x + y;
        return ret;
    }
    void i2rc(unsigned &x, unsigned &y, unsigned i) const 
    {
        x = i / h;
        y = i % h;
    }
};

template <class T>
class WHMatrix : public BaseWHMatrix
{
  public:
    typedef WHMatrix<T> self_t;
    WHMatrix(unsigned _w=0, unsigned _h=0) : 
        BaseWHMatrix(_w, _h), _a(_w*_h > 0 ? new T[_w*_h] : 0) 
    {
        fill_n(_a, _w*_h, T(0));
    }
    virtual ~WHMatrix() 
    { 
        if (_a) 
        { 
            delete [] _a; 
        }
    }
    WHMatrix(const self_t &x) :
        BaseWHMatrix(x.w, x.h), _a(size() > 0 ? new T[size()] : 0) 
    {
        copy(x._a, x._a + size(), _a);
    }
    WHMatrix& operator=(const self_t &rhs)
    {
        if (this != &rhs)
        {
            u_t sz = rhs.w * rhs.h;
            if ((w != rhs.w) || (h != rhs.h))
            {
                w = rhs.w;
                h = rhs.h;
                if (_a) { delete [] _a; }
                _a = (sz > 0 ? new T[sz] : 0);
            }
            copy(rhs._a, rhs._a + sz, _a);
        }
        return *this;
    }
    const T& get(unsigned x, unsigned y) const { return _a[xy2i(x, y)]; }
    const T& get(const u2_t& xy) const { return get(xy[0], xy[1]); }
    void put(unsigned x, unsigned y, const T &v) { _a[xy2i(x, y)] = v; }
    void put(const u2_t& xy, const T &v) { put(xy[0], xy[1], v); }
    const T* raw() const { return _a; }
  private:
    T *_a;
};

typedef enum {Empty, Full, Active} CellState_t;

typedef WHMatrix<CellState_t> matcell_t;
typedef WHMatrix<char> matchar_t;

class State
{
 public:
    State(bool ot, const matcell_t &m) : oturn(ot), mat(m) {}
    bool oturn;
    matcell_t mat;
};

bool operator<(const State &s0, const State &s1)
{
    bool lt = (s0.oturn < s1.oturn);
    if (s0.oturn == s1.oturn)
    {
        const matcell_t m0 = s0.mat;
        const matcell_t m1 = s1.mat;
        const CellState_t *r0 = m0.raw();
        const CellState_t *r1 = m1.raw();
        lt = lexicographical_compare(r0, r0 + m0.size(), r1, r1 + m1.size());
    }
    return lt;
}

typedef map<State, bool> state2bool_t;

class Bacterial
{
 public:
    Bacterial(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool is_win(const State &state, u_t x, u_t y, bool horizontal);
    u_t sub_solve(const matcell_t &mat, const u2_t& bxy, const u2_t& exy);
    u_t sub_solve_xyd(const matcell_t &mat, const u2_t& bxy, const u2_t& exy,
        const u2_t& xy, u_t dim);
    matcell_t mat_initial;
    ul_t solution;
    state2bool_t state2win;
};

Bacterial::Bacterial(istream& fi) : solution(0)
{
    u_t c, r;
    fi >> r >> c;
    mat_initial = matcell_t(c, r);
    string line;
    for (u_t y = r; y > 0; )
    {
        --y;
        fi >> line;
        for (u_t x = 0; x < c; ++x)
        {
            char ch = line[x];
            CellState_t ct = (ch == '.' ? Empty : Active);
            mat_initial.put(x, y, ct);
        }
    }
}

void Bacterial::solve_naive()
{
    matchar_t matwins(mat_initial.w, mat_initial.h);
    State state0(false, mat_initial);
    for (u_t x = 0; x < mat_initial.w; ++x)
    {
        for (u_t y = 0; y < mat_initial.h; ++y)
        {
            CellState_t ct = mat_initial.get(x, y);
            char cwin = '#';
            if (ct == Empty)
            {
                cwin = '.';
                for (u_t b = 0; b != 2; ++b)
                {
                    if (is_win(state0, x, y, b == 0))
                    {
                        ++solution;
                        cwin = (b == 0 ? '-' : (cwin == '.' ? '|' : '+'));
                    }
                }
            }
            matwins.put(x, y, cwin);
        }
    }
    if (dbg_flags & 0x1)
    {
        cerr << '\n';
        for (u_t y = mat_initial.h; y > 0; )
        {
            --y;
            for (u_t x = 0; x < mat_initial.w; ++x)
            {
                cerr << matwins.get(x, y);
            }
            cerr << '\n';
        }
    }
}

bool Bacterial::is_win(const State &state, u_t x, u_t y, bool horizontal)
{
    bool win = false;
    State sub_state(state);
    matcell_t &m = sub_state.mat;
    m.put(x, y, Full);
    bool mutate = false;
    u_t dim = (horizontal ? 0 : 1); // horizontal, vertical
    for (u_t d = 0; (d != 2) && !mutate; ++d)
    {
        u2_t xy = {x, y};
        int step = (d == 0 ? 1 : -1);
        int s_last = (step == 1 ? int(dim == 0 ? m.w : m.h) - 1: 0);
        bool loop = int(xy[dim]) != s_last;
        for (xy[dim] += step; loop; xy[dim] += step)
        {
            CellState_t ct = m.get(xy);
            if (ct == Empty)
            {
                m.put(xy, Full);
                loop = (int(xy[dim]) != s_last);
            }
            else if (ct == Full)
            {
                loop = false;
            }
            else // Active
            {
                mutate = true;
                loop = false;
            }
        }
    }

    if (!mutate)
    {
        bool opponent_win = false;
        auto er = state2win.equal_range(sub_state);
        if (er.first == er.second)
        {
            win = false;
            for (u_t sx = 0; (sx < m.w) && !opponent_win; ++sx)
            {
                for (u_t sy = 0; (sy < m.h) && !opponent_win; ++sy)
                {
                    CellState_t ct = m.get(sx, sy);
                    if (ct == Empty)
                    {
                        for (u_t b = 0; (b != 2) && !opponent_win; ++b)
                        {
                            if (is_win(sub_state, sx, sy, b == 0))
                            {
                                opponent_win = true;
                            }
                        }
                    }
                }
            }
            state2bool_t::iterator iter = er.first;
            state2win.insert(iter,
                state2bool_t::value_type(sub_state, opponent_win));
        }
        else
        {
            opponent_win = (*er.first).second;
        }
        win = !opponent_win;
    }
    return win;
}

void Bacterial::solve()
{
    u2_t bxy({0, 0});
    u2_t exy({mat_initial.w, mat_initial.h});
    solution = sub_solve(mat_initial, bxy, exy);
}

u_t Bacterial::sub_solve(const matcell_t &mat, const u2_t& bxy, const u2_t& exy)
{
    unsigned ret = 0;
    u_t w = exy[0] - bxy[0];
    u_t h = exy[1] - bxy[1];
    if ((w == 1) && (h == 1))
    {
        ret = (mat.get(bxy[0], bxy[1]) == Empty ? 1 : 0);
    }
    else if ((w > 0) && (h > 0))
    {
        u2_t xy;
        for (xy[0] = bxy[0]; xy[0] < exy[0]; ++xy[0])
        {
            for (xy[1] = bxy[1]; xy[1] < exy[1]; ++xy[1])
            {
                CellState_t ct = mat.get(xy);
                if (ct == Empty)
                {
                    u_t hwin = sub_solve_xyd(mat, bxy, exy, xy, 0);
                    u_t vwin = sub_solve_xyd(mat, bxy, exy, xy, 1);
                    ret += hwin + vwin;
                }
            }
        }
    }
    return ret;
}

u_t Bacterial::sub_solve_xyd(const matcell_t &mat,
    const u2_t& bxy, const u2_t& exy, const u2_t& xy0, u_t dim)
{
    u_t ret = 0;
    bool mutate = false;
    for (u_t d = 0; (d != 2) && !mutate; ++d)
    {
        u2_t xy(xy0);
        int step = (d == 0 ? 1 : -1);
        int s_last = (step == 1 ? exy[dim] - 1: bxy[dim]);
        bool loop = int(xy[dim]) != s_last;
        for (xy[dim] += step; loop; xy[dim] += step)
        {
            CellState_t ct = mat.get(xy);
            if (ct == Active)
            {
                mutate = true;
                loop = false;
            }
            else
            {
                loop = (int(xy[dim]) != s_last);
            }
        }
    }
    if (!mutate)
    {
        u2_t mxy;
        mxy[dim] = exy[dim];
        mxy[1 - dim] = xy0[1 - dim];
        u_t opw0 = sub_solve(mat, bxy, mxy);
        mxy[dim] = bxy[dim];
        mxy[1 - dim] = xy0[1 - dim] + 1;
        u_t opw1 = sub_solve(mat, mxy, exy);
        bool opponent_win = (opw0 > 0) ^ (opw1 > 0);
        ret = opponent_win ? 0 : 1;
    }
    return ret;
}

void Bacterial::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
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

    void (Bacterial::*psolve)() =
        (naive ? &Bacterial::solve_naive : &Bacterial::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Bacterial bacterial(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (bacterial.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        bacterial.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
