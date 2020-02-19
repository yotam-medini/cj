// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ull_t> vull_t;
// typedef map<ul_t, ull_t> ul2ull_t;

static unsigned dbg_flags;

class Chain
{
 public:
    Chain(u_t s=0, ull_t w=0) : start(s), weight(w) {}
    u_t start;
    ull_t weight;
};
typedef map<ul_t, Chain> ul2chain_t;
typedef vector<Chain> vchain_t;


class AntStack
{
 public:
    AntStack(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t n;
    vull_t w;
    u_t solution;
};

AntStack::AntStack(istream& fi) : solution(0)
{
    fi >> n;
    w = vull_t(vull_t::size_type(n), 0);
    for (u_t i = 0; i < n; ++i)
    {
        fi >> w[i];
    }
}

void AntStack::solve_naive()
{
    ul2chain_t sz2chain;
    const ul2chain_t::value_type z(1, Chain(0, w[0]));
    sz2chain.insert(sz2chain.end(), z);
    for (u_t i = 1; i < n; ++i)
    {
        ull_t weight = w[i];
        ull_t w6 = 6*weight;
        for (ul2chain_t::reverse_iterator 
            iter = sz2chain.rbegin(), iter_next = iter;
            (iter != sz2chain.rend());
            iter = iter_next)
        {
            ++iter_next;
            ul_t sz = iter->first;
            Chain& chain = iter->second;
            if (chain.start < i)
            {
                if (chain.weight <= w6)
                {
                    ull_t weight_new = chain.weight + weight;
                    auto ew = sz2chain.equal_range(sz + 1);
                    if (ew.first == ew.second) // new
                    {
                         ul2chain_t::value_type v(sz + 1, Chain(i, weight_new));
                         sz2chain.insert(ew.first, v);
                    }
                    else
                    {
                        Chain& chain_old = ew.first->second;
                        if (chain_old.weight > weight_new)
                        {
                            chain_old.start = i;
                            chain_old.weight = weight_new;
                        }
                    }
                }
                if ((weight < w[chain.start]) && 
                    (chain.weight - w[chain.weight] <= w6))
                {
                     chain.start = i;
                     chain.weight -= (w[chain.weight] - weight);
                }
            }
        }   
    }
    const ul2chain_t::value_type& longest = *(sz2chain.rbegin());
    solution = longest.first;
}

void AntStack::solve()
{
    vchain_t sz2sw;
    for (u_t i = 0; i < n; ++i)
    {
        ull_t weight = w[i];
        ull_t w6 = 6*weight;
        vchain_t::iterator lb = 
            lower_bound(sz2sw.begin(), sz2sw.end(), weight,
                [this](const Chain& c, const ull_t cw) // --this ???
                { return c.weight < cw; });
        vchain_t::iterator ub = 
            upper_bound(sz2sw.begin(), sz2sw.end(), w6,
                [this](const ull_t carry, const Chain& c)
                {
#if 0
                    bool ret = carry < c.weight - w[c.start]; 
                    cerr << __LINE__ << ": carry="<<carry << 
                      ", c.weight="<<c.weight << ", start="<<c.start << 
                      ", ret="<<ret <<"\n"; 
#endif
                    return carry < c.weight - w[c.start]; 
                });
        ull_t last_weight = (sz2sw.empty() ? 0 : sz2sw.back().weight);
        for (u_t sz = ub - sz2sw.begin(), sz_lb = lb - sz2sw.begin();
            sz > sz_lb;)
        {
            --sz;
            ull_t wstart = w[sz2sw[sz].start];
            if ((weight < wstart) && (sz2sw[sz].weight - wstart <= w6))
            {
                sz2sw[sz].weight -= wstart - weight;
                sz2sw[sz].start = i;
            }
            ull_t pre_weight = (sz > 0 ? sz2sw[sz - 1].weight : ull_t(-1));
            if ((pre_weight <= w6) && (pre_weight + weight < sz2sw[sz].weight))
            {
                sz2sw[sz].weight = pre_weight + weight;
                sz2sw[sz].start = i;
            }
        }
        if (last_weight <= w6)
        {
            sz2sw.push_back(Chain{i, last_weight + weight});
        }
    }
    solution = sz2sw.size();
}

void AntStack::print_solution(ostream &fo) const
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

    void (AntStack::*psolve)() =
        (naive ? &AntStack::solve_naive : &AntStack::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        AntStack antStack(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (antStack.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        antStack.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
