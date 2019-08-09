// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <array>
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

template<typename R, typename RE>
class NextBoundBase
{
 public:
    NextBoundBase(const vi_t &values);
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
    int get(int i) const { return binm[0][i]; }
    int prev_bound_naive(int before) const
    {
        return prev_bound_value_naive(before, get(before));
    }
    int next_bound_naive(int after) const
    {
        return next_bound_value_naive(after, get(after));
    }
    int prev_bound_value_naive(int before, int v) const;
    int next_bound_value_naive(int after, int v) const;
 private:
    static int M(int x, int y) { return R()(x, y) ? x : y; }
    void fill_bin(vvi_t &bmax);
    vvi_t binm;
};

template<typename R, typename RE>
NextBoundBase<R, RE>::NextBoundBase(const vi_t &values)
{
    u_t sz = values.size();
    int log2 = 0;
    while ((1ul << log2) < sz)
    {
        ++log2;
    }
    binm.reserve(log2);
    binm.push_back(values);
    fill_bin(binm);
}

template<typename R, typename RE>
void NextBoundBase<R, RE>::fill_bin(vvi_t &bmax)
{
    while (bmax.back().size() > 1)
    {
        bmax.push_back(vi_t());
        vi_t &next = bmax.back();
        const vi_t &prev = bmax[bmax.size() - 2];
        u_t prev_sz = prev.size();
        next.reserve((prev_sz + 1) / 2);
        for (u_t i = 0, j = 0; j + 1 < prev_sz; ++i, j += 2)
        {
            next.push_back(M(prev[j], prev[j + 1]));
        }
        if (prev_sz % 2 != 0)
        {
            next.push_back(prev.back());
        }
    }    
}

template<typename R, typename RE>
int NextBoundBase<R, RE>::prev_bound_value(int before, int v) const
{
    int ret = -1;
    u_t p2b = u_t(-1), bib = u_t(-1);
    for (int p2 = int(binm.size()) - 1, bi = 0; p2 >= 0; --p2)
    {
        if ((bi + 1)*(1u << p2) <= unsigned(before))
        {
            if (RE()(binm[p2][bi], v))
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
            if (R()(v, binm[p2b][bib]))
            {
                --bib;
            }
            
        }
        ret = bib;
    }
    return ret;
}

template<typename R, typename RE>
int NextBoundBase<R, RE>::next_bound_value(int after, int v) const
{
    u_t sz = binm[0].size();
    int ret = sz;
    u_t p2b = u_t(-1), bib = u_t(-1);
    auto re = RE();
    for (int p2 = (re(binm.back()[0], v) ? binm.size() : 0) - 2, bi = 0;
        p2 >= 0; --p2)
    {
        unsigned bi1 = bi + 1;
        if (bi1*(1u << p2) > unsigned(after))
        {
            const vi_t& binm_p2 = binm[p2];
            if ((bi1 < binm_p2.size()) && re(binm_p2[bi1], v))
            {
                p2b = p2;
                bib = bi1;
            }
            bi = 2*bi;
        }
        else
        {
            bi = 2*bi + 2;
        }
    }
    if (p2b != u_t(-1))
    {
        while (p2b > 0)
        {
            --p2b;
            bib = 2*bib;
            if (R()(v, binm[p2b][bib]))
            {
                ++bib;
            }
            
        }
        ret = bib;
    }
    return ret;
}

template<typename R, typename RE>
int NextBoundBase<R, RE>::prev_bound_value_naive(int before, int v) const
{
    int ret = -1;
    for (int i = before - 1; i >= 0; --i)
    {
        if (RE()(get(i), v))
        {
            ret = i;
            i = 0;
        }
    }
    return ret;
}

template<typename R, typename RE>
int NextBoundBase<R, RE>::next_bound_value_naive(int after, int v) const
{
    int sz = binm[0].size();
    int ret = sz;
    for (int i = after + 1; i < sz; ++i)
    {
        if (RE()(get(i), v))
        {
            ret = i;
            i = sz;
        }
    }
    return ret;
}

typedef NextBoundBase<less<int>, less_equal<int>> NextBoundMin;
typedef NextBoundBase<greater<int>, greater_equal<int>> NextBoundMax;

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
    }
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool good(const u2_t &bxy, const u2_t &exy) const;
    void compute_width_row(u_t y, vu_t& width_row);
    u_t row_get_width_at_x(
        const vi_t& row, u_t x,
        const NextBoundMax& nb_max, const NextBoundMin& nb_min) const;
    u_t max_min_widths_at_x(
        u_t& width_high, u_t& width_low,
        const NextBoundMax& nb_max, const NextBoundMin& nb_min, 
        u_t at, int max_bound)
        const;
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
    // solve_naive();
    mtx_t width_mtx(c, r);
    vu_t width_row;
    width_row.reserve(c);
    for (u_t i = 0; i < r; ++i)
    {
        compute_width_row(i, width_row);
        for (u_t j = 0; j < c; ++j)
        {
            width_mtx.put(j, i, width_row[j]);
        }
    }

    vu_t width_col(vu_t::size_type(r), 0);
    for (u_t j = 0; j < c; ++j)
    {
        for (u_t i = 0; i < r; ++i)
        {
            width_col[i] = width_mtx.get(j, i);
        }
        u_t rect = max_bounded_rect(width_col);
        maximize(solution, rect);                
    }
}

void CBoard::compute_width_row(u_t y, vu_t& width_row)
{
    vi_t row;
    row.reserve(c);
    for (u_t x = 0; x < c; ++x)
    {
        int v = pmtx->get(x, y);
        row.push_back(v);
    }
    width_row.clear();
    NextBoundMax nb_max(row);
    NextBoundMin nb_min(row);
    for (u_t x = 0; x < c; ++x)
    {
        u_t width = row_get_width_at_x(row, x, nb_max, nb_min);
        width_row.push_back(width);
    }
}

u_t CBoard::row_get_width_at_x(
    const vi_t& row, u_t x,
    const NextBoundMax& nb_max, const NextBoundMin& nb_min) const
{
    int v = row[x];
    int max_high = v + k;
    int max_low = v;
    u_t width_high(u_t(-1)), width_low(u_t(-1));
    u_t width(u_t(-1));
    bool loop = true;
    while (loop)
    {
        int max_mid = (max_low + max_high)/2;
        width = max_min_widths_at_x(
            width_high, width_low, nb_max, nb_min, x, max_mid);
        if (width_high == width_low)
        {
            loop = false;
        }
        else
        {
            loop = (max_high - max_low) > 1;
            int* max_who = (width_high > width_low ? &max_high : &max_low);
            *max_who = max_mid;
        }
    }
    if (max_low < max_high)
    {
        u_t width_alt = max_min_widths_at_x(
            width_high, width_low, nb_max, nb_min, x, max_high);
        maximize(width, width_alt);
    }
    return width;
}

u_t CBoard::max_min_widths_at_x(
    u_t& width_high, u_t& width_low,
    const NextBoundMax& nb_max, const NextBoundMin& nb_min,
    u_t x, int max_bound)
    const
{
    int min_bound = max_bound - k;
    u_t x_bound_max = nb_max.next_bound_value(x, max_bound + 1);
    u_t x_bound_min = nb_min.next_bound_value(x, min_bound - 1);
    width_high = x_bound_max - x; 
    width_low = x_bound_min - x;
    return min(width_high, width_low);
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
