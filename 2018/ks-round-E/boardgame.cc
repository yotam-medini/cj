// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <chrono>
#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

#include <cstdlib>
using namespace std;

typedef chrono::duration<double, std::milli> durms_t;
typedef chrono::high_resolution_clock::time_point tp_t;
typedef chrono::duration<double> duration_t;
typedef chrono::time_point<chrono::high_resolution_clock> hr_clock_t;

double dt_OLD(tp_t t0)
{
    tp_t t1 = chrono::high_resolution_clock::now();
    duration_t ddt = chrono::duration_cast<duration_t>(t1 - t0);
    double ret = ddt.count();
    return ret;
}

double dt(hr_clock_t t0)
{
    hr_clock_t t1 = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> fp_ms = t1 - t0;
    double ret = fp_ms.count();
    return ret;
}

double dtr(hr_clock_t& t0)
{
    hr_clock_t t1 = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> fp_ms = t1 - t0;
    double ret = fp_ms.count();
    t0 = t1;
    return ret;
}

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;
typedef vector<vu_t> vvu_t;
typedef array<u_t, 2> au2_t;
typedef array<u_t, 3> au3_t;
typedef vector<au2_t> vau2_t;
typedef vector<au3_t> vau3_t;
typedef array<au2_t, 3> a3au2_t;
typedef vector<a3au2_t> va3au2_t;
typedef array<au3_t, 3> a3au3_t;
typedef vector<a3au3_t> va3au3_t;
typedef set<u_t> setu_t;

static unsigned dbg_flags;

enum { BIT_MAX = 5*1000000 };

string vu_to_str(const vu_t& a)
{
    ostringstream os;
    os << '{';
    const char* sep = "";
    for (u_t x: a)
    {
        os << sep << x; sep = ", ";
    }
    os << '}';
    string ret = os.str();
    return ret;
}

size_t get_index(const vu_t& a, u_t x)
{
    vu_t::const_iterator iter = lower_bound(a.begin(), a.end(), x);
    size_t i = iter - a.begin();
    return i;
}

size_t n_below(const vu_t& a, u_t x)
{
    size_t n = 0;
    if (x > 0)
    {
        vu_t::const_iterator iter = upper_bound(a.begin(), a.end(), x - 1);
        n = iter - a.begin();
    }
    return n;
}

void combination_first(vu_t &c, u_t n, u_t k)
{
    c = vu_t(k);
    iota(c.begin(), c.end(), 0);
}

bool combination_next(vu_t &c, u_t n)
{
    u_t j = 0, k = c.size();

    // sentinels (Knuth) (Why the second (0) ??!)
    c.push_back(n);  c.push_back(0);

    while ((j < k) && (c[j] + 1 == c[j + 1]))
    {
        c[j] = j;
        ++j;
    }
    bool ret = j < k;
    if (ret)
    {
        ++(c[j]);
    }

    c.pop_back(); c.pop_back(); // the sentinels

    return ret;
}

const vvu_t& get_combinations(u_t n, u_t k)
{
    typedef map<au2_t, vvu_t> nk2combs_t;
    static nk2combs_t nk2combs;
    au2_t key{n, k};
    auto er = nk2combs.equal_range(key);
    nk2combs_t::iterator iter = er.first;
    if (er.first == er.second)
    {
        vvu_t combs;
        vu_t comb;
        bool more = true;
        for (combination_first(comb, n, k); more; 
            more = combination_next(comb, n))
        {
            combs.push_back(comb);
        }
        iter = nk2combs.insert(iter, nk2combs_t::value_type(key, combs));
    }
    return iter->second;
}

static va3au2_t choose_6_222;
static void _compute_choose_6_222()
{
    vu_t head;
    combination_first(head, 6, 2);
    bool head_more = true;
    for (; head_more; head_more = combination_next(head, 6))
    {
        a3au2_t a32;
        copy(head.begin(), head.end(), &a32[0][0]);
        vu_t a6(6, 0);
        iota(a6.begin(), a6.end(), 0);
        for (u_t i = 0; i != 2; ++i)
        {
            swap(a6[i], a6[head[i]]);
        }
        sort(a6.begin() + 2, a6.end());
        vu_t mid;
        combination_first(mid, 4, 2);
        bool mid_more = true;
        for (; mid_more; mid_more = combination_next(mid, 4))
        {
            vu_t a4(4, 0);
            iota(a4.begin(), a4.end(), 0);
            for (u_t i = 0; i != 2; ++i)
            {
                swap(a4[i], a4[mid[i]]);
                a32[1][i] = a6[a4[i] + 2];
            }
            sort(a4.begin() + 2, a4.end());
            for (u_t i = 0; i != 2; ++i)
            {
                a32[2][i] = a6[a4[i + 2] + 2];
            }
            choose_6_222.push_back(a32);
        }
    }
}

static void compute_choose_6_222()
{
    if (choose_6_222.empty())
    {
        _compute_choose_6_222();
        if (dbg_flags & 0x1) {
            cerr << "#(choose_6_222)="<<choose_6_222.size() << '\n'; }
    }
}

