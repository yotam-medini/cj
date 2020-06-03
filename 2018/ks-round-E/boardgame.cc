// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
// #include <map>
// #include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef array<u_t, 3> au3_t;
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

#if 0
static vvu_t third_comb3;
static vvu_t third_comb4;
static vvu_t third_comb5;
static vvu_t tricomb3;
static vvu_t tricomb4;
static vvu_t tricomb5;

static void compute_tricomb(vvu_t& thcs, vvu_t& tcs, const u_t n)
{
    // const n3 = 3*n;
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
        thcs.push_back(thc);
        sort(tc.begin() + n, tc.end());
        vu_t mid;
        combination_first(mid, 2*n, n);
        for (bool mmore = true; mmore; mmore = combination_next(mid, 2*n))
        {
            for (u_t i = 0; i != n; ++i)
            {
                swap(tc[n + i], tc[n + mid[i]]);
            }
            sort(tc.begin() + 2*n, tc.end());
            tcs.push_back(tc);
            size_t sz = tcs.size();
            if ((dbg_flags & 0x1) && ((sz & (sz - 1)) == 0)) {
                cerr << "tricomb"<<n << " ... sz=" << sz << '\n'; }
        }
    }
    if (dbg_flags & 0x1) { cerr << "tricomb"<<n << "sz="<<tcs.size() << '\n'; }
}

static void compute_tricombs()
{
    if (tricomb3.empty())
    {
        compute_tricomb(third_comb3, tricomb3, 3);
        compute_tricomb(third_comb4, tricomb4, 4);
        compute_tricomb(third_comb5, tricomb5, 5);
    }
}
#endif

class TComb
{
 public:
    TComb(const vu_t& m=vu_t(), const vvu_t& s=vvu_t()) : main(m), subs(s) {}
    vu_t main;
    vvu_t subs;
};
typedef vector<TComb> vtcomb_t;

static vtcomb_t tcomb345[3];

static void compute_tcomb(vtcomb_t& tcombN, u_t n)
{
    // const n3 = 3*n;
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
    if (tcomb345[0].empty())
    {
        for (u_t n = 3; n <= 5; ++n)
        {
            compute_tcomb(tcomb345[n - 3], n);
        }
    }
}

class Sum12
{
 public:
    Sum12(u_t f=0, const vu_t& s=vu_t()) : first(f), seconds(s) {}
    u_t first;
    vu_t seconds;
};
typedef vector<Sum12> vsum12_t;

