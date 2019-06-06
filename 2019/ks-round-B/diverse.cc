// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <set>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef map<u_t, u_t> u2u_t;
typedef set<u_t> setu_t;
typedef vector<u2u_t> vu2u_t;

class Node
{
 public:
    Node(int t=0, int pmax=0) : total(t), pfx_max(pmax) {}
    int total;
    int pfx_max;
};
typedef vector<Node> vnode_t;
typedef vector<vnode_t> vvnode_t;
static const Node znode(0, 0);

enum { AMAX = 10000 };

static unsigned dbg_flags;

class Diverse
{
 public:
    Diverse(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t lr_count(u_t l, u_t r) const;
    u_t lr_count_reduced(u_t l, u_t r) const;
    u_t reduce();
    void init_level_nodes();
    void init_level_nodes_low();
    void node_update_up(u_t ni, int val);
    u_t n, s;
    u_t solution;
    vu_t trinkets;
    vu2u_t count_till;
    vu_t trinkets_reduced;
    u_t n_reduced;
    vvu_t positions;
    vvnode_t level_nodes;
};

Diverse::Diverse(istream& fi) : solution(0)
{
    fi >> n >> s;
    trinkets.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        u_t t;
        fi >> t;
        trinkets.push_back(t);
    }
}

void maximize(u_t &v, u_t by)
{
    if (v < by)
    {
        v = by;
    }
}

#if 1
void Diverse::solve_naive()
{
    u2u_t count;
    count_till.reserve(n + 1);
    count_till.push_back(count);
    for (u_t i = 0; i < n; ++i)
    {
        u_t t = trinkets[i];
        auto er = count.equal_range(t);
        if (er.first == er.second)
        {
            u2u_t::value_type v(t, 1);
            count.insert(er.first, v);
        }
        else
        {
            ++(*er.first).second;
        }
        count_till.push_back(count); 
    }

    for (u_t l = 0; l < n; ++l)
    {
        for (u_t r = l + 1; r <= n; ++r)
        {
            u_t candid = lr_count(l, r);
            if (solution < candid)
            {
                solution = candid;
            }
        }
    }
}

u_t Diverse::lr_count(u_t l, u_t r) const
{
    u_t ret = 0;
    const u2u_t& rcount = count_till[r];
    const u2u_t& lcount = count_till[l];
    for (u2u_t::const_iterator i = rcount.begin(), e = rcount.end();
        i != e; ++i)
    {
        const u2u_t::value_type &v = *i;
        u_t nr = v.second;
        u2u_t::const_iterator li = lcount.find(v.first);
        u_t nl = (li == lcount.end() ? 0 : (*li).second);
        u_t nn = nr - nl;
        if (nn <= s)
        {
            ret += nn;
        }
    }
    return ret;
}   
#endif

#if 0
u_t Diverse::lr_count_reduced(u_t l, u_t r) const
{
    u_t ret = 0;
    vu_t counts(vu_t::size_type(n_reduced), 0);
    for (u_t i = l; i < r; ++i)
    {
        u_t t = trinkets_reduced[i];
        u_t nt = counts[t];
        if (nt < s)
        {
            ++ret;
        }
        else if (nt == s)
        {
            ret -= s;
        }
        counts[t] = nt + 1;
    }
    return ret;
}   

void Diverse::solve_naive()
{
    n_reduced = reduce();
    // set positions
    positions = vvu_t(vvu_t::size_type(n_reduced), vu_t());
    for (u_t i = 0; i < n; ++i)
    {
        u_t t = trinkets_reduced[i];
        positions[t].push_back(i);
    }
    setu_t starts;
    starts.insert(starts.end(), 0);
    u_t current = 0;
    for (u_t r = 0, r1 = 1; r < n; r = r1++)
    {
        u_t t = trinkets_reduced[r];
        const vu_t &tpos = positions[t];
        vu_t::const_iterator ti = lower_bound(tpos.begin(), tpos.end(), r);
        u_t tii = ti - tpos.begin();
        u_t next = 0;
        setu_t starts_next;
        for (u_t pass = 0; pass < 2; ++pass)
        {
            for (u_t start: starts)
            {
                u_t s_next = start, s_alt = start;
                u_t candid = 0;
                if ((tii >= s) && (tpos[tii - s] >= start))
                {
                    if ((tii > s) && (tpos[tii - s - 1] >= start))
                    {
                        candid = current;
                    }
                    else
                    {
                        candid = current - s;
                        s_alt = tpos[tii - s] + 1;
                        u_t candid_alt = lr_count_reduced(s_alt, r + 1);
                        if (candid_alt < candid)
                        {
                            s_alt = s_next; // ignore
                        }
                        else if (candid < candid_alt)
                        {
                            candid = candid_alt;
                            s_next = s_alt;
                        }
                        // else: ==  consuder both s_next, s_alt
                    }
                }
                else
                {
                    candid = current + 1;
                }
                if (pass == 0)
                {
                    maximize(next, candid);
                }
                else if (candid == next)
                {
                    starts_next.insert(starts_next.end(), s_next);
                    if (s_alt != start)
                    {
                        starts_next.insert(starts_next.end(), s_alt);
                    }
                }
            }
        }
        swap(starts, starts_next);
        current = next;
        maximize(solution, current);
    }
}
#endif

