// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>
#include "bipartite_match.h"

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ul_t> vul_t;
typedef vector<vul_t> vvul_t;

static unsigned dbg_flags;

typedef enum {REL_UNDEF, REL_LT, REL_GT, REL_X} rel_t;
typedef vector<rel_t> vrel_t;

class StockChart
{
 public:
    StockChart(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void compute_relations();
    rel_t compute_relation(unsigned si0, unsigned si1) const;
    rel_t relation(unsigned si0, unsigned si1) const
        { return relations[n * si0 + si1]; }
    bool anti_chain(ul_t subset_mask) const;
    static unsigned n_bits(ul_t n);
    unsigned n, k;
    vvul_t stocks;
    unsigned solution;
    vrel_t relations;
};

class StockCompare
{
 public:
    StockCompare(const vvul_t& vstocks) : stocks(vstocks) {}
    bool operator()(const unsigned& u0, const unsigned& u1) const;
 private:
    const vvul_t &stocks;
};

StockChart::StockChart(istream& fi) : solution(~0)
{
    fi >> n >> k;
    const vul_t vz(vul_t::size_type(k), 0);
    stocks = vvul_t(vvul_t::size_type(n), vz);
    for (unsigned si = 0; si < n; ++si)
    {
        vul_t &stock = stocks[si];
        for (unsigned ti = 0; ti < k; ++ti)
        {
            fi >> stock[ti];
        }
    }
}

void StockChart::solve_naive()
{
    compute_relations();
    solution = 1;
    for (ul_t subset_mask = 1; subset_mask < ul_t(1ul << n); ++subset_mask)
    {
        unsigned nb = n_bits(subset_mask);
        if ((solution < nb) && anti_chain(subset_mask))
        {
            solution = nb;
        }
    }
}

void StockChart::solve()
{
    // After peeking in the analysis... :(
    compute_relations();
    lr_edges_t match, edges;
    for (unsigned si0 = 0; si0 < n; ++si0)
    {
        relations[n*si0 + si0] = REL_X;
        for (unsigned si1 = 0; si1 < n; ++si1)
        {
            rel_t r = compute_relation(si0, si1);
            relations[n*si0 + si1] = r;
            if (r == REL_LT)
            {
                edges.insert(uu_t{si0, si1});
            }
        }
    }
    int flow = bipartitee_max_match(match, edges);
    if (flow < 0) 
    { 
        cerr << "Failure:\n";
        exit(1);
    }
    solution = n - flow;
}

void StockChart::compute_relations()
{
    relations = vrel_t(vrel_t::size_type(n * n), REL_UNDEF);
    for (unsigned si0 = 0; si0 < n; ++si0)
    {
        relations[n*si0 + si0] = REL_X;
        for (unsigned si1 = si0 + 1; si1 < n; ++si1)
        {
            rel_t r = compute_relation(si0, si1);
            relations[n*si0 + si1] = r;
            if (r != REL_X)
            {
                r = (r == REL_LT ? REL_GT : REL_LT);
            }
            relations[n*si1 + si0] = r;
        }
    }
}

rel_t
StockChart::compute_relation(unsigned si0, unsigned si1) const
{
    rel_t ret = REL_UNDEF;
    bool any_lt = false, any_gt = false;
    const vul_t &s0 = stocks[si0];
    const vul_t &s1 = stocks[si1];
    for (unsigned ti = 0; (ti != k) && (ret == REL_UNDEF); ++ti)
    {
        if (s0[ti] < s1[ti])
        {
            any_lt = true;
        }
        else if (s0[ti] > s1[ti])
        {
            any_gt = true;
        }
        else
        {
            ret = REL_X;
        }
        if (any_lt && any_gt)
        {
            ret = REL_X;
        }
    }
    if (ret == REL_UNDEF)
    {
        ret = (any_lt ? REL_LT : REL_GT);
    }
    return ret;
}

bool StockChart::anti_chain(ul_t subset_mask) const
{
    bool anti = true;
    for (unsigned si0 = 0; anti && (si0 < n); ++si0)
    {
        bool si0_in = ((subset_mask & (ul_t(1) << si0)) != 0);
        for (unsigned si1 = (si0_in ? si0 + 1 : n); 
             anti && (si1 < n); ++si1)
        {
            bool si1_in = ((subset_mask & (ul_t(1) << si1)) != 0);
            if (si1_in)
            {
                anti = (relation(si0, si1) == REL_X);
            }
        }
    }
    return anti;
}

unsigned StockChart::n_bits(ul_t n)
{
    unsigned ret = 0;
    while (n > 0)
    {
        if (n & ul_t(1))
        {
            ++ret;
        }
        n >>= 1;
    }
    return ret;
}

void StockChart::print_solution(ostream &fo) const
{
    fo << " " << solution;
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

    void (StockChart::*psolve)() =
        (naive ? &StockChart::solve_naive : &StockChart::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        StockChart stockchart(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (stockchart.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        stockchart.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
