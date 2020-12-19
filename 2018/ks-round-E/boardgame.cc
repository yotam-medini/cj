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

double dt(tp_t t0)
{
    tp_t t1 = chrono::high_resolution_clock::now();
    duration_t ddt = chrono::duration_cast<duration_t>(t1 - t0);
    double ret = ddt.count();
    return ret;
}

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;
typedef vector<u_t> vu_t;
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
    const u_t tsz = sz/3;
    // vu_t c0;
    T0VT1 t0vt1;
    vu_t& c0 = t0vt1.third0;
    vu_t rest; 
    rest.reserve(2*tsz);
    combination_first(c0, sz, tsz);
    vu_t c1; c1.reserve(tsz);
    vu_t c1i; c1i.reserve(tsz);
    for (bool c0_more = true; c0_more; c0_more = combination_next(c0, sz))
    {
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
        combination_first(c1i, 2*tsz, tsz);
        for (bool c1_more = true; c1_more;
            c1_more = combination_next(c1i, 2*tsz))
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

void compute_2thirds(vau2_t& r, const vu_t a)
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
    const vt0vt1_t& t91combs = iter->second;
    r.clear();
    au2_t s01;
    for (const T0VT1& t0vt1: t91combs)
    {
        s01[0] = 0;
        for (u_t i: t0vt1.third0)
        {
            s01[0] += a[i];
        }
        for (const vu_t c1: t0vt1.third1s)
        {
            s01[1] = 0;
            for (u_t i: c1)
            {
                s01[1] += a[i];
            }
            r.push_back(s01);
        }
    }
    sort(r.begin(), r.end());
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
    bool lt = (g0.b0 < g1.b0) || 
        ((g0.b0 == g1.b0) && (g0.b1s < g1.b1s));
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
    const vt0vt1_t& t91combs = iter->second;
    r.clear(); r.reserve(t91combs.size());
    for (const T0VT1& t0vt1: t91combs)
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
            sort(g.b1s.begin(), g.b1s.end());
        }
        r.push_back(g);
    }
    sort(r.begin(), r.end());
}

class SegTree1D
{
 public:
    SegTree1D(u_t pb=0, const vu_t& b=vu_t()); // b strict;y increasing
    void add_pts(const vu_t& pts)
    {
       for (const u_t pt: pts) { add_pt(pt); }
    }
    void add_pt(const u_t pt);
    ull_t n_gt(u_t pt) const { return n_gt(pt, bounds.size() - 1, 0); }
    const vu_t& b1s() const { return bounds[0]; }
    u_t parent_bound;
 private:
    ull_t n_gt(u_t pt, size_t bi, size_t bii) const;
    vvu_t bounds;
    vvu_t counts;
};
typedef vector<SegTree1D> vst1d_t;
typedef vector<vst1d_t> vvst1d_t;

SegTree1D::SegTree1D(u_t pb, const vu_t& b) :
    parent_bound(pb)
{
    bounds.push_back(b);
    counts.push_back(vu_t(b.size(), 0));
    while (bounds.back().size() > 1)
    {
        const vu_t& bb = bounds.back();
        const u_t bsz = bb.size();
        vu_t next; next.reserve((bsz + 1)/2);
        for (size_t i = 1; i < bsz; i += 2)
        {
            next.push_back(bb[i]);
        }
        if (next.back() != bb.back())
        {
            next.push_back(bb.back());
        }
        bounds.push_back(next);
        counts.push_back(vu_t(next.size(), 0));
    }
}

void SegTree1D::add_pt(const u_t pt)
{
    u_t i = lower_bound(bounds[0].begin(), bounds[0].end(), pt) - 
        bounds[0].begin();
    for (vu_t& cnts: counts)
    {
        ++cnts[i];
        i /= 2;
    }
}

ull_t SegTree1D::n_gt(u_t pt, size_t bi, size_t bii) const
{
    ull_t n = 0;
    const vu_t& cbi = counts[bi];
    if (bii < cbi.size())
    {
        ull_t count = cbi[bii];
        if (count > 0)
        {
            if (pt > bounds[bi][bii])
            {
                n = count;
            }
            else if (bi > 0)
            {
                n = n_gt(pt, bi - 1, 2*bii);
                n += n_gt(pt, bi - 1, 2*bii + 1);
            }
        }
    }
    return n;
}