static va3au3_t choose_9_333;
static void _compute_choose_9_333()
{
    vu_t head;
    combination_first(head, 9, 3);
    bool head_more = true;
    for (; head_more; head_more = combination_next(head, 9))
    {
        a3au3_t a33;
        copy(head.begin(), head.end(), &a33[0][0]);
        vu_t a9(9, 0);
        iota(a9.begin(), a9.end(), 0);
        for (u_t i = 0; i != 3; ++i)
        {
            swap(a9[i], a9[head[i]]);
        }
        sort(a9.begin() + 3, a9.end());
        vu_t mid;
        combination_first(mid, 6, 3);
        bool mid_more = true;
        for (; mid_more; mid_more = combination_next(mid, 6))
        {
            vu_t a6(6, 0);
            iota(a6.begin(), a6.end(), 0);
            for (u_t i = 0; i != 3; ++i)
            {
                swap(a6[i], a6[mid[i]]);
                a33[1][i] = a9[a6[i] + 3];
            }
            sort(a6.begin() + 3, a6.end());
            for (u_t i = 0; i != 3; ++i)
            {
                a33[2][i] = a9[a6[i + 3] + 3];
            }
            choose_9_333.push_back(a33);
        }
    }
}

static void compute_choose_9_333()
{
    if (choose_9_333.empty())
    {
        _compute_choose_9_333();
        if (dbg_flags & 0x1) {
            cerr << "#(choose_9_333)="<<choose_9_333.size() << '\n'; }
    }
}

class T0VT1
{
 public:
    vu_t third0;
    vvu_t third1s;
};
typedef vector<T0VT1> vt0vt1_t;

void get_comb_2thirds(vt0vt1_t& v, u_t sz)
{
    if (dbg_flags & 0x1) { cerr << __func__ << " sz=" << sz << '\n'; }
    const u_t tsz = sz/3;
    // vu_t c0;
    const vvu_t& combs_sz_tsz = get_combinations(sz, tsz);
    T0VT1 t0vt1;
    vu_t& c0 = t0vt1.third0;
    vu_t rest; 
    rest.reserve(2*tsz);
    // combination_first(c0, sz, tsz);
    vu_t c1; c1.reserve(tsz);
    // vu_t c1i; c1i.reserve(tsz);
    // for (bool c0_more = true; c0_more; c0_more = combination_next(c0, sz))
    for (const vu_t& comb: combs_sz_tsz)
    {
        c0 = comb;
        t0vt1.third1s.clear();
        rest.clear();
        for (u_t ai = 0, ai0 = 0; ai != sz; ++ai)
        {
            if ((ai0 < tsz) && (ai == c0[ai0]))
            {
                ++ai0;
            }
            else
            {
                rest.push_back(ai);
            }
        }
        const vvu_t& combs1 = get_combinations(2*tsz, tsz);
        for (const vu_t& c1i: combs1)
        {
            c1.clear();
            for (u_t i: c1i)
            {
                c1.push_back(rest[i]);
            }
            t0vt1.third1s.push_back(c1);
        }
        v.push_back(t0vt1);
    }
}

class G2Thirds
{
 public:
   G2Thirds(u_t _b0=0, const vu_t& _b1=vu_t()) : b0(_b0), b1s(_b1) {}
   u_t b0;
   vu_t b1s;
};
typedef vector<G2Thirds> vg2thirds_t;
bool operator<(const G2Thirds& g0, const G2Thirds& g1)
{
    bool lt = (g0.b0 < g1.b0); // || ((g0.b0 == g1.b0) && (g0.b1s < g1.b1s));
    return lt;
}

void compute_g2thirds(vg2thirds_t& r, const vu_t a)
{
    typedef map<u_t, vt0vt1_t> u2combs_t;
    static u2combs_t sz_to_th01combs;
    const u_t sz = a.size();
    u2combs_t::const_iterator iter = sz_to_th01combs.find(sz);
    if (iter == sz_to_th01combs.end())
    {
        vt0vt1_t th01combs;
        get_comb_2thirds(th01combs, sz);
        iter = sz_to_th01combs.insert(sz_to_th01combs.end(), 
            u2combs_t::value_type(sz, th01combs));
    }
    const vt0vt1_t& t01combs = iter->second;
    r.clear(); r.reserve(t01combs.size());
    for (const T0VT1& t0vt1: t01combs)
    {
        G2Thirds g;
        for (u_t i: t0vt1.third0)
        {
            g.b0 += a[i];
        }
        for (const vu_t c1: t0vt1.third1s)
        {
            u_t b1 = 0;
            for (u_t i: c1)
            {
                b1 += a[i];
            }
            g.b1s.push_back(b1);
        }
        sort(g.b1s.begin(), g.b1s.end());
        r.push_back(g);
    }
    sort(r.begin(), r.end());
}

void g2thirds_to_a2inc(vau2_t& r, const vg2thirds_t& g, u_t total)
{
    const u_t third = total / 3;
    for (size_t gi = 0, ge = 0, gsz = g.size();
        (gi < gsz) && (g[gi].b0 <= third); gi = ge)
    {
        const u_t b0 = g[gi].b0;
        const u_t b1_bound = (total - b0)/2;
        const vu_t& gib1s = g[gi].b1s;
        vu_t b1s(
            lower_bound(gib1s.begin(), gib1s.end(), b0),
            upper_bound(gib1s.begin(), gib1s.end(), b1_bound));
        for (ge = gi + 1; (ge < gsz) && (g[ge].b0 == b0); ++ge)
        {
            const vu_t& geb1s = g[ge].b1s;
            vu_t mrg;
            merge(b1s.begin(), b1s.end(), 
                lower_bound(geb1s.begin(), geb1s.end(), b0),
                upper_bound(geb1s.begin(), geb1s.end(), b1_bound),
                back_inserter(mrg));
            mrg.erase(unique(mrg.begin(), mrg.end()), mrg.end());
            swap(b1s, mrg);
        }
        for (u_t b1: b1s)
        {
            r.push_back(au2_t{b0, b1});
        }
    }
}

typedef u_t stval_t;
typedef vector<stval_t> vval_t;
typedef vector<vval_t> vvval_t;

