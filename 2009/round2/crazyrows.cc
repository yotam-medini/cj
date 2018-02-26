// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <set>
#include <map>
#include <queue>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;
typedef vector<bool> vb_t;
typedef vector<unsigned> vu_t;
typedef vector<int> vi_t;
typedef vector<string> vs_t;

typedef set<unsigned> setu_t;
typedef set<int> seti_t;
typedef map<int, seti_t> i2seti_t;

static unsigned dbg_flags;

typedef enum { WHITE, GRAY, BLACK } color_t;

static void iota(vi_t &v, int n)
{
    v = vi_t(vi_t::size_type(n), 0);
    for (int i = 0; i < n; ++i) { v[i] = i; }
}

class Node
{
 public:
    Node(unsigned vd=0, color_t vc=WHITE) : d(vd), color(vc) {}
    unsigned d;
    color_t color;
};

class Possible
{
 public:
    Possible(const setu_t &vidx=setu_t(), unsigned vib=0, unsigned vie=0) :
        idx(vidx), ib(vib), ie(vie) {}
    unsigned size() const { return idx.size(); }
    setu_t idx;
    unsigned ib;
    unsigned ie;
};
typedef map<int, Possible> i2psbl_t;

typedef map<vi_t, Node> graph_t;
typedef queue<vi_t> qvi_t;