class SegTree2D
{
 public:
    void init(const vg2thirds_t& b_combs);
    void add_pts(const G2Thirds& b_comb);
    void add_pt(const au2_t& pt);
    ull_t n_gt(const au2_t& pt) const
    {
        return n_gt(pt, trees1d.size() - 1, 0);
    }
 private:
    ull_t n_gt(const au2_t& pt, size_t ti, size_t tii) const;
    vvst1d_t trees1d;
};

void SegTree2D::init(const vg2thirds_t& b_combs)
{
    trees1d.reserve(b_combs.size());
    trees1d.push_back(vst1d_t());
    vst1d_t& t1ds0 = trees1d.back();
    const size_t nc = b_combs.size();
    for (size_t ci = 0, ce = 1; ci < nc; ci = ce)
    {
        const u_t b0 = b_combs[ci].b0;
        setu_t b1s_set;
        for (ce = ci; (ce < nc) && (b_combs[ce].b0 == b0); ++ce)
        {
            for (u_t b1: b_combs[ce].b1s)
            {
                b1s_set.insert(b1);
            }
        }
        vu_t b1s(b1s_set.begin(), b1s_set.end());
        t1ds0.push_back(SegTree1D(b0, b1s));
    }
    while (trees1d.back().size() > 1)
    {
        const vst1d_t& back = trees1d.back();
        const size_t sz = back.size();
        vst1d_t next; next.reserve((sz + 1)/2);
        for (size_t i = 1; i < sz; i += 2)
        {
            // merge back[i-1] + back[i+1];
            const SegTree1D& tree0 = back[i - 1];
            const SegTree1D& tree1 = back[i];
            const vu_t& b1s0 = tree0.b1s();
            const vu_t& b1s1 = tree1.b1s();
            vu_t b1s;
            merge(b1s0.begin(), b1s0.end(), b1s1.begin(), b1s1.end(),
                std::back_inserter(b1s));
            next.push_back(SegTree1D(tree1.parent_bound, b1s));
        }
        if (sz % 2 == 1)
        {
            next.push_back(back.back());
        }
        trees1d.push_back(next);
    }
}

void SegTree2D::add_pts(const G2Thirds& b_comb)
{
    const vst1d_t& trees0 = trees1d[0];
    auto iter = lower_bound(trees0.begin(), trees0.end(), b_comb.b0,
        [](const SegTree1D& t, u_t b0) -> bool
        {
            bool lt = t.parent_bound < b0;
            return lt;
        });
    u_t i = iter - trees0.begin();
    for (vst1d_t& trees: trees1d)
    {
        trees[i].add_pts(b_comb.b1s);
        i /= 2;
    }
}

void SegTree2D::add_pt(const au2_t& pt)
{
    const vst1d_t& trees0 = trees1d[0];
    auto iter = lower_bound(trees0.begin(), trees0.end(), pt[0],
        [](const SegTree1D& t, u_t b0) -> bool
        {
            bool lt = t.parent_bound < b0;
            return lt;
        });
    u_t i = iter - trees0.begin();
    for (vst1d_t& trees: trees1d)
    {
        trees[i].add_pt(pt[1]);
        i /= 2;
    }
}

ull_t SegTree2D::n_gt(const au2_t& pt, size_t ti, size_t tii) const
{
    ull_t n = 0;
    const vst1d_t& tree = trees1d[ti];
    if (tii < tree.size())
    {
        const SegTree1D& tree1d = tree[tii];
        if (pt[0] > tree1d.parent_bound)
        {
            n = tree1d.n_gt(pt[1]);
        }
        else if (ti > 0)
        {
            n = n_gt(pt, ti - 1, 2*tii);
            n += n_gt(pt, ti - 1, 2*tii + 1);
        }
    }
    return n;
}

