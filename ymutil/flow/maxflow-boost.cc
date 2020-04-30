#include "maxflow-boost.h"
#include <iostream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/edmonds_karp_max_flow.hpp>

using namespace std;
using namespace boost;

class MaxFlow
{
 public:
    MaxFlow(const uu_edge_2_u_t &flow, unsigned vsource, unsigned vsink);
    bool compute(unsigned &total_flow, uu_edge_2_u_t &result_flow);
    bool ok() const { return _ok; }
    unsigned nvertices() const { return call_idx.size(); }

 private:
    typedef map<unsigned, unsigned> u2u_t;
    typedef set<unsigned> setu_t;
    typedef adjacency_list_traits <vecS, vecS, directedS> Traits;
    typedef adjacency_list <
        listS,
        vecS,
        directedS,
        property <vertex_name_t, string>,
        property <
            edge_capacity_t,
            long,
            property <
                edge_residual_capacity_t,
                long,
                property <edge_reverse_t, Traits::edge_descriptor>
            >
        >
    > flow_graph_t;
    typedef graph_traits<flow_graph_t>::vertex_descriptor vdesc_t;
    typedef graph_traits<flow_graph_t>::edge_descriptor edesc_t;
    typedef property_map <flow_graph_t, edge_capacity_t>::type cap_t;
    typedef property_map <flow_graph_t, edge_reverse_t>::type erev_t;
    typedef property_map <flow_graph_t, edge_residual_capacity_t>::type
        res_cap_t;

    void build();

    bool _ok;
    const uu_edge_2_u_t flow;
    unsigned source;
    unsigned sink;
    vdesc_t gsource;
    vdesc_t gsink;

    flow_graph_t g;
    cap_t capacity;
    erev_t erev;
    res_cap_t residual_cap;    

    setu_t call_idx;
    u2u_t caller2g;
    u2u_t g2caller;
};

MaxFlow::MaxFlow(const uu_edge_2_u_t &vflow, unsigned vsource, unsigned vsink) :
    _ok(true),
    flow(vflow),
    source(vsource),
    sink(vsink)
{
    for (const auto &i: flow)
    {
        const uu_edge_t &e = i.first;
        call_idx.insert(e.first);
        call_idx.insert(e.second);
    }

    if (!flow.empty())
    {
        _ok = _ok && (call_idx.find(source) != call_idx.end());
        _ok = _ok && (call_idx.find(sink) != call_idx.end());
    }
    if (_ok)
    {
        build();
    }
}

void MaxFlow::build()
{
    unsigned gi = 0;
    for (const auto &ci: call_idx)
    {
        caller2g.insert(u2u_t::value_type(ci, gi));
        g2caller.insert(u2u_t::value_type(gi, ci));
        ++gi;
    }

    gsource = caller2g[source];
    gsink = caller2g[sink];

    capacity = get(edge_capacity, g);
    erev = get(edge_reverse, g);
    residual_cap = get(edge_residual_capacity, g);

    for (const auto &e2c: flow)
    {
        const uu_edge_t &e = e2c.first;
        unsigned cap = e2c.second;
        unsigned gs = caller2g[e.first];
        unsigned gt = caller2g[e.second];
        edesc_t efwd, ebck;
        bool    infwd, inbck;        
        tie(efwd, infwd) = add_edge(gs, gt, g);
        tie(ebck, inbck) = add_edge(gt, gs, g);
        _ok = _ok && infwd && inbck;
        if (_ok)
        {
            capacity[efwd] = cap;
            capacity[ebck] = 0;
            erev[efwd] = ebck;
            erev[ebck] = efwd;
        }
    }
}

bool MaxFlow::compute(unsigned &total_flow, uu_edge_2_u_t &result_flow)
{
    total_flow = edmonds_karp_max_flow(g, gsource, gsink);
    for (unsigned gv = 0, nv = nvertices(); gv != nv; ++gv)
    {
        graph_traits<flow_graph_t>::out_edge_iterator ei, e_end;
        const unsigned cv = g2caller[gv];
        for (tie(ei, e_end) = out_edges(gv, g); ei != e_end; ++ei)
        {
            const auto &ge = *ei;
            unsigned ecap = capacity[ge];
            unsigned erescap = residual_cap[ge];
            unsigned egt = ge.m_target;
            unsigned cet = g2caller[egt];
            unsigned res_cap = (ecap >= erescap ? ecap - erescap : 0);
            // cerr << "egt="<< egt << ", cet="<<cet << "\n";
            uu_edge_t ce{cv, cet};
            uu_edge_2_u_t::value_type vres{ce, res_cap};
            result_flow.insert(result_flow.end(), vres);
#if 0
            cerr << "ge="<<ge << "\n";
            cerr  << ", cap="<<ecap << ", res="<<erescap << "\n";
            cerr << "ge="<<ge << ", cap="<<ecap << ", res="<<erescap << "\n";
            cerr << "ce=["<<ce.first << ", "<<ce.second<<"]" <<
                ", cap="<<ecap << ", res="<<erescap << "\n";
#endif
        }
    }
    return _ok;
}

bool max_flow(
    unsigned &total_flow,
    uu_edge_2_u_t &result_flow,
    const uu_edge_2_u_t &flow,
    unsigned source,
    unsigned sink)
{
    MaxFlow mf{flow, source, sink};

    bool ok = mf.ok();
    if (ok) 
    {
        ok = mf.compute(total_flow, result_flow);
    }
    return ok;
}