class SegTree2D
{
 public:
    SegTree2D(size_t _m=1, size_t _n=1) :
        m(_m), n(_n), st(2*_m, vval_t(2*_n, 0)) {}
    void add(size_t x, size_t y, stval_t v)
    {
        x += m;
        size_t yj = y + n;
        st[x][yj] += v;
        for (size_t yup = 0; yj > 1; yj = yup)
        {
            yup = yj / 2;
            st[x][yup] = st[x][yj] + st[x][yj ^ 1];
        }

        for (size_t xup = 0; x > 1; x = xup)
        {
            xup = x / 2;
            yj = y + n;
            vval_t& st_xup = st[xup];
            st_xup[yj] = st[x][yj] + st[x ^ 1][yj];
            for (size_t yup = 0; yj > 1; yj = yup)
            {
                yup = yj / 2;
                st_xup[yup] = st[x][yup] + st[x ^ 1][yup];
            }
        }
    }
    stval_t query(size_t xb, size_t xe, size_t yb, size_t ye) const
    {
        size_t ret = 0;
        for (xb += m, xe += m; xb < xe; xb /= 2, xe /= 2)
        {
            if (xb % 2 == 1)
            {
                ret += query1d(st[xb++], yb, ye);
            } 
            if (xe % 2 == 1)
            {
                ret += query1d(st[--xe], yb, ye);
            } 
        }
        return ret;
    }
 private:
    stval_t query1d(const vval_t& st1d, size_t yb, size_t ye) const
    {
        size_t ret = 0;
        for (yb += n, ye += n; yb < ye; yb /= 2, ye /= 2)
        {
            if (yb % 2 == 1)
            {
                ret += st1d[yb++];
            } 
            if (ye % 2 == 1)
            {
                ret += st1d[--ye];
            } 
        }
        return ret;
    }
    size_t m, n;
    vvval_t st;
};

class BoardGame
{
 public:
    BoardGame(istream& fi);
    void solve_naive();
    void solve();
    void solve_seg2d();
    void solve_cdq2();
    void print_solution(ostream&) const;
 private:
    void set2(a3au2_t& gset, const a3au2_t& ig_set, const vu_t& cards) const;
    void set3(a3au3_t& gset, const a3au3_t& ig_set, const vu_t& cards) const;
    bool win2(const a3au2_t& a_set, const a3au2_t& b_set, bool& all) const;
    bool win(const a3au3_t& a_set, const a3au3_t& b_set) const;
    u_t lutn_sum(const vu_t& v, const vu_t& lut, u_t nsz) const;
    void subtract_all3_wins();
    void get_sorted_battles(vau3_t& battles, const vu_t& army) const;
    void reduce_battles(vau3_t& a_battles, vau3_t& b_battles) const;
    u_t n;
    vu_t a;
    vu_t b;
    double solution;
    u_t b_third_max;
    u_t a_total;
    u_t b_total;
    // vau2_t a_combs, b_combs;
    vg2thirds_t a_combs, b_combs;
    vu_t b_inc;
    vau2_t a_inc;
    vu_t a_comb_wins;
};

BoardGame::BoardGame(istream& fi) : solution(0.)
{
    fi >> n;
    copy_n(istream_iterator<u_t>(fi), 3*n, back_inserter(a));
    copy_n(istream_iterator<u_t>(fi), 3*n, back_inserter(b));
}

void BoardGame::solve_naive()
{
    compute_choose_6_222();
    compute_choose_9_333();
    if (n == 2)
    {
        u_t const ncomb = choose_6_222.size();
        u_t max_wins = 0, n_all_win = 0;
        a3au2_t a_set_win;
        for (u_t ia = 0; ia != ncomb; ++ia)
        {
            const a3au2_t& ia_set = choose_6_222[ia];
            a3au2_t a_set;
            set2(a_set, ia_set, a);
            u_t n_wins = 0, n_all = 0;
            for (u_t ib = 0; ib != ncomb; ++ib)
            {
                const a3au2_t& ib_set = choose_6_222[ib];
                a3au2_t b_set;
                set2(b_set, ib_set, b);
                bool all;
                if (win2(a_set, b_set, all))
                {
                    ++n_wins;
                    n_all += (all ? 1 : 0);
                }
            }
            if (max_wins < n_wins)
            {
                max_wins = n_wins;
                a_set_win = a_set;
                n_all_win = n_all;
            }
        }
        if (dbg_flags & 0x1) { cerr << "max_wins="<<max_wins << 
            ", n_all_win="<<n_all_win << ", a_set: ";
            for (u_t i = 0; i != 3; ++i) { 
                cerr << a_set_win[i][0] << ' ' << a_set_win[i][1] << "  "; }
            cerr << '\n'; }
        solution = double(max_wins)/double(ncomb);
    }
    if (n == 3)
    {
        a3au3_t a_set_win;
        u_t const ncomb = choose_9_333.size();
        u_t max_wins = 0;
        for (u_t ia = 0; ia != ncomb; ++ia)
        {
            const a3au3_t& ia_set = choose_9_333[ia];
            a3au3_t a_set;
            set3(a_set, ia_set, a);
            u_t n_wins = 0;
            for (u_t ib = 0; ib != ncomb; ++ib)
            {
                const a3au3_t& ib_set = choose_9_333[ib];
                a3au3_t b_set;
                set3(b_set, ib_set, b);
                if (win(a_set, b_set))
                {
                    ++n_wins;
                }
            }
            if (max_wins < n_wins)
            {
                max_wins = n_wins;
                a_set_win = a_set;
            }
        }
        if (dbg_flags & 0x1) { cerr << "max_wins="<<max_wins << ", a_set: ";
            for (u_t i = 0; i != 3; ++i) {
                u_t g = accumulate(&a_set_win[i][0], &a_set_win[i][3], 0);
                cerr << g << "  "; }
            cerr << '\n'; }
        solution = double(max_wins)/double(ncomb);
    }
}

