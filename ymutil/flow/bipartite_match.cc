#include "bipartite_match.h"
#include "maxflow.h"
#include <map>
#include <set>

using namespace std;

typedef unsigned u_t;
typedef set<u_t> setu_t;
typedef map<u_t, unsigned> u2u_t;

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
       lset.insert(e[0]);
       rset.insert(e[1]);
    }
    u2u_t l2g, r2g, g2l, g2r;
    u_t eol = maps_set(l2g, g2l, lset, 0);
    u_t eor = maps_set(r2g, g2r, rset, eol);
    u_t source = eor;
    u_t sink = source + 1;
    au2_2u_t flow, result_flow;
    for (u_t i: lset)
    {
        au2_t e{source, l2g[i]};
        flow.insert(au2_2u_t::value_type(e, 1));
    }
    for (const auto &e: edges)
    {
        au2_t e1{l2g[e[0]], r2g[e[1]]};
        flow.insert(au2_2u_t::value_type(e1, 1));
    }
    for (u_t i: rset)
    {
        au2_t e{r2g[i], sink};
        flow.insert(au2_2u_t::value_type(e, 1));
    }
    u_t total_flow = 0;
    bool ok = flow.empty() ||
        max_flow(total_flow, result_flow, flow, source, sink);
    if (ok)
    {
        for (const auto v: result_flow)
        {
            const au2_t &ge = v.first;
            u_t capacity = v.second;
            if ((ge[0] < eol) && (eol <= ge[1]) && (ge[1] < eor) &&
                (capacity == 1))
            {
                au2_t e{g2l[ge[0]], g2r[ge[1]]};
                match.insert(e);
            }
        }
    }
    int ret = ok ? total_flow : -1;
    return ret;
}
