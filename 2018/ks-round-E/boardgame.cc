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
// #include "kdsegtree.h" // temporary - eventtully will merged in
#include "segtree2d.cc" // temporary - eventtully will merged in

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
    vvu_t  third1s;
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

void compute_2thirds_OLD(vau2_t& r, const vu_t a)
{
    const u_t sz = a.size();
    const u_t tsz = sz/3;
    vu_t c0;
    combination_first(c0, sz, tsz);
    vu_t a1i; a1i.reserve(2*tsz);
    for (bool c0_more = true; c0_more; c0_more = combination_next(c0, sz))
    {
        u_t sum0 = 0;
        a1i.clear();
        for (u_t ai = 0, ai0 = 0; ai != sz; ++ai)
        {
            if ((ai0 < tsz) && (ai == c0[ai0]))
            {
                sum0 += a[ai];
                ++ai0;
            }
            else
            {
                a1i.push_back(ai);
            }
        }
        vu_t c1;
        combination_first(c1, 2*tsz, tsz);
        for (bool c1_more = true; c1_more;
            c1_more = combination_next(c1, 2*tsz))
        {
            u_t sum1 = 0;
            for (u_t c1i: c1)
            {
                sum1 += a[a1i[c1i]];
            }
            au2_t s01{{sum0, sum1}};
            r.push_back(s01);
        }
    }
    sort(r.begin(), r.end());
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
    vau2_t a_combs, b_combs;
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
            }
        }
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

class SegTreeWrapper
{
 public:
    SegTreeWrapper() : debug_test(dbg_flags & 0x10) {}
    void init(const avu2_t& ends) { tree.init(ends); }
    void add(const au2_t& pt)
    {
        tree.add(pt);
        if (debug_test) { pts.push_back(pt); }
    }
    ull_t n_gt(const au2_t& pt) const
    {
        ull_t ret = tree.n_gt(pt);
        if (debug_test)
        {
            ull_t n_naive = n_gt_naive(pt, pts);
            if (ret != n_naive)
            {
                cerr << "Failed ...\n";
                special_msg(pt, pts);
            }
        }
        return ret;
    }
 private:
    bool debug_test;
    SegTree2D tree;
    vau2_t pts;
};

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
    compute_2thirds(a_combs, a);
    compute_2thirds(b_combs, b);
    if (dbg_flags & 0x1) { 
        cerr << "} t=" << dt(t0) << " compute a_combs, b_total done\n"; }
    const u_t ncombs = a_combs.size();
    SegTreeWrapper b_segtree[3];
    a_comb_wins = vu_t(ncombs, 0);
    for (u_t i = 0; i != 3; ++i)
    {
        const u_t j = (i + 1) % 3, k = (j + 1) % 3;
        SegTreeWrapper& bst = b_segtree[i];
        if (dbg_flags & 0x1) { 
            cerr << "t="<<dt(t0) << ", i="<<i << ", SegTree.init ...\n"; }
        setu_t xs, ys;
        for (const au2_t& bc: b_combs)
        {
            const u_t b3[3] = {bc[0], bc[1], b_total - (bc[0] + bc[1])};
            xs.insert(b3[j]);
            ys.insert(b3[k]);
        }
        avu2_t xy{vu_t(xs.begin(), xs.end()), vu_t(ys.begin(), ys.end())};
        bst.init(xy);
        if (dbg_flags & 0x1) { 
            cerr << "t="<<dt(t0) << ".  i="<<i << ", add segments ...\n"; }
        for (const au2_t& bc: b_combs)
        {
            const u_t b3[3] = {bc[0], bc[1], b_total - (bc[0] + bc[1])};
            au2_t pt{b3[j], b3[k]};
            bst.add(pt);
        }
    }
    
    if (dbg_flags & 0x1) { cerr << "t="<<dt(t0) << ", get 2-wins\n"; }
    for (u_t ai = 0; ai != ncombs; ++ai)
    {
        const au2_t& ac = a_combs[ai];
        const u_t a3[3] = {ac[0], ac[1], a_total - (ac[0] + ac[1])};
        for (u_t i = 0; i != 3; ++i)
        {
            const au2_t a_pt{{a3[(i + 1) % 3], a3[(i + 2) % 3]}};
            a_comb_wins[ai] += b_segtree[i].n_gt(a_pt);
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
        u_t a0 = a_combs[wi][0], a1 = a_combs[wi][1], a2 = a_total - (a0 + a1);
        cerr << "win["<<wi<<"]: ("<<a0 << ' ' << a1 << ' ' << a2 << ")\n";
    }
    solution = double(max_wins)/double(ncombs);
}

void BoardGame::subtract_all3_wins()
{
    tp_t t0 = chrono::high_resolution_clock::now();
    SegTree2D b_segtree;
    const u_t ncombs = b_combs.size(); // a_combs.size()

    setu_t xs, ys;
    for (const au2_t& bc: b_combs)
    {
        u_t bc2 = b_total - (bc[0] + bc[1]);
        xs.insert(bc[1]);
        ys.insert(bc2);
    }
    if (dbg_flags & 0x1) { cerr << "t="<<dt(t0) << ", all3 init_leaves\n"; }
    avu2_t xy{vu_t(xs.begin(), xs.end()), vu_t(ys.begin(), ys.end())};
    b_segtree.init(xy);

    if (dbg_flags & 0x1) { cerr << "t="<<dt(t0) << " } all3 init_leaves\n"; }
    
    u_t ai;
    u_t b0 = b_combs[0][0];
    for (ai = 0; (ai != ncombs) && (a_combs[ai][0] <= b0); ++ai) {}
    for (u_t bi = 0; bi != ncombs; )
    {
        for (; (bi != ncombs) && (b_combs[bi][0] == b0); ++bi)
        {
            const au2_t& bc = b_combs[bi];
            u_t bc2 = b_total - (bc[0] + bc[1]);
            const au2_t pt{bc[1], bc2};
            b_segtree.add(pt);
        }
        u_t b0_next = (bi < ncombs ? b_combs[bi][0] : a_total);
        for (; (ai != ncombs) && 
            (b0 < a_combs[ai][0]) && (a_combs[ai][0] <= b0_next); ++ai)
        {
            const au2_t& ac = a_combs[ai];
            const au2_t a_pt{{ac[1], a_total - (ac[0] + ac[1])}};
            u_t nw3 = b_segtree.n_gt(a_pt);
            a_comb_wins[ai] -= 2*nw3;
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