void BoardGame::set2(a3au2_t& g_set, const a3au2_t& ig_set, const vu_t& cards)
    const
{
    for (u_t si = 0; si != 3; ++si)
    {
        for (u_t sj = 0; sj != 2; ++sj)
        {
            g_set[si][sj] = cards[ig_set[si][sj]];
        }
    }
}

void BoardGame::set3(a3au3_t& g_set, const a3au3_t& ig_set, const vu_t& cards)
    const
{
    for (u_t si = 0; si != 3; ++si)
    {
        for (u_t sj = 0; sj != 3; ++sj)
        {
            g_set[si][sj] = cards[ig_set[si][sj]];
        }
    }
}

bool BoardGame::win2(const a3au2_t& a_set, const a3au2_t& b_set,
    bool& all) const
{
    u_t battle_wins = 0;
    for (u_t battle = 0; battle != 3; ++battle)
    {
        u_t a_sum = 0, b_sum = 0;
        for (u_t i = 0; i != 2; ++i)
        {
            a_sum += a_set[battle][i];
            b_sum += b_set[battle][i];
        }
        if (a_sum > b_sum)
        {
            ++battle_wins;
        }
    }
    bool ret = (battle_wins >= 2);
    all = (battle_wins == 3);
    return ret;
}

bool BoardGame::win(const a3au3_t& a_set, const a3au3_t& b_set) const
{
    u_t battle_wins = 0;
    for (u_t battle = 0; battle != 3; ++battle)
    {
        u_t a_sum = 0, b_sum = 0;
        for (u_t i = 0; i != 3; ++i)
        {
            a_sum += a_set[battle][i];
            b_sum += b_set[battle][i];
        }
        if (a_sum > b_sum)
        {
            ++battle_wins;
        }
    }
    bool ret = (battle_wins >= 2);
    return ret;
}

u_t BoardGame::lutn_sum(const vu_t& v, const vu_t& lut, u_t nsz) const
{
    u_t sum = 0;
    for (u_t i = 0; i != nsz; ++i)
    {
        sum += v[lut[i]];
    }
    return sum;
}

typedef vector<int> vi_t;

class BIT
{
 public:
    BIT(u_t _max_idx) : 
        max_idx(_max_idx), 
        tree(_max_idx + 1, 0),
        total_delta(0)
    {}
    void update(u_t idx, int delta)
    {
        if (idx > 0)
        {
            while (idx <= max_idx)
            {
                tree[idx] += delta;
                idx += (idx & -idx);
            }
        }
        total_delta += delta;
    }
    int query(int idx) const
    {
        int n = 0;
        while (idx > 0)
        {
            n += tree[idx];
            idx -= (idx & -idx); // removing low bit
        }
        return n;
    }
    int cquery(int idx) const // complement query
    {
        int n = total_delta - query(idx);
        return n;
    }
 private:
    u_t max_idx;
    vi_t tree;
    u_t total_delta;
};

typedef array<u_t, 2> au2_t;

class IPoint2
{
 public:
    IPoint2(int _i=-1, const au2_t& _v={0, 0}) : i(_i), v(_v) {}
    int i;
    au2_t v;
};
typedef vector<IPoint2> vipt2_t;

// inspired by: 
// robert1003.github.io/2020/01/31/cdq-divide-and-conquer.html
class CDQ2
{
 public:
    CDQ2(const vau2_t& _upts, const vau2_t& _pts) :
        n_below(_upts.size(), 0),
        pts(_pts)
    {
        ipts.reserve(_upts.size());
        for (size_t i = 0, sz = _upts.size(); i < sz; ++i)
        {
            const au2_t& upt = _upts[i];
            const au2_t& upt1{upt[0] - 1, upt[1] - 1};
            ipts.push_back(IPoint2(i, upt1));
        }
        solve();
    }
    vu_t n_below;
 private:
    void solve();
    vipt2_t ipts;
    vau2_t pts;
};

void CDQ2::solve()
{
    static BIT bit(BIT_MAX);
    sort(ipts.begin(), ipts.end(),
        [](const IPoint2& p0, const IPoint2& p1) -> bool
        {
            return p0.v[0] < p1.v[0];
        });
    const size_t isz = ipts.size();
    const size_t bsz = pts.size();
    vu_t record;
    u_t sum = 0;
    size_t a = 0, b = 0;
    while ((a < isz) && (b < bsz))
    {
        const IPoint2& a_ipt = ipts[a];
        const au2_t& b_pt = pts[b];
        if (b_pt[0] <= a_ipt.v[0])
        {
            u_t y = b_pt[1];
            bit.update(y, 1);
            record.push_back(y);
            ++sum;
            ++b;
        }
        else
        {
            n_below[a_ipt.i] += sum - bit.cquery(a_ipt.v[1]);
            ++a;
        }
    }
    while (a < isz)
    {
        const IPoint2& a_ipt = ipts[a];
        n_below[a_ipt.i] += sum - bit.cquery(a_ipt.v[1]);
        ++a;
    }
    for (u_t y: record)
    {
        bit.update(y, -1);
    }
}

class IPoint
{
 public:
    IPoint(int _i=-1, const au3_t& _v={0, 0, 0}) : i(_i), v(_v) {}
    int i;
    au3_t v;
};
bool operator==(const IPoint& p0, const IPoint& p1)
{
    return (p0.i == p1.i) && (p0.v == p1.v);
}
typedef vector<IPoint> vipt_t;

