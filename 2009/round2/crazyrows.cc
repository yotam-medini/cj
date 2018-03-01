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

using namespace std;

typedef unsigned long ul_t;
typedef vector<unsigned> vu_t;
typedef vector<string> vs_t;
typedef set<unsigned> setu_t;
typedef vector<setu_t> vsetu_t;

static unsigned dbg_flags;

typedef enum { WHITE, GRAY, BLACK } color_t;

static void iota(vu_t &v, int n)
{
    v = vu_t(vu_t::size_type(n), 0);
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

typedef map<vu_t, Node> graph_t;
typedef queue<vu_t> qvu_t;

class CrazyRows
{
 public:
    CrazyRows(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool is_ll(const vu_t &vi) const; // low-left triangle matrix
    unsigned run_perm() const;
    void possible_compute();
    void may_compute();
    void possible_step();
    unsigned n;
    vs_t srows;
    vu_t vrows;
    vu_t perm;
    vu_t best_perm;
    vu_t invperm;
    graph_t graph;
    qvu_t qvi;
    i2psbl_t possible;
    vsetu_t may_from;
    vsetu_t may_to;
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
        while ((pos > 0) && (cs[pos] == '0'))
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
        const vu_t& qtop_mtx = qvi.front();
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
                    vu_t vnext(qtop_mtx);
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
    may_compute();
    perm = invperm = vu_t(vu_t::size_type(n), n); // undef
    for (unsigned ti = 0; ti < n; ++ti)
    {
        auto ib = may_to[ti].begin();
        unsigned fi = *ib;
        perm[fi] = ti;
        invperm[ti] = fi;
        for (unsigned taili = ti + 1; taili != n; ++taili)
        {
            may_to[taili].erase(fi);
        }
    }
    solution = run_perm();
}

void CrazyRows::possible_compute()
{
    for (unsigned i = 0; i < n; ++i)
    {
        int val = vrows[i];
        auto er = possible.equal_range(val);
        if (er.first == er.second)
        {
            int b = (val >= 0 ? val : 0);
            i2psbl_t::value_type p(val, Possible(setu_t({i}), b, n));
            possible.insert(er.first, p);
        }
        else
        {
            Possible &p = er.first->second;
            p.idx.insert(i);
        }
    }
}

void CrazyRows::may_compute()
{
    may_from = may_to = vsetu_t(vsetu_t::size_type(n), setu_t());
    for (auto const &pi: possible)
    {
        
        const Possible &p = pi.second;
        for (auto fi: p.idx)
        {
            for (unsigned ti = p.ib; ti < p.ie; ++ti)
            {
                may_from[fi].insert(ti);
                may_to[ti].insert(fi);
            }
        }
    }
}

unsigned CrazyRows::run_perm() const
{
    unsigned total_swaps = 0;
    vu_t play;
    iota(play, n);
    vu_t::iterator pb = play.begin();
    // Bring play to perm
    for (unsigned i = 0; i < n; ++i)
    {
        // Assume  play[:i] == perm[:i]
        // find j so play[j] = perm[i];
        unsigned permi = perm[i];
        unsigned j;
        for (j = i; play[j] != permi; ++j) {}
        // back_swap
        unsigned n_swaps = j - i;
        if (n_swaps > 0)
        {
            vu_t::iterator pbi = pb + i;
            copy(pbi, pbi + n_swaps, pbi + 1);
            *pbi = permi;
            total_swaps += n_swaps;
        }
    }
    return total_swaps;
} 

bool CrazyRows::is_ll(const vu_t &vi) const
{
    bool ret = true;
    for (unsigned j = 0, nn = n; ret && (j < nn); ++j)
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
