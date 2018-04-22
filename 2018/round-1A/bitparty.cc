// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <numeric>

#include <cstdlib>

using namespace std;

typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class Cachier
{
 public:
    Cachier(ul_t _m=0, ul_t _s=0, ul_t _p=0) : 
        m(_m), s(_s), p(_p), nb_assigned(0) {}
    ull_t nb_time(unsigned nb) const 
    {
        ul_t ret = s*nb + p;
        return ret;
    }
    ull_t time() const { return nb_time(nb_assigned); }
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
    ull_t r, b, c;
    vcachier_t cachiers;
    ull_t solution;
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
    ull_t time_low = 0;
    ull_t time_high = 0;
    for (const Cachier &cachier: cachiers)
    {
        ull_t t = cachier.nb_time(b);
        if (time_high < t)
        {
            time_high = t;
        }
    }
    if (dbg_flags & 0x1) { cerr << "time_high="<< time_high << "\n"; }
    vull_t bits_possible = vull_t(vull_t::size_type(c), vull_t::value_type(0));
    while (time_low < time_high)
    {
        ull_t tmid = (time_low + time_high)/2;
        for (unsigned ci = 0; ci < c; ++ci)
        {
            const Cachier &cachier = cachiers[ci];
            // t = cachier.s * bits + p
            // bits = (t - p) / s
            ull_t bt = 0;
            if (cachier.p < tmid)
            {
                bt = (tmid - cachier.p) / cachier.s;
                if (bt > cachier.m)
                {
                    bt = cachier.m;
                }
            }
            bits_possible[ci] = bt;
        }
        sort(bits_possible.begin(), bits_possible.end());
        ull_t bmax = accumulate(bits_possible.end() - r, bits_possible.end(), 0);
        if (b <= bmax)
        {
            time_high = tmid;
        }
        else
        {
            time_low = tmid + 1;
        }
    }
    solution = time_low;
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