class CrazyRows
{
 public:
    CrazyRows(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool is_ll(const vi_t &vi) const; // low-left triangle matrix
    void initial_push_down();
    void handle_displaced();
    void transfer(unsigned pi, unsigned ti);
    void improve_perm();
    unsigned run_perm() const;
    void displaced_add(int pi);
    void possible_compute();
    void possible_step();
    unsigned n;
    vs_t srows;
    vi_t vrows;
    vi_t perm;
    vi_t best_perm;
    vi_t invperm;
    i2seti_t displaced;
    graph_t graph;
    qvi_t qvi;
    i2psbl_t possible;
    unsigned solution;
};

CrazyRows::CrazyRows(istream& fi) : n(0), solution(~0)
{
    fi >> n;
    srows.reserve(n);
    vrows.reserve(n);
    for (unsigned i = 0; i < n; ++i)
    {
        string s;
        fi >> s;
        srows.push_back(s);
        const char *cs = s.c_str();
        int pos = n - 1;
        while ((pos >= 0) && (cs[pos] == '0'))
        {
            --pos;
        }
        vrows.push_back(pos);
    }
}

void CrazyRows::solve_naive()
{
    graph.insert(graph_t::value_type(vrows, Node(0, GRAY)));
    qvi.push(vrows);
    bool found = false;
    while (!found)
    {
        const vi_t& qtop_mtx = qvi.front();
        graph_t::iterator w = graph.find(qtop_mtx);
        Node &node = (*w).second;
        if (is_ll(qtop_mtx))
        {
            found = true;
            solution = node.d;
        }
        else
        {
            unsigned dnext = node.d + 1;
            for (unsigned i = 0, j = 1; j < n; i = j++)
            {
                if (qtop_mtx[i] > qtop_mtx[j])
                {
                    vi_t vnext(qtop_mtx);
                    swap(vnext[i], vnext[j]);
                    w = graph.find(vnext);
                    if (w == graph.end())
                    {
                        graph.insert(graph_t::value_type(vnext, 
                            Node(dnext, GRAY)));
                        qvi.push(vnext);
                    }
                }
            }
        }
        node.color = BLACK;
        qvi.pop();
    }
}

void CrazyRows::solve()
{
    possible_compute();
    iota(perm, n);
    iota(invperm, n);
    improve_perm();
    solution = run_perm();
    bool progress = true;
    while (progress)
    {
        progress = false;
        unsigned i_push = n;
        for (unsigned i = 0; i < n; ++i)
        {
            unsigned pi = invperm[i];
            if (vrows[pi] > int(i))
            {
                if ((i_push == n) || (vrows[invperm[i_push]] < vrows[pi]))
                {
                    i_push = i;
                }
            }
        }
        if (i_push != n)
        {
            progress = true;
            unsigned e = i_push;
            for (; vrows[invperm[e]] >= int(e); ++e) {}
            for (unsigned i = e, i1 = e - 1; i > i_push; i = i1--)
            {
                int &ip = invperm[i];
                int &ip1 = invperm[i1];
                swap(perm[ip], perm[ip1]);  
                swap(ip, ip1);
            }
        }
    }
    improve_perm();
    solution = run_perm();
}

#if 0
void CrazyRows::solve()
{
    possible_compute();
    iota(perm, n);
    iota(invperm, n);
    bool progress = true;
    while (progress)
    {
        progress = false;
        unsigned i_heavy;
        int weight_max = 0;
        for (unsigned i = 0, i1 = 1; i1 < n; i = i1++)
        {
            int pi = invperm[i];
            int pi1 = invperm[i1];
            int wi = vrows[pi] - i;
            if (wi > 0)
            {
                int wi1 = vrows[pi1] - i1;
                int weight = wi - wi1;
                if (weight_max < weight)
                {
                    weight_max = weight;
                    i_heavy = i;
                }
            }
        }
        if (weight_max > 0)
        {
            progress = true;
            int pi = invperm[i_heavy];
            int pi1 = invperm[i_heavy + 1];
            swap(invperm[i_heavy], invperm[i_heavy + 1]);
            swap(perm[pi], perm[pi1]);  
        }
    }
    improve_perm();
    solution = run_perm();
}
#endif

void CrazyRows::possible_compute()
{
    for (unsigned i = 0; i < n; ++i)
    {
        int val = vrows[i];
        auto er = possible.equal_range(val);
        if (er.first == er.second)
        {
            i2psbl_t::value_type p(val, Possible(setu_t({i}), val, n));
            possible.insert(er.first, p);
        }
        else
        {
            Possible &p = er.first->second;
            p.idx.insert(i);
        }
    }
    unsigned n_above;
    for (auto i = possible.rbegin(), e = possible.rend(); i != e; ++i)
    {
        Possible &p = i->second;
        p.ie = n - n_above;
        n_above += p.size();
    }
}

#if 0
// Algorithm:
//   Push heavy rows down starting from the heaviest, most lower,
//   Pushed down rows considered final setting (!?)
//   Keep list/set of displaced rows.
//   With the displaced rows again start with the heavier lower.
//   Given displaced, Consider empty places, but if not
//   do push down, respect 'final setting'.
void CrazyRows::solve()
{
    iota(perm, n);
    iota(invperm, n);
    possible_compute();
    initial_push_down();
    handle_displaced();
    improve_perm();
    solution = run_perm();
}
#endif

void CrazyRows::transfer(unsigned pi, unsigned ti)
{
    int new_displaced =  invperm[ti];
    if (new_displaced >= 0)
    {
        displaced_add(new_displaced);
        perm[new_displaced] = -1;
    }
    int old = perm[pi];
    if (old >= 0)
    {
        invperm[old] = -1;
    }
    perm[pi] = ti;
    invperm[ti] = pi;
}

void CrazyRows::improve_perm()
{
    for (auto const &pi: possible)
    {
        const Possible &p = pi.second;
        vi_t targets;
        for (auto i: p.idx)
        {
            int ti = perm[i];
            targets.push_back(ti);
        }
        sort(targets.begin(), targets.end());
        auto tii = targets.begin();
        for (auto i: p.idx)
        {
            int ti = *tii;
            perm[i] = ti;
            ++tii;
        }
    }
}

void CrazyRows::initial_push_down()
{
    for (auto i = possible.rbegin(), e = possible.rend(); i != e; ++i)
    {
        int v = i->first;
        const Possible &p = i->second;
        unsigned ti = v;
        for (auto ii = p.idx.begin(), ie = p.idx.end(); ii != ie; ++ii)
        {
            int pi = *ii;
            if (v > pi)
            {
                transfer(pi, ti++);
            }
        }
    }
}

#if 0
void CrazyRows::initial_push_down()
{
    // unsigned perm_size = 0;
    for (int k = n - 1; k >= 0; --k)
    {
        const int vk = vrows[k];
        if (vk > k)
        {
            // How many v[k] above
            vi_t f;
            for (int i = 0; i <= k; ++i)
            {
                if (vrows[i] == vk)
                {
                    f.push_back(i);
                }
            }
            int n_push = f.size();
            auto fiter = f.begin();
            for (int pi = vk; pi < vk + n_push; ++pi, ++fiter)
            {
                int i = *fiter;
                transfer(i, pi);
            }
        }
    }    
}
#endif

void CrazyRows::handle_displaced()
{
    i2seti_t old_displaced;
    swap(old_displaced, displaced);
    while (!old_displaced.empty())
    {
        displaced.clear();
        for (auto &keyval: old_displaced)
        {
            int kv = keyval.first;
            int ti = kv;
            const seti_t &pis = keyval.second;
            for (auto i = pis.rbegin(), e = pis.rend(); i != e; ++i)
            {
                int pi = *i;
                for (; invperm[ti] >= 0 && vrows[invperm[ti]] >= kv; ++ti);
                transfer(pi, ti);
            }
        }
        swap(old_displaced, displaced);
    }
}

unsigned CrazyRows::run_perm() const
{
    unsigned total_swaps = 0;
    vi_t play;
    iota(play, n);
    vi_t::iterator pb = play.begin();
    // Bring play to perm
    for (unsigned i = 0; i < n; ++i)
    {
        // Assume  play[:i] == perm[:i]
        // find j so play[j] = perm[i];
        int permi = perm[i];
        unsigned j;
        for (j = i; play[j] != permi; ++j) {}
        // back_swap
        unsigned n_swaps = j - i;
        if (n_swaps > 0)
        {
            vi_t::iterator pbi = pb + i;
            copy(pbi, pbi + n_swaps, pbi + 1);
            *pbi = permi;
            total_swaps += n_swaps;
        }
    }
    return total_swaps;
} 

void CrazyRows::displaced_add(int pi)
{
    int vpi = vrows[pi];
    auto w = displaced.find(vpi);
    if (w == displaced.end())
    {
        i2seti_t::value_type piv(vpi, seti_t({pi}));
        displaced.insert(piv);
    }
    else
    {
         (*w).second.insert(pi);
    }
}

#if 0
void CrazyRows::solve()
{
    unsigned n_swaps = 0;
    vi_t v(vrows);
    for (int k = n - 1; k >= 0; --k)
    {
        if (v[k] > k)
        {
            // push v[k] down
            int curr = k, target = v[k];
            while (curr < target)
            {
                // Find first to swap curr <-> curr+1
                int light = curr + 1;
                while (v[light] > curr)
                {
                    ++light;
                }
                while (light > curr)
                {
                    int lm1 = light - 1;
                    swap(v[light], v[lm1]);
                    light = lm1;
                    ++n_swaps;
                }
                ++curr;
            }
        }
    }
    solution = n_swaps;
}
#endif



bool CrazyRows::is_ll(const vi_t &vi) const
{
    bool ret = true;
    for (int j = 0, nn = n; ret && (j < nn); ++j)
    {
        ret = (vi[j] <= j);
    }
    return ret;
}

void CrazyRows::print_solution(ostream &fo) const
{
    fo << " " << solution;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (argv[ai][0] == '-') && argv[ai][1] != '\0'; ++ai)
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

    unsigned n_cases;
    *pfi >> n_cases;

    void (CrazyRows::*psolve)() =
        (naive ? &CrazyRows::solve_naive : &CrazyRows::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        CrazyRows crazy_rows(*pfi);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << ", " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (crazy_rows.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        crazy_rows.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