// inspired by: 
// robert1003.github.io/2020/01/31/cdq-divide-and-conquer.html
class CDQ
{
 public:
    CDQ(const vau3_t& upts, const vau3_t& pts) :
        n_below(upts.size(), 0)
    {
        tp_t t0 = chrono::high_resolution_clock::now();
        set_ipts(upts, pts);
        if (dbg_flags & 0x1) { cerr << "  t="<< dtr(t0) << " CDQ::set_ipts\n"; }
        solve();
        if (dbg_flags & 0x1) { cerr << "  t="<< dtr(t0) << " CDQ::solve\n"; }
    }
    vu_t n_below;
 private:
    void set_ipts(const vau3_t& upts, const vau3_t& pts);
    void solve();
    void cdq(size_t l, size_t r);
    void cdq_comp(size_t l, size_t mid, size_t r);
    vipt_t ipts;
    vu_t record;
    vipt_t tmp; 
};

void CDQ::set_ipts(const vau3_t& upts, const vau3_t& pts)
{
    vipt_t u_ipts; u_ipts.reserve(upts.size());
    for (size_t i = 0, sz = upts.size(); i < sz; ++i)
    {
        const au3_t& upt = upts[i];
        const au3_t& upt1{upt[0] - 1, upt[1] - 1, upt[2] - 1}; // Z as-is ?
        u_ipts.push_back(IPoint(i, upt1));
    }

    vipt_t l_ipts; l_ipts.reserve(pts.size());
    for (const au3_t& v: pts)
    {
        l_ipts.push_back(IPoint(-1, v));
    }

    ipts.reserve(upts.size() + pts.size());
    merge(u_ipts.begin(), u_ipts.end(), l_ipts.begin(), l_ipts.end(), 
        back_inserter(ipts),
        [](const IPoint& p0, const IPoint& p1) -> bool
        {
            size_t i;
            for (i = 0; (i < 3) && (p0.v[i] == p1.v[i]); ++i) {}
            bool lt = (i < 3 ? (p0.v[i] < p1.v[i]) : p0.i < p1.i);
            return lt;
        });
    vipt_t sipts(ipts);
}

void CDQ::solve()
{
    u_t sz = ipts.size();
    cdq(0, sz);
}

void CDQ::cdq(size_t l, size_t r)
{
    if (l + 1 < r)
    {
        size_t mid = (l + r)/2;
        cdq(l, mid);
        cdq(mid, r);
        cdq_comp(l, mid, r);
    }
}

void CDQ::cdq_comp(size_t l, size_t mid, size_t r)
{
    // Now ipts[l,mid) [0(x)] <= ipts[mid,r) [0(x)]
    // and both have increasing [1(y)]
    static BIT bit(BIT_MAX);
    tmp.reserve(r - l);
    record.clear();
    tmp.clear();
    size_t a = l, b = mid;
    u_t sum = 0;
    while(a < mid && b < r)
    {
        const IPoint& ipta = ipts[a];
        const IPoint& iptb = ipts[b];
        if (ipta.v[1] <= iptb.v[1])
        {
            if (ipta.i == -1)
            {
                bit.update(ipta.v[2], 1);
                sum++;
                record.push_back(ipta.v[2]);
            }
            tmp.push_back(ipta);
            ++a;
        } 
        else
        {
            if (iptb.i >= 0)
            {
                n_below[iptb.i] += sum - bit.cquery(iptb.v[2]);
            }
            tmp.push_back(iptb);
            ++b;
        }
    }
    for ( ; a < mid; ++a)
    {
        tmp.push_back(ipts[a]);
    }
    for ( ; b < r; ++b)
    {
        const IPoint& iptb = ipts[b];
        if (iptb.i >= 0)
        {
            n_below[iptb.i] += sum - bit.cquery(iptb.v[2]);
        }
        tmp.push_back(iptb);
    }
    copy(tmp.begin(), tmp.end(), ipts.begin() + l);
    for (u_t z: record)
    {
        bit.update(z, -1);
    }
}

void BoardGame::solve_cdq2()
{
    vau3_t a_battles, b_battles;
    tp_t t0 = chrono::high_resolution_clock::now();
    get_sorted_battles(a_battles, a);
    get_sorted_battles(b_battles, b);
    if (dbg_flags & 0x1) { cerr << "t="<< dtr(t0) << " 2 get_sorted_battles\n"; }
    reduce_battles(a_battles, b_battles);
    if (dbg_flags & 0x1) { cerr << "t=" << dtr(t0) << " reduce_battles\n"; }
    const u_t ncombs = a_battles.size();
    a_comb_wins = vu_t(ncombs, 0);
    vau2_t b_pt2s;
    b_pt2s.reserve(b_battles.size());
    for (const au3_t& b_pt3: b_battles)
    {
        b_pt2s.push_back(au2_t{b_pt3[0], b_pt3[1]});
    }
    if (dbg_flags & 0x1) { cerr << "t="<< dtr(t0) << " set b_pt2s\n"; }
    for (u_t pi: {0, 1, 2})
    {
        vau2_t a_pt2s; a_pt2s.reserve(ncombs);
        for (const au3_t& a_pt3: a_battles)
        {
            const au2_t a_pt2{a_pt3[(pi + 0) % 3], a_pt3[(pi + 1) % 3]};
            a_pt2s.push_back(a_pt2);
        }
        CDQ2 cdq2(a_pt2s, b_pt2s);
        for (size_t wi = 0; wi < ncombs; ++wi)
        {
            u_t nw = cdq2.n_below[wi];
            a_comb_wins[wi] += nw;
        }
    }
    if (dbg_flags & 0x1) { cerr << "t=" << dtr(t0) << " of 3 CDQ2\n"; }
    CDQ cdq(a_battles, b_battles);
    if (dbg_flags & 0x1) { cerr << "t=" << dtr(t0) << " CDQ (3D)\n"; }
    for (size_t wi = 0; wi < ncombs; ++wi)
    {
        u_t nw = cdq.n_below[wi];
        a_comb_wins[wi] -= 2*nw;
    }
    if (dbg_flags & 0x1) { cerr << "t=" << dtr(t0) << " -2*nw\n"; }
    u_t wi_best = 0;
    u_t max_win = 0;
    for (size_t wi = 0; wi < ncombs; ++wi)
    {
        u_t nw = a_comb_wins[wi];
        if (max_win < nw)
        {
            max_win = nw;
            wi_best = wi;
        }
    }
    if (dbg_flags & 0x1) { cerr << "t=" << dtr(t0) << " find max_win\n"; }
    solution = double(max_win) / double(ncombs);
    if (dbg_flags) {
        const au3_t wb = a_battles[wi_best];
        cerr << "win[" << wi_best << "]: (" <<
            wb[0] << ' ' << wb[1] << ' ' << wb[2] << ") = " << max_win << '\n';
    }
}

