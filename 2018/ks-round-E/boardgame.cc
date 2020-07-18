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
#include <set>
#include <string>
#include <sstream>
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
typedef array<u_t, 2> au2_t;
typedef array<u_t, 3> au3_t;
typedef vector<au2_t> vau2_t;
typedef vector<au3_t> vau3_t;
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

class SegTree2DNode
{
 public:
    SegTree2DNode(u_t lb=0) : low_bound(lb), child{0, 0} {}
    u_t low_bound;
    SegTree2DNode* child[2];
};

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
    void compute_thirds(vsum12_t& thirds, const vu_t&);
    u_t lutn_sum(const vu_t& v, const vu_t& lut, u_t nsz) const;
    u_t n;
    vu_t a;
    vu_t b;
    double solution;
    vsum12_t a_thirds;
    vsum12_t b_thirds;
    u_t b_third_max;
    u_t a_total;
    u_t b_total;
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
    compute_thirds(a_thirds, a);
    compute_thirds(b_thirds, b);
    a_total = b_total = 0;
    for (u_t i = 0; i != 3*n; ++i)
    {
        a_total += a[i];
        b_total += b[i];
    }
#if 0
    const vtcomb_t& tcombs = tcomb2345[n - 2];
    const u_t sub_sz = tcombs[0].subs.size();
    const u_t sub_sz_half = sub_sz/2;
    au3_t best_team;
    ull_t max_wins = 0;
    const vsum12_t::const_iterator a_end = 
        (a_thirds.front().first < a_thirds.back().first
            ? lower_bound(a_thirds.begin(), a_thirds.end(), a_total/3 + 1)
            : a_thirds.begin() + 1);
    u_t a_third_sz = a_end - a_thirds.begin();
    for (u_t ai = 0; ai < a_third_sz; ++ai)
    {
        const Sum12& asum12 = a_thirds[ai];
        const u_t a0 = asum12.first;
        const vu_t& asecs = asum12.seconds;
        const u_t a12 = a_total - a0;
        u_t asi0 = 0;
        for (; (asi0 < sub_sz_half) && (asecs[asi0] < a0); ++asi0) {}
        for (u_t asi = asi0; asi < sub_sz_half; ++asi)
        {
            const u_t a1 = asum12.seconds[asi];
            const u_t a2 = a12 - a1;
            const au3_t ateam3{a0, a1, a2};
            u_t n_wins = 0;
            for (u_t exc = 0; exc != 3; ++exc)
            {
                au2_t ateam2{ateam3[(exc + 1) % 3], ateam3[(exc + 2) % 3]};
                u_t nw2 = bhb2_n_wins(bhb2, ateam2, 0, 0);
                n_wins += nw2;
            }
            u_t nw3 = bhb3_n_wins(bhb3, ateam3, 0, 0);
            n_wins -= 2*nw3;
            if (max_wins < n_wins)
            {
                max_wins = n_wins;
                best_team = ateam3;
            }
        }
    }
    if (dbg_flags & 0x1) {
        cerr << "max_wins="<<max_wins << ", best_team=["<<
        best_team[0] << ", "<< best_team[1] << ", "<< best_team[2] << "]\n"; 
    }
    u_t ncombs = tcombs.size() * tcombs[0].subs.size();
    solution = double(max_wins) / double(ncombs);
#endif
}

void BoardGame::compute_thirds(vsum12_t& thirds, const vu_t& x)
{
    const vtcomb_t& tcombs = tcomb2345[n - 2];
    thirds.reserve(tcombs.size());
    for (const TComb& tcomb: tcombs)
    {
        thirds.push_back(Sum12());
        Sum12& sum12 = thirds.back();
        sum12.first = lutn_sum(x, tcomb.main, n);
        for (const vu_t& sub: tcomb.subs)
        {
            u_t second = lutn_sum(x, sub, n);
            sum12.seconds.push_back(second);
        }
        sort(sum12.seconds.begin(), sum12.seconds.end());
    }
    sort(thirds.begin(), thirds.end());
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
