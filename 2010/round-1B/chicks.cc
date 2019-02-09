// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <utility>
#include <numeric>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

static void combination_first(vu_t &c, unsigned n, unsigned k)
{
    c = vu_t(vu_t::size_type(k), 0);
    iota(c.begin(), c.end(), 0);
}

static bool combination_next(vu_t &c, unsigned n)
{
    unsigned j = 0, k = c.size();

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

class Chicks
{
 public:
    Chicks(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    int chick_cost(unsigned ci) const;
    int comb_cost(const vu_t &chick_comb) const;
    bool may_get_by_time(unsigned ci) const;
    unsigned n, k;
    ul_t b, t;
    vul_t x;
    vul_t v;
    int solution;
};

Chicks::Chicks(istream& fi) : solution(-1)
{
    fi >> n >> k >> b >> t;
    x = v = vul_t(vul_t::size_type(n), 0);
    for (unsigned i = 0; i < n; ++i)
    {
        fi >> x[i];
    }
    for (unsigned i = 0; i < n; ++i)
    {
        fi >> v[i];
    }
}

int Chicks::chick_cost(unsigned ci) const
{
    int cost = -1;
    if (may_get_by_time(ci))
    {
        cost = 0;
        for (unsigned fi = ci + 1; fi < n; ++fi)
        {
            if (!may_get_by_time(fi))
            {
                ++cost;
            }
        }
    }
    return cost;
}

int Chicks::comb_cost(const vu_t &chick_comb) const
{
    int cost = 0;
    for (unsigned cci = 0; (cost >= 0) && (cci < k); ++cci)
    {
        unsigned ci = chick_comb[cci];
        int icost = chick_cost(ci);
        cost = (icost >= 0 ? cost + icost : -1);
    }
    return cost;
}

bool Chicks::may_get_by_time(unsigned ci) const
{
    ull_t xt = x[ci] + v[ci]*t;
    bool ret = b <= xt;
    return ret;
}

void Chicks::solve_naive()
{
    vu_t candidates;
    combination_first(candidates, n, k);
    solution = comb_cost(candidates);
    while (combination_next(candidates, n))
    {
        int cc = comb_cost(candidates);
        if ((cc >= 0) && 
            ((solution < 0) || ((solution >= 0) && (cc < solution))))
        {
            solution = cc;
        }
    }
}

void Chicks::solve()
{
    vu_t costs;
    for (unsigned ci = 0; ci < n; ++ci)
    {
        int icost = chick_cost(ci);
        if (icost >= 0)
        {
            costs.push_back(icost);
        }
    }
    if (costs.size() >= k)
    {
        sort(costs.begin(), costs.end());
        solution = accumulate(costs.begin(), costs.begin() + k, 0);
    }
}

void Chicks::print_solution(ostream &fo) const
{
    if (solution < 0)
    {
        fo << " IMPOSSIBLE";
    }
    else
    {
        fo << ' ' << solution;
    }
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

    void (Chicks::*psolve)() =
        (naive ? &Chicks::solve_naive : &Chicks::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Chicks chicks(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (chicks.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        chicks.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
