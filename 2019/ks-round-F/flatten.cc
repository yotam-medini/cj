// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<u_t, 3> au3_t;
typedef set<u_t> setu_t;
typedef vector<u_t> vu_t;
typedef map<au3_t, u_t> au3_2u_t;
typedef map<ul_t, u_t> u2u_t;

static unsigned dbg_flags;

// caller should initialize t = vu_t(bound.size(), 0)
void tuple_next(vu_t& t, const vu_t& bound)
{
    u_t i, sz = bound.size();
    bool done = false;
    for (i = 0; (i < sz) && !done; ++i)
    {
        ++t[i];
        done = (t[i] < bound[i]);
        if (!done)
        {
            fill_n(t.begin(), i + 1, 0);
        }
    }
    if (!done)
    {
        t.clear();
    }
}

class Flatten
{
 public:
    Flatten(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t seg_change(u_t bi, u_t ei, u_t max_steps);
    u_t memo_seg_change(u_t bi, u_t ei, u_t max_steps);
    u_t n, k;
    u_t solution;
    vu_t a;
    vu_t h;
    // au3_2u_t memo;
    u2u_t memo;
};

Flatten::Flatten(istream& fi) : solution(0)
{
    fi >> n >> k;
    a.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        u_t x;
        fi >> x;
        a.push_back(x);
    }
}

void Flatten::solve_naive()
{
    setu_t heights(a.begin(), a.end());
    u_t nh = heights.size();
    vu_t i2h(heights.begin(), heights.end());
    const vu_t bounds(vu_t::size_type(n), nh);
    h = vu_t(vu_t::size_type(n), u_t(-1));
    solution = u_t(-1);
    for (vu_t t = vu_t(vu_t::size_type(n), 0); !t.empty();
        tuple_next(t, bounds))
    {
        for (u_t i = 0; i < n; ++i)
        {
            h[i] = i2h[t[i]];
        }
        u_t n_steps = 0;
        u_t n_changes = (h[0] != a[0] ? 1 : 0);
        for (u_t i = 1; i < n; ++i)
        {
            n_steps += (h[i - 1] != h[i] ? 1 : 0);
            n_changes += (h[i] != a[i] ? 1 : 0);
        }
        if ((n_steps <= k) && (solution > n_changes))
        {
            solution = n_changes;
        }
    }
}

void Flatten::solve()
{
    h = a;
    solution = seg_change(0, n, k);
}

u_t Flatten::memo_seg_change(u_t bi, u_t ei, u_t max_steps) // assuming bi < ei
{
    u_t ret;
    // au3_t key({bi, ei, max_steps});
    ul_t key = (((ul_t(bi) << 8) | ul_t(ei)) << 8) | max_steps;
    auto er = memo.equal_range(key);
    if (er.first == er.second)
    {
        ret = seg_change(bi, ei, max_steps);
        // memo.insert(er.first, au3_2u_t::value_type(key, ret));
        memo.insert(er.first, u2u_t::value_type(key, ret));
        if (dbg_flags & 0x1)
        {
            ul_t sz = memo.size();
            if ((sz & (sz - 1)) == 0)
            {
                cerr << "memo.size=" << sz << '\n';
            }
        }
    }
    else
    {
        ret = er.first->second;
    }
    return ret;
}

u_t Flatten::seg_change(u_t bi, u_t ei, u_t max_steps) // assuming bi < ei
{
    u_t ret = u_t(-1);
    u_t seg = ei - bi;
    if (max_steps > seg - 1)
    {
        max_steps = seg - 1;
    }
    if (max_steps == 0)
    {
        copy(a.begin() + bi, a.begin() + ei, h.begin() + bi);
        sort(h.begin() + bi, h.begin() + ei);
        u_t n_max = 0;
        for (u_t i = bi; i < ei; )
        {
            u_t i0 = i, hb = h[i0];
            for (++i; (i < ei) && (h[i] == hb); ++i) {}
            u_t cnt = i - i0;
            if (n_max < cnt)
            {
                n_max = cnt;
            }
        }
        ret = seg - n_max;
    }
    else if (seg <= 2) // && (max_steps > 0) is implied
    {
        ret = 0;
    }
    else
    {
        u_t l_max_steps = max_steps / 2;
        u_t r_max_steps = max_steps - l_max_steps - 1;
        for (u_t mi = bi + 1; mi < ei; ++mi)
        {
            u_t lchange = memo_seg_change(bi, mi, l_max_steps);
            u_t rchange = memo_seg_change(mi, ei, r_max_steps);
            u_t bchange = lchange + rchange;
            if (ret > bchange)
            {
                ret = bchange;
            }
        }
    }
    return ret;
}

void Flatten::print_solution(ostream &fo) const
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

    void (Flatten::*psolve)() =
        (naive ? &Flatten::solve_naive : &Flatten::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Flatten flatten(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (flatten.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        flatten.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
