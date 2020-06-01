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
typedef array<u_t, 3> au3_t;
typedef array<au3_t, 3> a3au3_t;
typedef vector<a3au3_t> va3au3_t;

static unsigned dbg_flags;

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
    u_t n;
    vu_t a;
    vu_t b;
    double solution;
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

void BoardGame::solve()
{
    // solve_naive();
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
