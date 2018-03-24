#include "bipartite_match.h"
#include "maxflow.h"
#include <map>
#include <set>

using namespace std;

typedef set<unsigned> setu_t;
typedef map<unsigned, unsigned> u2u_t;

unsigned maps_set(u2u_t &c2g, u2u_t &g2c, const setu_t &s, unsigned gi)
{
    for (unsigned i: s)
    {
        c2g.insert(c2g.end(), u2u_t::value_type(i, gi));
        g2c.insert(g2c.end(), u2u_t::value_type(gi, i));
        ++gi;
    }
    return gi;
}

int bipartitee_max_match(lr_edges_t &match, const lr_edges_t &edges)
{
    setu_t lset, rset;
    for (const auto &e: edges)
    {
       lset.insert(e.first);
       rset.insert(e.second);
    }
    u2u_t l2g, r2g, g2l, g2r;
    unsigned eol = maps_set(l2g, g2l, lset, 0);
    unsigned eor = maps_set(r2g, g2r, rset, eol);
    unsigned source = eor;
    unsigned sink = source + 1;
    uu_edge_2_u_t flow, result_flow;
    for (unsigned i: lset)
    {
        uu_edge_t e(source, l2g[i]);
        flow.insert(uu_edge_2_u_t::value_type(e, 1));
    }
    for (const auto &e: edges)
    {
        uu_edge_t e1{l2g[e.first], r2g[e.second]};
        flow.insert(uu_edge_2_u_t::value_type(e1, 1));
    }
    for (unsigned i: rset)
    {
        uu_edge_t e(r2g[i], sink);
        flow.insert(uu_edge_2_u_t::value_type(e, 1));
    }
    unsigned total_flow;
    int ret = max_flow(total_flow, result_flow, flow, source, sink);
    if (ret >= 0)
    {
        for (const auto v: result_flow)
        {
            const uu_edge_t &ge = v.first;
            unsigned capacity = v.second;
            if ((ge.first < eol) && (eol <= ge.second) && (ge.second < eor) &&
                (capacity == 1))
            {
                uu_t e{g2l[ge.first], g2r[ge.second]};
                match.insert(e);
            }
        }
    }
    return ret;
}