u_t Diverse::reduce()
{
    u2u_t t2i;
    trinkets_reduced.clear();
    trinkets_reduced.reserve(trinkets.size());
    for (u_t t: trinkets)
    {
        auto er = t2i.equal_range(t);
        int i;
        if (er.first == er.second)
        {
            i = t2i.size();
            t2i.insert(er.first, u2u_t::value_type(t, i));
        }
        else
        {
            i = (*er.first).second;
        }
        trinkets_reduced.push_back(i);
    }
    return t2i.size();
}

void Diverse::solve()
{
    n_reduced = reduce();
    init_level_nodes();
    maximize(solution, level_nodes.back().front().pfx_max);

    vu_t count(vu_t::size_type(n_reduced), 0);
    vu_t pre_pos(vu_t::size_type(n_reduced), u_t(-1));
    for (u_t i = 0; i < n; ++i)
    {
        u_t t = trinkets_reduced[i];
        u_t nt = count[t];
        const vu_t &tpositions = positions[t];
        u_t tsi = nt + s;
        node_update_up(i, 0);
        if (tsi < tpositions.size())
        {
            u_t si = tpositions[tsi];
            node_update_up(si, 1);
            if (tsi + 1 < tpositions.size())
            {
                u_t si1 = tpositions[tsi + 1];
                node_update_up(si1, -s);
            }
        }
        count[t] = nt + 1;
        maximize(solution, level_nodes.back().front().pfx_max);
    }
}

void Diverse::init_level_nodes()
{
    init_level_nodes_low();
    while (level_nodes.back().size() > 1)
    {
        const vnode_t nodes_low = level_nodes.back();
        u_t sz_low = nodes_low.size();
        u_t sz = (sz_low + 1) / 2;
        vnode_t nodes_up(vnode_t::size_type(sz), znode);
        for (u_t i = 0; i < sz; ++i)
        {
            u_t i0 = 2*i, i1 = i0 + 1;
            const Node &node_low0 = nodes_low[i0];
            const Node &node_low1 = (i1 < sz_low ? nodes_low[i1] : znode);
            Node &node_up = nodes_up[i];
            node_up.total = node_low0.total + node_low1.total;
            node_up.pfx_max = max(node_low0.pfx_max,
               node_low0.total + node_low1.pfx_max);
        }
        level_nodes.push_back(vnode_t());
        swap(level_nodes.back(), nodes_up);
    }
}

void Diverse::init_level_nodes_low()
{
    vnode_t low_nodes(vnode_t::size_type(n), znode);
    vu_t count(vu_t::size_type(n_reduced), 0);
    positions = vvu_t(vu_t::size_type(n_reduced), vu_t());
    for (u_t i = 0; i < n; ++i)
    {
        Node &node = low_nodes[i];
        u_t t = trinkets_reduced[i];
        u_t nt = count[t];
        if (nt < s)
        {
            node.total = node.pfx_max = 1;
        }
        else if (nt == s)
        {
            node.total = node.pfx_max = low_nodes[i].total = -s;
        }
        count[t] = nt + 1;
        positions[t].push_back(i);
    }
    level_nodes.push_back(vnode_t());
    swap(level_nodes.back(), low_nodes);
}

void Diverse::node_update_up(u_t ni, int val)
{
    level_nodes[0][ni] = Node(val, val);
    for (u_t level = 0, level1 = 1, nlevel = level_nodes.size();
        level1 < nlevel; level = level1++)
    {
        const vnode_t &curr_level_nodes = level_nodes[level];
        u_t ni_buddy = (ni ^ 1);
        if (ni_buddy < ni)
        {
            swap(ni, ni_buddy);
        }
        const Node &node = curr_level_nodes[ni];
        ni /= 2;
        if (ni_buddy < curr_level_nodes.size())
        {
            const Node &buddy = curr_level_nodes[ni_buddy];
            int up_totol = node.total + buddy.total;
            int up_pfx_max = max(node.pfx_max, node.total + buddy.pfx_max);
            level_nodes[level1][ni] = Node(up_totol, up_pfx_max);
        }
        else
        {
            level_nodes[level1][ni] = node;
        }
    }
}

void Diverse::print_solution(ostream &fo) const
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

    void (Diverse::*psolve)() =
        (naive ? &Diverse::solve_naive : &Diverse::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Diverse diverse(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (diverse.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        diverse.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