class BIT2
{
 public:
    BIT2(u_t _max_idx_x, u_t _max_idx_y) :
        max_idx_x(_max_idx_x),
        max_idx_y(_max_idx_y),
        tree((_max_idx_x + 1)*(_max_idx_y + 1), 0) {}
    void update(int idx_x, int idx_y, u_t delta)
    {
        if ((idx_x > 0) && (idx_y > 0))
        {
            while (idx_x <= int(max_idx_x))
            {
                int y = idx_y;
                while (y <= int(max_idx_y))
                {
                    tree[ixy(idx_x, y)] += delta;
                    y += (y & -y);
                }
                idx_x += (idx_x & -idx_x);
            }
        }
    }
    u_t query(int idx_x, int idx_y) const
    {
        u_t n = 0;
        idx_x = min<int>(idx_x, max_idx_x);
        idx_y = min<int>(idx_y, max_idx_y);
        while (idx_x > 0)
        {
            int y = idx_y;
            while (y > 0)
            {
                n += tree[ixy(idx_x, y)];
                y -= (y & -y); // removing low bit
            }
            idx_x -= (idx_x & -idx_x); // removing low bit
        }
        return n;
    }
    void zero_fill() { fill(tree.begin(), tree.end(), 0); }
 private:
    u_t ixy(u_t x, u_t y) const { return x*(max_idx_y + 1) + y; }
    u_t max_idx_x;
    u_t max_idx_y;
    vu_t tree;
};

void BoardGame::solve()
{
    vau3_t a_battles, b_battles;
    tp_t t0 = chrono::high_resolution_clock::now();
    get_sorted_battles(a_battles, a);
    get_sorted_battles(b_battles, b);
    if (dbg_flags & 0x1) { cerr << "t="<<dt(t0) << " 2 get_sorted_battles\n"; }
    reduce_battles(a_battles, b_battles);
    if (dbg_flags & 0x1) { cerr << "t=" << dtr(t0) << " reduce_battles\n"; }
    const u_t b_max = b_battles.back()[0];
    BIT2 bit2b(b_max, b_max);
    if (dbg_flags & 0x1) { cerr << "t="<< dtr(t0) << " BIT2::ctor\n"; }
    for (const au3_t& b_pt3: b_battles)
    {
        bit2b.update(b_pt3[0], b_pt3[1], 1);
    }
    if (dbg_flags & 0x1) { cerr << "t="<< dtr(t0) << " Fill B\n"; }
    const u_t ncombs = a_battles.size();
    a_comb_wins = vu_t(ncombs, 0);
    for (size_t wi = 0; wi < ncombs; ++wi)
    {
        const au3_t& a_pt3 = a_battles[wi];
        for (u_t pi: {0, 1, 2})
        {
            const au2_t a_pt2{a_pt3[(pi + 0) % 3] - 1, a_pt3[(pi + 1) % 3] - 1};
            u_t nw = bit2b.query(a_pt2[0], a_pt2[1]);
            a_comb_wins[wi] += nw;
        }
    }
    if (dbg_flags & 0x1) { cerr << "t="<< dtr(t0) << " Get 3x2 Wins\n"; }
    bit2b.zero_fill();
    if (dbg_flags & 0x1) { cerr << "t="<< dtr(t0) << " Zero-Fill\n"; }
    for (size_t wi = 0, bi = 0; wi < ncombs; ++wi)
    {
        const au3_t& a_pt3 = a_battles[wi];
        for (; (bi < ncombs) && (b_battles[bi][0] < a_pt3[0]); ++bi)
        {
            const au3_t& b_pt3 = b_battles[bi];
            bit2b.update(b_pt3[1], b_pt3[2], 1);
        }
        u_t nw = bit2b.query(a_pt3[1] - 1, a_pt3[2] - 1);
        a_comb_wins[wi] -= 2*nw;
    }
    if (dbg_flags & 0x1) { cerr << "t="<< dtr(t0) << " subtract all 3 Wins\n"; }
    u_t wi_best = 0;
    u_t max_win = 0;
    for (size_t wi = 0; wi < ncombs; ++wi)
    {
        u_t nw = a_comb_wins[wi];
        if (max_win < nw)
        {
            max_win = nw;
            wi_best = wi;
        }
    }
    if (dbg_flags & 0x1) { cerr << "t="<< dtr(t0) << " final max(wins)\n"; }
    solution = double(max_win) / double(ncombs);
    if (dbg_flags) {
        const au3_t wb = a_battles[wi_best];
        cerr << "win[" << wi_best << "]: (" <<
            wb[0] << ' ' << wb[1] << ' ' << wb[2] << ") = " << max_win << '\n';
    }
}

