// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<ul_t> vul_t;
typedef vector<ull_t> vull_t;

class Stall
{
 public:
    Stall(ull_t _x=0, ull_t _c=0, ul_t _i=0):
        x(_x), c(_c), i(_i) {}
    ull_t cost(ull_t wx) const
    {
        ull_t d = (x < wx ? wx - x : x - wx);
        return c + d;
    }
    ull_t x;
    ull_t c;
    u_t i;
};
typedef vector<Stall> vstall_t;


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

class Fstall
{
 public:
    Fstall(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ull_t comb_price(const vu_t &comb) const;
    void advance_low(u_t add)
    {
        for (ilow += add; (ilow < n) && stalls[ilow].i >= ipre; ++ilow) {}
        next_low_cost = (ilow < n ? stalls[ilow].cost(curr_wx) : infty);
        min_next_cost = min(next_low_cost, next_high_cost);
    }
    void advance_high(u_t add)
    {
        for (ihigh += add; (ihigh < n) && stalls[ihigh].i <= icurr; ++ihigh) {}
        next_high_cost = (ihigh < n ? stalls[ihigh].cost(curr_wx) : infty);
        min_next_cost = min(next_low_cost, next_high_cost);
    }
    ull_t ware_total_cost(u_t wi) const
    {
        ull_t wx = xstalls[wi].x;
        ull_t cost = xstalls[wi].cost(wx);
        for (u_t i = 0, ns = 0; ns < k; ++i)
        {
            if (stalls[i].i != wi)
            {
                ull_t costi = stalls[i].cost(wx);
                cost += costi;
                ++ns;
            }
        }
        return cost;
    }
    static const ull_t infty;
    u_t k, n;
    vull_t x;
    vull_t c;
    ull_t solution;
    vstall_t stalls;
    vstall_t xstalls;
    u_t ilow, ihigh, ipre, icurr;
    ull_t curr_wx;
    ull_t next_low_cost;
    ull_t next_high_cost;
    ull_t min_next_cost;
};
const ull_t Fstall::infty = ull_t(-1);

Fstall::Fstall(istream& fi) : solution(0)
{
    fi >> k >> n;
    ull_t v;
    x.reserve(n);
    c.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        fi >> v;
        x.push_back(v);
    }
    for (u_t i = 0; i < n; ++i)
    {
        fi >> v;
        c.push_back(v);
    }
}

void Fstall::solve_naive()
{
    vu_t comb;
    combination_first(comb, n, k + 1);

    ull_t best_price = comb_price(comb);
    while (combination_next(comb, n))
    {
        ull_t price = comb_price(comb);
        if (best_price > price)
        {
            best_price = price;
        }
    }
    solution = best_price;
}

void Fstall::solve()
{
    stalls.reserve(n);
    stalls.clear();
    for (u_t i = 0; i < n; ++i)
    {
        stalls.push_back(Stall(x[i], c[i]));
    }
    sort(stalls.begin(), stalls.end(),
        [](const Stall& s0, const Stall& s1)
        {
            return s0.x < s1.x;
        });
    for (u_t i = 0; i < n; ++i)
    {
        stalls[i].i = i;
    }
    xstalls = stalls;

    ull_t wx0 = stalls[0].x;
    sort(stalls.begin(), stalls.end(),
        [wx0](const Stall& s0, const Stall& s1)
        {
            return s0.cost(wx0) < s1.cost(wx0);
        });
    ull_t best = ware_total_cost(0);

    vstall_t stalls_next;
    for (ipre = 0, icurr = 1; icurr < n; ipre = icurr++)
    {
        curr_wx = xstalls[icurr].x;
        ull_t pre_cost = xstalls[ipre].cost(curr_wx);
        ull_t curr_cost = xstalls[icurr].cost(curr_wx);
        ull_t min_pre_curr_cost = min(pre_cost, curr_cost);
        ilow = ihigh = 0;
        min_next_cost = infty;
        advance_low(0);
        advance_high(0);
        stalls_next.clear();
        while (stalls_next.size() != n)
        {
            if (min_pre_curr_cost <= min_next_cost)
            {
                if (pre_cost <= curr_cost)
                {
                    stalls_next.push_back(xstalls[ipre]);
                    pre_cost = infty;
                }
                else
                {
                    stalls_next.push_back(xstalls[icurr]);
                    curr_cost = infty;
                }
                min_pre_curr_cost = min(pre_cost, curr_cost);
            }
            else
            {
                if (next_low_cost <= next_high_cost)
                {
                    stalls_next.push_back(stalls[ilow]);
                    advance_low(1);
                }
                else
                {
                    stalls_next.push_back(stalls[ihigh]);
                    advance_high(1);
                }
            }
        }
        swap(stalls, stalls_next);
        ull_t curr_total_cost = ware_total_cost(icurr);
        if (best > curr_total_cost)
        {
            best = curr_total_cost;
        }
    }
    solution = best;
}

ull_t Fstall::comb_price(const vu_t &comb) const
{
    ull_t base = 0;
    for (u_t pi: comb)
    {
        base += c[pi];
    }
    ull_t smin_distance(-1);
    for (u_t p: comb)
    {
        ull_t xp = x[p];
        ull_t sdist = 0;
        for (u_t q: comb)
        {
            ull_t xq = x[q];
            ull_t delta = (xp < xq ? xq - xp : xp - xq);
            sdist += delta;
        }
        if (smin_distance > sdist)
        {
            smin_distance = sdist;
        }
    }
    ull_t ret = base + smin_distance;
    return ret;
}

void Fstall::print_solution(ostream &fo) const
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

    void (Fstall::*psolve)() =
        (naive ? &Fstall::solve_naive : &Fstall::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Fstall fstall(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (fstall.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        fstall.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