class BoardGame
{
 public:
    BoardGame(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void set3(a3au3_t& gset, const a3au3_t& ig_set, const vu_t& cards) const;
    bool win(const a3au3_t& a_set, const a3au3_t& b_set) const;
    void compute_b_thirds();
    ull_t compute_wins(const u_t a1, const u_t a2) const;
    u_t lutn_sum(const vu_t& v, const vu_t& lut, u_t nsz) const;
    u_t n;
    vu_t a;
    vu_t b;
    // vu_t b_thirds;
    double solution;
    vsum12_t b_thirds;
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
    compute_choose_9_333();
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

#if 0
void BoardGame::solve()
{
    compute_tricombs();
    compute_b_thirds();
    const vvu_t* ptcs = (n == 3 ? &tricomb3 
        : (n == 4 ? &tricomb4 : &tricomb5));
    const vvu_t& tcs = *ptcs;
    const double dcomb = 1. / double(b_thirds.size());
    vu_t best_comb;
    for (const vu_t& comb: tcs)
    {
        double bp[3];
        for (u_t battle = 0; battle != 3; ++battle)
        {
            u_t asum = 0;
            for (u_t i = 0; i != n; ++i)
            {
                asum += a[comb[battle*n + i]];
            }
            vu_t::const_iterator lb =
                lower_bound(b_thirds.begin(), b_thirds.end(), asum);
            u_t wins = lb - b_thirds.begin();
            bp[battle] = double(wins) * dcomb;
        }
        double prob = bp[0]*bp[1] + bp[0]*bp[2] + bp[1]*bp[2] - 
            bp[0]*bp[1]*bp[2];
        if (solution < prob)
        {
            solution = prob;
            best_comb = comb;
        }
    }
    if (dbg_flags & 0x2)
    {
        cerr << "prob="<<solution << ", best: " << vu_to_str(best_comb) << '\n';
    }
}
#endif

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
    const vtcomb_t& tcombs = tcomb345[n - 3];
    for (const TComb& tcomb: tcombs)
    {
        u_t a_first = lutn_sum(a, tcomb.main, n);
        for (const vu_t& sub: tcomb.subs)
        {
            u_t a_second = lutn_sum(a, sub, n);
            ull_t n_wins = compute_wins(a_first, a_second);
            if (max_wins < n_wins)
            {
                max_wins = n_wins;
            }
        }
    }
    u_t ncombs = tcombs.size() * tcombs[0].subs.size();
    solution = double(max_wins) / double(ncombs);
}

void BoardGame::compute_b_thirds()
{
    const vtcomb_t& tcombs = tcomb345[n - 3];
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
}

ull_t BoardGame::compute_wins(const u_t a1, const u_t a2) const
{
    ull_t wins = 0;
    const u_t a3 = a_total - (a1 + a2);
    vsum12_t::const_iterator lb = lower_bound(
        b_thirds.begin(), b_thirds.end(), a1, 
        [](const Sum12& sum12, u_t rhs) -> bool
        {
            bool lt = sum12.first < rhs;
            return lt;
        });

    // itreate where A wins fitst battle
    for (vsum12_t::const_iterator iter = b_thirds.begin(); iter != lb; ++iter)
    {
        const u_t b1 = iter->first; // < a_first
        const vu_t& seconds = iter->seconds;
        vu_t::const_iterator lb2 = lower_bound(seconds.begin(), seconds.end(), 
            a2);
        // When a3 > b3 = b_total - (b1 + b2)
        // ==>  b2 > (b_total - b1) - a3
        u_t b23 = b_total - b1;
        vu_t::const_iterator lb3 = (b23 < a3 ? seconds.begin() 
           : upper_bound(seconds.begin(), seconds.end(), b23 - a3));
        u_t ilb2 = lb2 - seconds.begin();
        u_t ilb3 = lb3 - seconds.begin();
        if (dbg_flags & 0x4) {
           cerr << "ilb2=" << ilb2 << ", lb3=" << ilb3 << '\n'; }
        if (lb2 < lb3)
        {
            ull_t win2 = lb2 - seconds.begin();
            ull_t win3 = seconds.end() - lb3;
            wins += (win2 + win3);
        }
        else
        {
            wins += seconds.size();
        }
    }
    // itreate where B wins fitst battle
    for (vsum12_t::const_iterator iter = lb; iter != b_thirds.end(); ++iter)
    {
        const u_t b1 = iter->first; // >= a_first
        const vu_t& seconds = iter->seconds;
        vu_t::const_iterator lb2 = lower_bound(seconds.begin(), seconds.end(), 
            a2);
        // When a3 > b3 = b_total - (b1 + b2)
        // ==>  b2 > (b_total - b1) - a3
        u_t b23 = b_total - b1;
        vu_t::const_iterator lb3 = (b23 < a3 ? seconds.begin() 
           : upper_bound(seconds.begin(), seconds.end(), b23 - a3));
        u_t ilb2 = lb2 - seconds.begin();
        u_t ilb3 = lb3 - seconds.begin();
        if (dbg_flags & 0x4) {
           cerr << "ilb2=" << ilb2 << ", lb3=" << ilb3 << '\n'; }
        if (lb3 < lb2)
        {
            ull_t win23 = lb2 - lb3;
            wins += win23;
        }
    }

    return wins;
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

#if 0
void BoardGame::compute_b_thirds()
{
    const vvu_t* pthc = (n == 3 ? &third_comb3 
        : (n == 4 ? &third_comb4 : &third_comb5));
    const vvu_t& thc = *pthc;
    b_thirds.reserve(thc.size());
    for (const vu_t& comb: thc)
    {
        u_t sum = 0;
        for (u_t ci: comb)
        {
            sum += b[ci];
        }
        b_thirds.push_back(sum);
    }
    sort(b_thirds.begin(), b_thirds.end());
}
#endif

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
