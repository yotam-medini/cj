// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
// #include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef array<u_t, 3> au3_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;
typedef array<au2_t, 3> a3au2_t;
typedef vector<a3au2_t> va3au2_t;
typedef array<au3_t, 3> a3au3_t;
typedef vector<a3au3_t> va3au3_t;

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

class TComb
{
 public:
    TComb(const vu_t& m=vu_t(), const vvu_t& s=vvu_t()) : main(m), subs(s) {}
    vu_t main;
    vvu_t subs;
};
typedef vector<TComb> vtcomb_t;

static vtcomb_t tcomb2345[4];

static void compute_tcomb(vtcomb_t& tcombN, u_t n)
{
    vu_t iinc(size_t(3*n), 0);
    iota(iinc.begin(), iinc.end(), 0); // now like a const;
    vu_t head;
    combination_first(head, 3*n, n);
    for (bool hmore = true; hmore; hmore = combination_next(head, 3*n))
    {
        vu_t tc(iinc);
        for (u_t i = 0; i != n; ++i)
        {
            swap(tc[i], tc[head[i]]);
        }
        vu_t thc(tc.begin(), tc.begin() + n);
        sort(tc.begin() + n, tc.end());
        TComb tcomb(thc);
        vu_t mid;
        combination_first(mid, 2*n, n);
        for (bool mmore = true; mmore; mmore = combination_next(mid, 2*n))
        {
            vu_t sub;
            sub.reserve(n);
            for (u_t i = 0; i != n; ++i)
            {
                sub.push_back(tc[n + mid[i]]);
            }
            tcomb.subs.push_back(sub);
        }
        tcombN.push_back(tcomb);
    }
}

static void compute_tcombs()
{
    if (tcomb2345[0].empty())
    {
        for (u_t n = 2; n <= 5; ++n)
        {
            compute_tcomb(tcomb2345[n - 2], n);
        }
    }
}

