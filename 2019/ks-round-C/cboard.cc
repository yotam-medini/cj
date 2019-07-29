// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <array>
// #include <set>
// #include <map>
// #include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<u_t, 2> u2_t;

static unsigned dbg_flags;

class BaseWHMatrix
{
  public:
    BaseWHMatrix(unsigned _w, unsigned _h) : w(_w), h(_h) {}
    const unsigned w; // width
    const unsigned h; // hight
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
    WHMatrix(unsigned _w=0, unsigned _h=0) : 
        BaseWHMatrix(_w, _h), _a(w*h > 0 ? new T[w*h] : 0) {}
    virtual ~WHMatrix() { delete [] _a; }
    const T& get(unsigned x, unsigned y) const { return _a[xy2i(x, y)]; }
    const T& get(const unsigned* xy) const { return get(xy[0], xy[1]); }
    void put(unsigned x, unsigned y, const T &v) { _a[xy2i(x, y)] = v; }
    void put(const unsigned* xy, const T &v) { put(xy[0], xy[1], v); }
    const T* raw() const { return _a; }
  private:
    T *_a;
};

typedef WHMatrix<u_t> mtx_t;

static void minimize(u_t& v, u_t by)
{
    if (v > by)
    {
        v = by;
    }
}

static void maximize(u_t& v, u_t by)
{
    if (v < by)
    {
        v = by;
    }
}

class CBoard
{
 public:
    CBoard(istream& fi);
    ~CBoard() { delete pmtx; }
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool good(const u2_t &bxy, const u2_t &exy) const;
    u_t r, c;
    u_t k;
    mtx_t *pmtx;
    u_t solution;
};

CBoard::CBoard(istream& fi) : solution(0)
{
    fi >> r >> c >> k;
    pmtx = new mtx_t(c, r);
    for (u_t y = r; y > 0;)
    {
        --y;
        for (u_t x = 0; x < c; ++x)
        {
            u_t v;
            fi >> v;
            pmtx->put(x, y, v);
        }
    }
}

void CBoard::solve_naive()
{
    u2_t bxy, exy;
    for (bxy[0] = 0; bxy[0] < c; ++bxy[0])
    {
        for (bxy[1] = 0; bxy[1] < r; ++bxy[1])
        {
            for (exy[0] = bxy[0] + 1; exy[0] <= c; ++exy[0])
            {
                for (exy[1] = bxy[1] + 1; exy[1] <= r; ++exy[1])
                {
                    u_t dx = exy[0] - bxy[0];
                    u_t dy = exy[1] - bxy[1];
                    u_t sz = dx * dy;
                    if ((solution < sz) && good(bxy, exy))
                    {
                        solution = sz;
                    }
                }
            }
        }
    }
}

bool CBoard::good(const u2_t& bxy, const u2_t& exy) const
{
    bool ret = true;
    for (u_t y = bxy[1]; ret && (y < exy[1]); ++y)
    {
        u_t x = bxy[0];
        u_t bmin = pmtx->get(x, y), bmax = bmin;
        for (; ret && x < exy[0]; ++x)
        {
            u_t v = pmtx->get(x, y);
            minimize(bmin, v);
            maximize(bmax, v);
            ret = ((bmax - bmin) <= k);
        }
    }
    return ret;
}

void CBoard::solve()
{
    solve_naive();
}

void CBoard::print_solution(ostream &fo) const
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

    void (CBoard::*psolve)() =
        (naive ? &CBoard::solve_naive : &CBoard::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        CBoard cboard(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (cboard.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        cboard.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