void BoardGame::get_sorted_battles(vau3_t& battles, const vu_t& army) const
{
    typedef map<u_t, vt0vt1_t> u2combs_t;
    static u2combs_t sz_to_th01combs;
    vu_t sarmy(army);
    sort(sarmy.begin(), sarmy.end());
    const u_t sz = sarmy.size();
    u2combs_t::const_iterator iter = sz_to_th01combs.find(sz);
    if (iter == sz_to_th01combs.end())
    {
        vt0vt1_t th01combs;
        tp_t t0 = chrono::high_resolution_clock::now();
        get_comb_2thirds(th01combs, sz);
        if (dbg_flags & 0x1) { 
             cerr << "t="<<dt(t0) << " get_comb_2thirds(" << sz << ")\n"; }
        iter = sz_to_th01combs.insert(sz_to_th01combs.end(), 
            u2combs_t::value_type(sz, th01combs));
    }
    const vt0vt1_t& t01combs = iter->second;
    const size_t n0 = t01combs.size();
    const u_t sarmy_total = accumulate(sarmy.begin(), sarmy.end(), 0);
    typedef pair<u_t, size_t> usz_t;
    vector<usz_t> t0idx; t0idx.reserve(n0);
    for (size_t i = 0; i < n0; ++i)
    {
        const vu_t& t0 = t01combs[i].third0;
        u_t battle0 = 0;
        for (size_t j = 0, je = t0.size(); j < je; ++j)
        {
            battle0 += sarmy[t0[j]];
        }
        t0idx.push_back(usz_t(battle0, i));
    }
    sort(t0idx.begin(), t0idx.end());
    for (size_t i = 0, i1 = 0; i < n0; i = i1)
    {
        const u_t battle0 = t0idx[i].first;
        vu_t battle1s;
        for (i1 = i; (i1 < n0) && (t0idx[i1].first == battle0); ++i1)
        {
            for (const vu_t& third1: t01combs[t0idx[i1].second].third1s)
            {
                u_t battle1 = 0;
                for (size_t j = 0, je = third1.size(); j < je; ++j)
                {
                    battle1 += sarmy[third1[j]];
                }
                battle1s.push_back(battle1);
            }
        }
        sort(battle1s.begin(), battle1s.end());
        for (const u_t battle1: battle1s)
        {
            const u_t battle2 = sarmy_total - (battle0 + battle1);
            battles.push_back(au3_t{battle0, battle1, battle2});
        }
    }
}

void BoardGame::solve_seg2d()
{
    a_total = b_total = 0;
    for (u_t i = 0; i != 3*n; ++i)
    {
        a_total += a[i];
        b_total += b[i];
    }
    tp_t t0 = chrono::high_resolution_clock::now();
    if (dbg_flags & 0x1) { cerr << "{ compute a_combs, b_total\n"; }
    compute_g2thirds(a_combs, a);
    compute_g2thirds(b_combs, b);
    b_inc.reserve(b_combs.size());
    b_inc.push_back(b_combs[0].b0);
    for (const G2Thirds& b_comb: b_combs)
    {
        if (b_inc.back() != b_comb.b0)
        {
            b_inc.push_back(b_comb.b0);
        }
    }
    const size_t b_inc_sz = b_inc.size();
    
    g2thirds_to_a2inc(a_inc, a_combs, a_total);
    const u_t n_subcombs = a_combs[0].b1s.size();
    const u_t ncombs = a_combs.size() * n_subcombs;
    a_comb_wins = vu_t(a_inc.size(), 0);
    if (dbg_flags & 0x1) { 
        cerr << "} t=" << dt(t0) << " compute a_combs, b_total done\n"; }
    if (dbg_flags & 0x1) { cerr << "t="<<dt(t0) << ", SegTree.init ...\n"; }
    SegTree2D b_segtree(b_inc_sz, b_inc_sz);
    for (const G2Thirds& b_comb: b_combs)
    {
        size_t bi0 = get_index(b_inc, b_comb.b0);
        for (u_t b1: b_comb.b1s)
        {
            size_t bi1 = get_index(b_inc, b1);
            b_segtree.add(bi0, bi1, 1);
        }
    }
    if (dbg_flags & 0x1) { cerr << "t="<<dt(t0) << "{ get 2-wins\n"; }
    for (size_t ai = 0, asz = a_inc.size(); ai < asz; ++ai)
    {
        const au2_t low = a_inc[ai];
        const u_t a3[3] = {low[0], low[1], a_total - (low[0] + low[1])};
        size_t a3idx[3];
        for (u_t i = 0; i != 3; ++i)
        {
            a3idx[i] = n_below(b_inc, a3[i]);
        }
        for (u_t i = 0; i != 3; ++i)
        {
            const size_t a_pt[2] = {a3idx[(i + 1) % 3], a3idx[(i + 2) % 3]};
            u_t nwi = b_segtree.query(0, a_pt[0], 0, a_pt[1]);
            a_comb_wins[ai] += nwi;
        }
    }
    if (dbg_flags & 0x1) { cerr << "t="<<dt(t0) << "}get 2-wins\n"; }
    if (dbg_flags & 0x1) { cerr << "t="<<dt(t0) << " { subtract_all3_wins\n"; }
    subtract_all3_wins();
    if (dbg_flags & 0x1) { cerr << "t="<<dt(t0) << " } subtract_all3_wins\n"; }
    vu_t::const_iterator witer =
        max_element(a_comb_wins.begin(), a_comb_wins.end());
    u_t max_wins = *witer;
    if (dbg_flags) {
        u_t wi = witer - a_comb_wins.begin();
        const G2Thirds& a_comb = a_combs[wi / n_subcombs];
        u_t a0 = a_comb.b0;
        u_t a1 = a_comb.b1s[wi % n_subcombs];
        u_t a2 = a_total - (a0 + a1);
        cerr << "win["<<wi<<"]: ("<<a0 << ' ' << a1 << ' ' << a2 << ")\n";
    }
    solution = double(max_wins)/double(ncombs);
}

