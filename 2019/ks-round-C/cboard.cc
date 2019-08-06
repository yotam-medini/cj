// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <array>
// #include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<int> vi_t;
typedef vector<vi_t> vvi_t;
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

class NextBound
{
 public:
    NextBound(const vi_t &values);
    int prev_bound(int before) const 
    {
        return prev_bound_value(before, get(before));
    }
    int next_bound(int after) const
    {
        return next_bound_value(after, get(after));
    }
    int prev_bound_value(int before, int v) const;
    int next_bound_value(int after, int v) const;
    int get(int i) const { return binmax[0][i]; }
 private:
    void fill_bin(vvi_t &bmax);
    int bin_prev_bound_value(const vvi_t &bmax, int before, int v) const;
    vvi_t binmax;
    vvi_t rbinmax;
};

NextBound::NextBound(const vi_t &values)
{
    u_t sz = values.size();
    int log2 = 0;
    while ((1ul << log2) < sz)
    {
        ++log2;
    }
    binmax.reserve(log2);
    binmax.push_back(values);
    fill_bin(binmax);
    rbinmax.reserve(log2);
    vi_t rvalues(values.rbegin(), values.rend());
    rbinmax.push_back(rvalues);
    fill_bin(rbinmax);
}

void NextBound::fill_bin(vvi_t &bmax)
{
    while (bmax.back().size() > 1)
    {
        bmax.push_back(vi_t());
        vi_t &next = bmax.back();
        const vi_t &prev = bmax[bmax.size() - 2];
        u_t prev_sz = prev.size();
        next.reserve(prev_sz / 2);
        for (u_t i = 0, j = 0; j + 1 < prev_sz; ++i, j += 2)
        {
            next.push_back(max(prev[j], prev[j + 1]));
        }
    }    
}

int NextBound::prev_bound_value(int before, int v) const
{
    return bin_prev_bound_value(binmax, before, v);
}

int NextBound::next_bound_value(int after, int v) const
{
    u_t sz = rbinmax[0].size();
    int bi = bin_prev_bound_value(rbinmax, sz - after - 1, v);
    int ret = sz - bi - 1;
    return ret;
}

int NextBound::bin_prev_bound_value(const vvi_t &bmax, int before, int v) const
{
    int ret = -1;
    u_t p2b = u_t(-1), bib = u_t(-1);
    for (int p2 = int(bmax.size()) - 1, bi = 0; p2 >= 0; --p2)
    {
        if ((bi + 1)*(1u << p2) <= unsigned(before))
        {
            if (v <= bmax[p2][bi])
            {
                p2b = p2;
                bib = bi;
            }
            bi = 2*bi + 2;
        }
        else
        {
            bi = 2*bi;
        }
    }
    if (p2b != u_t(-1))
    {
        while (p2b > 0)
        {
            --p2b;
            bib = 2*bib + 1;
            if (bmax[p2b][bib] < v)
            {
                --bib;
            }
            
        }
        ret = bib;
    }
    return ret;
}

class MaxBoundRect
{
 public:
    MaxBoundRect(const vu_t& _v) : v(_v), result(0), bi(0), ei(0)
    {
        solve();
    }
    u_t get(u_t* pbi=0, u_t* pei=0) const
    {
        u_t dum;
        *(pbi ? : &dum) = bi;
        *(pei ? : &dum) = ei;
        return result;
    }
 private:
    class XY
    {
      public:
         XY(u_t _x=0, u_t _y=0) : x(_x), y(_y) {}
      u_t x, y;
    };
    typedef vector<XY> vxy_t;
    void solve();
    u_t top() const { return inc_stack.empty() ? 0 : inc_stack.back().y; }
    u_t pop(u_t ie);
    const vu_t &v;
    u_t result;
    u_t bi;
    u_t ei;
    vxy_t inc_stack;
};

void MaxBoundRect::solve()
{
    u_t sz = v.size();
    bi = 0;
    ei = 1;
    
    for (u_t i = 0; i < sz; ++i)
    {
        u_t x = i;
        u_t y = v[i];
        while (y < top())
        {
            x = pop(i);
        }
        if (top() < y)
        {
            inc_stack.push_back(XY(x, y));
        }
    }
    while (!inc_stack.empty())
    {
        pop(sz);
    }
}

u_t MaxBoundRect::pop(u_t ie)
{
    const XY& back = inc_stack.back();
    u_t dx = ie - back.x;
    u_t xback = back.x;
    u_t yback = back.y;
    u_t rect = dx*yback;
    if (result < rect)
    {
        result = rect;
        bi = xback;
        ei = ie;
    }
    inc_stack.pop_back();
    return xback;
}

u_t max_bounded_rect(const vu_t& v, u_t* pbi=0, u_t* pei=0)
{
    return MaxBoundRect(v).get(pbi, pei);
}

class CBoard
{
 public:
    CBoard(istream& fi);
    ~CBoard()
    {
        delete pmtx;
        delete width_mtx;
    }
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool good(const u2_t &bxy, const u2_t &exy) const;
    void compute_width_row(u_t y, vu_t& width_row);
    u_t r, c;
    u_t k;
    mtx_t *pmtx;
    u_t solution;
    mtx_t *width_mtx;
};

CBoard::CBoard(istream& fi) : solution(0), width_mtx(0)
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
    // solve_naive();
    pmtx = new mtx_t(c, r);
    for (u_t i = 0; i < r; ++i)
    {
        vu_t width_row;
        compute_width_row(i, width_row);
    }
}

void CBoard::compute_width_row(u_t y, vu_t& width_row)
{
    vi_t row, neg_row;
    row.reserve(c);
    neg_row.reserve(c);
    for (u_t x = 0; x < c; ++x)
    {
        int v = pmtx->get(x, y);
        row.push_back(v);
        neg_row.push_back(-v);
    }
    NextBound nb(row);
    NextBound neg_nb(neg_row);
    for (u_t x = 0; x < c; ++x)
    {
        int v = row[x];
        int m = max(0, int(v - k));
        int M = v + k;
        int m_bound_low = neg_nb.next_bound_value(-(m + 1), x);
        int m_bound_high = nb.next_bound_value(m + k + 1, x);
        int m_bound = min(m_bound_low, m_bound_high);
        int M_bound_low = neg_nb.next_bound_value(-(M - k - 1), x);
        int M_bound_high = nb.next_bound_value(M + 1, x);
        int M_bound = min(M_bound_low, M_bound_high);
        
    }
    
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