class BoardGame
{
 public:
    BoardGame(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void set2(a3au2_t& gset, const a3au2_t& ig_set, const vu_t& cards) const;
    void set3(a3au3_t& gset, const a3au3_t& ig_set, const vu_t& cards) const;
    bool win2(const a3au2_t& a_set, const a3au2_t& b_set, bool& all) const;
    bool win(const a3au3_t& a_set, const a3au3_t& b_set) const;
    u_t lutn_sum(const vu_t& v, const vu_t& lut, u_t nsz) const;
    void subtract_all3_wins();
    u_t n;
    vu_t a;
    vu_t b;
    double solution;
    u_t b_third_max;
    u_t a_total;
    u_t b_total;
    // vau2_t a_combs, b_combs;
    vg2thirds_t a_combs, b_combs;
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

void BoardGame::solve()
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
    const u_t n_subcombs = a_combs[0].b1s.size();
    const u_t ncombs = a_combs.size() * n_subcombs;
    a_comb_wins = vu_t(ncombs, 0);
    if (dbg_flags & 0x1) { 
        cerr << "} t=" << dt(t0) << " compute a_combs, b_total done\n"; }
    if (dbg_flags & 0x1) { cerr << "t="<<dt(t0) << ", SegTree.init ...\n"; }
    SegTree2D b_segtree;
    b_segtree.init(b_combs);
    if (dbg_flags & 0x1) { cerr << "t="<<dt(t0) << ", add segments ...\n"; }
    for (const G2Thirds& b_comb: b_combs)
    {
        b_segtree.add_pts(b_comb);
    }   
    if (dbg_flags & 0x1) { cerr << "t="<<dt(t0) << ", get 2-wins\n"; }
    u_t ai = 0;
    for (const G2Thirds& a_comb: a_combs)
    {
        for (u_t i1 = 0; i1 < n_subcombs; ++i1, ++ai)
        {
            if ((dbg_flags & 0x2) && ((ai & (ai - 1)) == 0)) {
                cerr << "A-combs: " << ai << '/' << ncombs << '\n'; }
            const u_t a3[3] = {a_comb.b0, a_comb.b1s[i1],
                a_total - (a_comb.b0 + a_comb.b1s[i1])};
            for (u_t i = 0; i != 3; ++i)
            {
                const au2_t a_pt{{a3[(i + 1) % 3], a3[(i + 2) % 3]}};
                a_comb_wins[ai] += b_segtree.n_gt(a_pt);
            }
        }
    }
    if (dbg_flags & 0x1) { cerr << "t="<<dt(t0) << ", subtract_all3_wins\n"; }
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
    if (dbg_flags & 0x1) { cerr << "t="<<dt(t0) << ", all3 init_leaves\n"; }
    SegTree2D b_segtree;
    b_segtree.init(b_combs);
    if (dbg_flags & 0x1) { cerr << "t="<<dt(t0) << " } all3 init_leaves\n"; }
    
    const u_t n_subcombs = a_combs[0].b1s.size();
    const u_t n_gcombs = a_combs.size();
    u_t ai = 0, agi = 0;
    u_t b0 = b_combs[0].b0;
    for ( ; (agi != n_gcombs) && (a_combs[agi].b0 <= b0); 
        ++agi, ai += n_subcombs)
    {}
    for (u_t bgi = 0; bgi != n_gcombs; )
    {
        for (; (bgi != n_gcombs) && (b_combs[bgi].b0 == b0); ++bgi)
        {
            const G2Thirds& g = b_combs[bgi];
            const vu_t& b1s = g.b1s;
            for (u_t bi = 0; bi < n_subcombs; ++bi)
            {
                const au2_t pt{b1s[bi], b_total - (b0 + b1s[bi])};
                b_segtree.add_pt(pt);
            }
        }
        u_t b0_next = (bgi < n_gcombs ? b_combs[bgi].b0 : a_total);
        for (; (agi != n_gcombs) && 
            (b0 < a_combs[agi].b0) && (a_combs[agi].b0 <= b0_next); ++agi)
        {
            const G2Thirds& g = a_combs[agi];
            const vu_t& b1s = g.b1s;
            for (u_t bi = 0; bi < n_subcombs; ++bi, ++ai)
            {
                const au2_t a_pt{b1s[bi], a_total - (g.b0 + b1s[bi])};
                u_t nw3 = b_segtree.n_gt(a_pt);
                a_comb_wins[ai] -= 2*nw3;
            }
        }
        b0 = b0_next;
    }
    if (dbg_flags & 0x1) { cerr << "t="<<dt(t0) << " EOF subtract_all3_wins\n";}
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

    void (BoardGame::*psolve)() =
        (naive ? &BoardGame::solve_naive : &BoardGame::solve);
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
