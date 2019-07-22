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

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
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
    void put(unsigned x, unsigned y, const T &v) { _a[xy2i(x, y)] = v; }
    const T* raw() const { return _a; }
  private:
    T *_a;
};

typedef enum {Empty, Full, Active} CellState_t;

typedef WHMatrix<CellState_t> matcell_t;

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
    State state0(false, mat_initial);
    for (u_t x = 0; x < mat_initial.w; ++x)
    {
        for (u_t y = 0; y < mat_initial.h; ++y)
        {
            CellState_t ct = mat_initial.get(x, y);
            if (ct == Empty)
            {
                for (u_t b = 0; b != 2; ++b)
                {
                    if (is_win(state0, x, y, b == 0))
                    {
                        ++solution;
                    }
                }
            }
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
    
    if (horizontal)
    {
        for (u_t d = 0; d != 2; ++d)
        {
            int step = (d == 0 ? 1 : -1);
            int sx_last = (step == 1 ? int(m.w) - 1 : 0);
            bool loop = int(x) != sx_last;
            for (int sx = x + step; loop; sx += step)
            {
                CellState_t ct = m.get(sx, y);
                if (ct == Empty)
                {
                    m.put(sx, y, Full);
                    loop = (sx != sx_last);
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
    }
    else // vertical
    {
        for (u_t d = 0; d != 2; ++d)
        {
            int step = (d == 0 ? 1 : -1);
            int sy_last = (step == 1 ? int(m.h) - 1: 0);
            bool loop = int(y) != sy_last;
            for (int sy = y + step; loop; sy += step)
            {
                CellState_t ct = m.get(x, sy);
                if (ct == Empty)
                {
                    m.put(x, sy, Full);
                    loop = (sy != sy_last);
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
    solve_naive();
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
