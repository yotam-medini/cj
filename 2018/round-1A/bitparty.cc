// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ull_t> vull_t;
typedef set<unsigned> setu_t;

static unsigned dbg_flags;

class Cachier
{
 public:
    Cachier(ul_t _m=0, ul_t _s=0, ul_t _p=0) : 
        m(_m), s(_s), p(_p), nb_assigned(0) {}
    ul_t nb_time(unsigned nb) const 
    {
        ul_t ret = s*nb + p;
        return ret;
    }
    ul_t time() const { return nb_time(nb_assigned); }
    ull_t m, s, p;
    ull_t nb_assigned;
    vull_t best_tail;
};
typedef vector<Cachier> vcachier_t;

class BitParty
{
 public:
    BitParty(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ull_t time_subsplit(unsigned ci, ull_t nbit, ull_t nr);
    void reduce();
    unsigned best_deal(ull_t nb) const;
    bool is_better(const Cachier &can, const Cachier &than, ull_t nb) const;
    bool pair_reduce(unsigned ci_slow, unsigned ci);
    ull_t r, b, c;
    vcachier_t cachiers;
    ull_t solution;
    setu_t c_assigned;
    setu_t c_unassigned;
};

BitParty::BitParty(istream& fi) : solution(ull_t(-1))
{
    fi >> r >> b >> c;
    cachiers.reserve(c);
    for (ull_t i = 0; i < c; ++i)
    {
        ull_t m, s, p;
        fi >> m >> s >> p;
        cachiers.push_back(Cachier(m, s, p));
    }
}

void BitParty::solve_naive()
{
    solution = time_subsplit(0, b, 0);
}

ull_t BitParty::time_subsplit(unsigned ci, ull_t nbit, ull_t nr)
{
    ull_t ret = (nbit > 0 ? ull_t(-1) : 0);
    if ((ci < c) && (nbit > 0) && (nr < r))
    {
        Cachier &cachier = cachiers[ci];
        ull_t nr_sub = nr;
        for (ull_t nb = 0; (nb <= cachier.m) && (nb <= nbit); ++ nb)
        {
            cachier.nb_assigned = nb;
            ull_t tc = (nb ? cachier.time() : 0);
            ull_t trest = time_subsplit(ci + 1, nbit - nb, nr_sub);
            ull_t t = max(tc, trest);
            nr_sub = nr + 1;
            if (ret > t)
            {
                ret = t;
                vull_t &bt = cachier.best_tail;
                bt.clear();
                bt.push_back(nb);
                if (ci + 1 < c) 
                {
                    const vull_t bt1 = cachiers[ci + 1].best_tail;
                    bt.insert(bt.end(), bt1.begin(), bt1.end());
                }
            }
        }
    }
    else
    {
        for (unsigned ctail = ci; ctail < c; ++ctail)
        {
            Cachier &cachier = cachiers[ctail];
            cachier.nb_assigned = 0;
        }
    }
    return ret;
}

void BitParty::solve()
{
    // solve_naive();
    for (unsigned ci = 0; ci < c; ++ci)
    {
        c_unassigned.insert(ci);
    }

    ull_t nb_pending = b;
    while ((nb_pending > 0) && (c_assigned.size() < r))
    {
        unsigned ci = best_deal(nb_pending);
        Cachier &cachier = cachiers[ci];
        unsigned nb = min(nb_pending, cachier.m);
        cachier.nb_assigned = nb;
        nb_pending -= nb;
        c_assigned.insert(setu_t::value_type(ci));
        c_unassigned.erase(setu_t::value_type(ci));
    }

    while ((nb_pending > 0) && !c_unassigned.empty())
    {
        // Give up min-nb cachier
        unsigned ci_min = *(c_assigned.begin());
        for (auto cii = c_assigned.begin(), cie = c_assigned.end();
            cii != cie; ++cii)
        {
            unsigned ci = *cii;
            if (cachiers[ci_min].nb_assigned >  cachiers[ci].nb_assigned)
            {
                ci_min = ci;
            }
        }
        nb_pending += cachiers[ci_min].nb_assigned;
        cachiers[ci_min].nb_assigned = 0;
        c_assigned.erase(setu_t::value_type(ci_min));
        unsigned ci = best_deal(nb_pending);
        unsigned nb = min(nb_pending, cachiers[ci].m);
        cachiers[ci].nb_assigned = nb;
        nb_pending -= nb;
        c_assigned.insert(setu_t::value_type(ci));
        c_unassigned.erase(setu_t::value_type(ci));
    }
    if (nb_pending > 0)
    {
        cerr << "Failure\n";  exit(1);
    }
    reduce();
    solution = 0;
    for (auto cii = c_assigned.begin(), cie = c_assigned.end();
        cii != cie; ++cii)
    {
        unsigned ci = *cii;
        unsigned t = cachiers[ci].time();
        if (solution < t)
        {
            solution = t;
        }
    }
}

unsigned BitParty::best_deal(ull_t nb) const
{
    unsigned ci_best = *(c_unassigned.begin());
    for (auto cii = c_unassigned.begin(), cie = c_unassigned.end(); cii != cie;
        ++cii)
    {
        unsigned ci = *cii;
        if (is_better(cachiers[ci], cachiers[ci_best], nb))
        {
            ci_best = ci;
        }
    }
    return ci_best;
}

bool BitParty::is_better(const Cachier &can, const Cachier &than, ull_t nb)
    const
{
    bool ret = false;
    nb = min(nb, max(can.m, than.m));
    if ((nb <= can.m) && (nb <= than.m))
    {
        ull_t tcan = can.nb_time(nb);
        ull_t tthan = than.nb_time(nb);
        ret = (tcan < tthan);
    }
    else
    {
        if (nb > can.m)
        {
            ull_t tcan = can.nb_time(can.m) + than.nb_time(nb - can.m);
            ull_t tthan = than.nb_time(nb);
            ret = (tcan < tthan);
        }
        else
        {
            ull_t tcan = can.nb_time(nb);
            ull_t tthan = than.nb_time(than.m) + can.nb_time(nb - than.m);
            ret = (tcan < tthan);
        }
    }
      
    return ret;
}

void BitParty::reduce()
{
    bool reducing = true;
    while (reducing)
    {
        reducing = false;
        unsigned ci_slowest = *c_assigned.begin();
        ull_t t_slowest = cachiers[ci_slowest].time();
        for (auto cii = c_assigned.begin(), cie = c_assigned.end();
            cii != cie; ++cii)
        {
            unsigned ci = *cii;
            ull_t t = cachiers[ci].time();
            if (t_slowest < t)
            {
                t_slowest = t;
                ci_slowest = ci;
            }
        }

        //for (auto cii = c_assigned.begin(), cie = c_assigned.end();
        //    (cii != cie) && !reducing; ++cii)
        for (unsigned ci = 0; ci < c; ++ci)
        {
             // unsigned ci = *cii;
             reducing = (ci != ci_slowest) && 
                 ((c_assigned.size() < r) || (cachiers[ci].nb_assigned > 0)) &&
                 pair_reduce(ci_slowest, ci);
        }
    }
}

bool BitParty::pair_reduce(unsigned ci_slow, unsigned ci)
{
    bool ret = false;
    Cachier &cslow = cachiers[ci_slow];
    Cachier &cother = cachiers[ci];
    ull_t nb = cslow.nb_assigned + cother.nb_assigned;
    ull_t tcurr = cslow.time();
    
    // p1 + s1 x = p2 + s2 (n - x)
    // x = (p2 - p1 + s2n)/(s1 + s2)
    ull_t x = (cother.p + cother.s * nb - cslow.p) / (cslow.s + cother.s);
    ull_t x1 = x + 1;
    if (max(cslow.nb_time(x), cother.nb_time(nb - x)) >
        max(cslow.nb_time(x1), cother.nb_time(nb - x1)))
    {
        x = x1;
    }
    if (x < cslow.m)
    {
         ull_t xother = nb - x;
         if (xother > cother.m)
         {
              xother = cother.m;
              x = nb - xother;
         }
         if (x < cslow.m)
         {
             ull_t t = max(cslow.nb_time(x), cother.nb_time(xother));
             ret = (t < tcurr);
             if (ret)
             {
                 
                 cslow.nb_assigned = x;
                 if (cother.nb_assigned == 0)
                 {
                     c_assigned.insert(setu_t::value_type(ci));
                     c_unassigned.erase(setu_t::value_type(ci));
                 }
                 cother.nb_assigned = nb - x;
             }
         }
    }
    return ret;
}

void BitParty::print_solution(ostream &fo) const
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
        exit(13);
    }

    string ignore;
    unsigned n_cases;
    *pfi >> n_cases;
    getline(*pfi, ignore);

    void (BitParty::*psolve)() =
        (naive ? &BitParty::solve_naive : &BitParty::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        BitParty bitParty(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (bitParty.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        bitParty.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