class Sum12
{
 public:
    Sum12(u_t f=0, const vu_t& s=vu_t()) : first(f), seconds(s) {}
    vu_t::const_iterator lb(vu_t::const_iterator e, u_t b, u_t m) const
    {
        return b > m ? lower_bound(seconds.begin(), e, b - m) : seconds.begin();
    }
    u_t first;
    vu_t seconds;
};
typedef vector<Sum12> vsum12_t;
bool operator<(const Sum12& sm0, const Sum12& sm1)
{
    bool lt = (sm0.first < sm1.first) ||
        ((sm0.first == sm1.first) && (sm0.seconds < sm1.seconds));
    return lt;
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
    void compute_b_thirds();
    ull_t compute_wins(const u_t a0, const u_t a1) const;
    u_t lutn_sum(const vu_t& v, const vu_t& lut, u_t nsz) const;
    void compute_b_expectation_inc();
    u_t n;
    vu_t a;
    vu_t b;
    double solution;
    vsum12_t b_thirds;
    u_t a_total;
    u_t b_total;
    ll_t b_expectation[3];
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

void BoardGame::solve()
{
    compute_tcombs();
    compute_b_thirds();
    a_total = b_total = 0;
    for (u_t i = 0; i != 3*n; ++i)
    {
        a_total += a[i];
        b_total += b[i];
    }
    vu_t best_comb;
    ull_t max_wins = 0;
    const vtcomb_t& tcombs = tcomb2345[n - 2];
    for (u_t ci = 0, nc = tcombs.size(); ci != nc; ++ci)
    {
        if (dbg_flags & 0x1) { cerr << ci << '/' << nc << " tcombs\n"; }
        const TComb& tcomb = tcombs[ci];
        u_t a0 = lutn_sum(a, tcomb.main, n);
        for (const vu_t& sub: tcomb.subs)
        {
            u_t a1 = lutn_sum(a, sub, n);
            u_t a2 = a_total - (a0 + a1);
            if ((a0 <= a1) && (a1 <= a2))
            {
                ull_t n_wins = compute_wins(a0, a1);
                if (max_wins < n_wins)
                {
                    max_wins = n_wins;
                }
            }
        }
    }
    u_t ncombs = tcombs.size() * tcombs[0].subs.size();
    solution = double(max_wins) / double(ncombs);
}

ull_t BoardGame::compute_wins(const u_t a0, const u_t a1) const
{
    ull_t wins = 0;
    ull_t sub = 0;
    const u_t a2 = a_total - (a0 + a1); // a0 <= a1 <= a2
    const u_t a_team[3] = {a0, a1, a2};
    if (dbg_flags & 0x2) { cerr << "a0="<<a0 << ", a1="<<a1 << '\n';}
    //// const au2_t a_pairs[3] = {{a0, a1}, {a0, a2}, {a1, a2}};
    // a1+a2 >= a0+a2 >= a0+a1
    // a1>b1 & a2>b2 ==> a1+a2 >= b1+b2+2 = (b_total-b0)+2
    //   => b0 >= b_total-(a1+a2)+2
    vsum12_t::const_iterator lb[4]; // increasing!
    vu_t lbo, bounds;
    const u_t b_total_p2 = b_total + 2;
    for (u_t i = 0; i != 3; ++i)
    {
        vsum12_t::const_iterator bb = (i == 0 ? b_thirds.begin() : lb[i - 1]);
        const u_t a_other = a_total - a_team[i];
        // u_t bound = b_total - (a_total - a_team[i]) + 2;
        const u_t bound = (b_total_p2 > a_other ? b_total_p2 - a_other : 0);
        lb[i] = lower_bound(bb, b_thirds.end(), bound,
            [](const Sum12& sum12, u_t rhs) -> bool
            {
                bool lt = sum12.first < rhs;
                return lt;
            });
        // b_thirds from and above lb[i] their seconds < a[jk] # ijk={0,1,2}
        // lb[0] : a[12], lb[1]: a[02]<=a[12], lb[2]: a[01]<=a[02]<=a[12]
        bounds.push_back(bound);
        lbo.push_back(lb[i] - b_thirds.begin());
    }
    lb[3] = b_thirds.end();
    lbo.push_back(b_thirds.size());
    if (dbg_flags & 0x1) { cerr << "bounds: "<<vu_to_str(bounds) <<
         ", lb: "<<vu_to_str(lbo) << '\n'; }

        // i=0: a0+a1, a0+a2, a1+a2
        // i=1: a0+a1, a0+a2
        // i=2: a0+a1

    set<Sum12> spit23;
    for (vsum12_t::const_iterator iter = lb[2]; iter != lb[3]; ++iter)
    {   // a[01], a[02], a[12]
        const Sum12& sum12 = *iter;
        u_t bpair = b_total - sum12.first;
        const vu_t& secs = sum12.seconds;

        const vu_t::const_iterator lb_a1 =
            lower_bound(secs.begin(), secs.end(), a1);
        const vu_t::const_iterator lb_a0r = sum12.lb(lb_a1, bpair + 1, a0);
        const u_t add_01 = lb_a1 - lb_a0r;
        const u_t sub_012 = (sum12.first < a2 ? add_01 : 0);
        sub += sub_012;
        wins += add_01;
        const vu_t::const_iterator lb_a2 =
            lower_bound(lb_a1, secs.end(), a2);
        const vu_t::const_iterator lb_a02r = sum12.lb(lb_a2, bpair + 1, a0);
        const vu_t::const_iterator lb_a1r = sum12.lb(lb_a2, bpair + 1, a1);
        const u_t add_12 = lb_a2 - lb_a1r;
        wins += add_12;
        const u_t add_02 = lb_a2 - lb_a02r;
        wins += add_02;
        if (dbg_flags & 0x2) { 
            if (spit23.find(sum12) == spit23.end()) {
                spit23.insert(spit23.end(), sum12);
                cerr << "[23] sum12: f="<<sum12.first << ", secs=" << 
                  vu_to_str(sum12.seconds) << ", sub_012="<<sub_012<<
                  ", add_01="<<add_01 << 
                  ", add_02="<<add_02 << ", add_12="<<add_12 << '\n';
            }
        }
    }

    set<Sum12> spit12;
    for (vsum12_t::const_iterator iter = lb[1]; iter != lb[2]; ++iter)
    {   // a[02], a[12]
        const Sum12& sum12 = *iter;
        u_t bpair = b_total - sum12.first;
        const vu_t& secs = sum12.seconds;

        const vu_t::const_iterator lb_a2 =
            lower_bound(secs.begin(), secs.end(), a2);
        const vu_t::const_iterator lb_a02r = sum12.lb(lb_a2, bpair + 1, a0);
        const vu_t::const_iterator lb_a1r = sum12.lb(lb_a2, bpair + 1, a1);
        const u_t add_12 = lb_a2 - lb_a1r;
        wins += add_12;
        const u_t add_02 = lb_a2 - lb_a02r;
        wins += add_02;
        
        if (dbg_flags & 0x2) { 
            if (spit23.find(sum12) == spit23.end()) {
                spit23.insert(spit23.end(), sum12);
                cerr << "[12] sum12: f="<<sum12.first << ", secs=" << 
                  vu_to_str(sum12.seconds) << 
                  ", add_02="<<add_02 << ", add_12="<<add_12 << '\n';
            }
        }
    }

    set<Sum12> spit01;
    for (vsum12_t::const_iterator iter = lb[0]; iter != lb[1]; ++iter)
    {   // a[12]
        const Sum12& sum12 = *iter;
        u_t bpair = b_total - sum12.first;
        const vu_t& secs = sum12.seconds;
        const vu_t::const_iterator lb_a2 =
            lower_bound(secs.begin(), secs.end(), a2);
        const vu_t::const_iterator lb_a1r = sum12.lb(lb_a2, bpair + 1, a1);
        const u_t add_12 = lb_a2 - lb_a1r;
        wins += add_12;
        if (dbg_flags & 0x2) { 
            if (spit23.find(sum12) == spit23.end()) {
                spit23.insert(spit23.end(), sum12);
                cerr << "[01] sum12: f="<<sum12.first << ", secs=" << 
                  vu_to_str(sum12.seconds) << ", add_12="<<add_12 << '\n';
            }
        }

    }

    wins -= 2*sub;

    return wins;
}

void BoardGame::compute_b_thirds()
{
    const vtcomb_t& tcombs = tcomb2345[n - 2];
    b_thirds.reserve(tcombs.size());
    for (const TComb& tcomb: tcombs)
    {
        b_thirds.push_back(Sum12());
        Sum12& sum12 = b_thirds.back();
        sum12.first = lutn_sum(b, tcomb.main, n);
        for (const vu_t& sub: tcomb.subs)
        {
            u_t second = lutn_sum(b, sub, n);
            sum12.seconds.push_back(second);
        }
        sort(sum12.seconds.begin(), sum12.seconds.end());
    }
    sort(b_thirds.begin(), b_thirds.end());
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

void BoardGame::compute_b_expectation_inc()
{
    fill_n(&b_expectation[0], 3, 0);
    const vtcomb_t& tcombs = tcomb2345[n - 2];
    for (u_t ci = 0, nc = tcombs.size(); ci != nc; ++ci)
    {
        ull_t bs[3];
        const TComb& tcomb = tcombs[ci];
        ull_t b0 = lutn_sum(a, tcomb.main, n);
        for (const vu_t& sub: tcomb.subs)
        {
            bs[0] = b0;
            bs[1] = lutn_sum(a, sub, n);
            bs[2] = b_total - (b[0] + b[1]);
            sort(&bs[0], &bs[0] + 3);
            for (u_t i = 0; i != 3; ++i)
            {
                b_expectation[i] += bs[i];
            }
        }
    }
}

void BoardGame::print_solution(ostream &fo) const
{
    fo << ' ' << setprecision(9) << solution;
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