void BoardGame::subtract_all3_wins()
{
    tp_t t0 = chrono::high_resolution_clock::now();
    if (dbg_flags & 0x1) { cerr << "t="<<dt(t0) << " { all3 init_leaves\n"; }
    const size_t b_inc_sz = b_inc.size();
    SegTree2D b_segtree(b_inc_sz, b_inc_sz);
    if (dbg_flags & 0x1) { cerr << "t="<<dt(t0) << " } all3 init_leaves\n"; }
    
    const u_t n_subcombs = a_combs[0].b1s.size();
    const u_t n_gcombs = a_combs.size();
    const u_t a_sz = a_inc.size();
    for (u_t bgi = 0, ai = 0; (bgi != n_gcombs) && (ai < a_sz); )
    {
        const u_t b0_next = b_combs[bgi].b0;
        for ( ; (ai != a_sz) && (a_inc[ai][0] <= b0_next); ++ai) 
        {}
        const u_t a0_next = (ai < a_sz ? a_inc[ai][0] : 0);
        for (; (bgi != n_gcombs) && (b_combs[bgi].b0 < a0_next); ++bgi)
        {
            const G2Thirds& g = b_combs[bgi];
            const u_t b0 = g.b0;
            const vu_t& b1s = g.b1s;
            for (u_t bi = 0; bi < n_subcombs; ++bi)
            {
                size_t pt_idx0 = get_index(b_inc, b1s[bi]);
                size_t pt_idx1 = get_index(b_inc, b_total - (b0 + b1s[bi]));
                b_segtree.add(pt_idx0, pt_idx1, 1);
            }
        }
        const u_t a_bound = (bgi < n_gcombs ? b_combs[bgi].b0 : a_total);
        for (; (ai != a_sz) && (a_inc[ai][0] <= a_bound); ++ai) 
        {
            const au2_t ainc_ai = a_inc[ai];
            size_t pt_idx0 = n_below(b_inc, ainc_ai[1]);
            size_t pt_idx1 = n_below(b_inc, a_total - (ainc_ai[0] + ainc_ai[1]));
            u_t nw3 = b_segtree.query(0, pt_idx0, 0, pt_idx1);
            a_comb_wins[ai] -= 2*nw3;
        }
    }
    if (dbg_flags & 0x1) { cerr << "t="<<dt(t0) << " EOF subtract_all3_wins\n";}
}

void BoardGame::reduce_battles(vau3_t& a_battles, vau3_t& b_battles) const
{
    vu_t a_values, b_values, values;
    for (u_t i: {0, 1})
    {
        const vau3_t& battles = (i == 0 ? a_battles : b_battles);
        vu_t& x = (i == 0 ? a_values : b_values);
        x.push_back(battles[0][0]);
        for (const au3_t& battle: battles)
        {
            if (x.back() != battle[0])
            {
                x.push_back(battle[0]);
            }
        }
    }
    size_t ia = 0, ib = 0;
    const size_t ae = a_values.size(), be = b_values.size(); 
    while ((ia != ae) && (ib != be)) // unique sort merge
    {
        if (a_values[ia] < b_values[ib])
        {
            values.push_back(a_values[ia++]);
        }
        else if (b_values[ib] < a_values[ia])
        {
            values.push_back(b_values[ib++]);
        }
        else
        {
            values.push_back(a_values[ia]);
            ++ia;
            ++ib;
        }
    }
    values.insert(values.end(), a_values.begin() + ia, a_values.end());
    values.insert(values.end(), b_values.begin() + ib, b_values.end());
    for (u_t i: {0, 1})
    {
        vau3_t& battles = (i == 0 ? a_battles : b_battles);
        for (au3_t& battle: battles)
        {
            for (u_t j: {0, 1, 2})
            {
                u_t vold = battle[j];
                size_t pos = lower_bound(values.begin(), values.end(), vold) -
                    values.begin();
                if (values[pos] != vold)
                {
                    cerr << "Algorithm error @ " << __LINE__ << '\n';
                    exit(1);
                }
                battle[j] = pos + 1; // we do not want zeros.
            }
        }
    }
}

void BoardGame::print_solution(ostream &fo) const
{
    ostringstream os;
    os.precision(16);
    os.setf(ios::fixed);
    os << solution;
    string s = os.str();
    while (s.back() == '0')
    {
        s.pop_back();
    }
    fo << ' ' << s;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    void (BoardGame::*psolve)() = &BoardGame::solve;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            psolve = &BoardGame::solve_naive;
        }
        else if (opt == string("-seg2d"))
        {
            psolve = &BoardGame::solve_seg2d;
        }
        else if (opt == string("-cdq2"))
        {
            psolve = &BoardGame::solve_cdq2;
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

    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        BoardGame boardgame(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (boardgame.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        boardgame.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
