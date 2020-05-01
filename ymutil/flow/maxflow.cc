#include "maxflow.h"
#include <queue>
#include <vector>

using namespace std;
typedef vector<u_t> vu_t;
typedef map<u_t, vu_t> u2vu_t;
typedef map<u_t, u_t> u2u_t;

class EdgeFlow
{
 public:
    EdgeFlow(u_t c=0, u_t r=0, u_t f=0, bool o=true): 
        capacity(c), residual_capcity(r), flow(f), original(o) {}
    u_t capacity;
    u_t residual_capcity;
    u_t flow;
    bool original;
};
typedef map<au2_t, EdgeFlow> au2_2ef_t;

class GFK
{
 public:
    GFK(const au2_2u_t& ucapacity, u_t _src, u_t _sink) :
        source(_src), sink(_sink)
    {
        build_graph(ucapacity);
        init_edges(ucapacity);
    }
    bool ford_fulkerson(u_t &total_flow, au2_2u_t& result_flow);
 private:
    void build_graph(const au2_2u_t& ucapacit);
    void init_edges(const au2_2u_t& ucapacit);
    bool get_augment_path(u_t& flow, vu_t& path) const;
    void augment_flow(u_t flow, const vu_t& path);
    void add_edge(const au2_t& edge);
    // const au2_2u_t& capacity;
    u_t source;
    u_t sink;
    u2vu_t graph;
    au2_2ef_t eflows;
};

bool GFK::ford_fulkerson(u_t &total_flow, au2_2u_t& result_flow)
{
    bool ok = true;
    u_t flow;
    vu_t path;
    while (get_augment_path(flow, path))
    {
        augment_flow(flow, path);
    }
    total_flow = 0;
    result_flow.clear();
    for (const au2_2ef_t::value_type& kv: eflows)
    {
        const EdgeFlow& ef = kv.second;
        if (ef.flow > 0)
        {
            const au2_t& edge = kv.first;
            if (edge[0] == source)
            {
                total_flow += ef.flow;
            }
            au2_2u_t::value_type v(edge, ef.flow);
            result_flow.insert(result_flow.end(), v);
        }
    }
    return ok;
}

void GFK::build_graph(const au2_2u_t& ucapacity)
{
    for (const au2_2u_t::value_type& kv: ucapacity)
    {
        const au2_t& edge = kv.first;
        add_edge(edge);
    }
}

void GFK::add_edge(const au2_t& edge)
{
    u_t u = edge[0];
    auto er = graph.equal_range(u);
    u2vu_t::iterator iter = er.first;
    if (er.first == er.second)
    {
        vu_t adjs;
        u2vu_t::value_type gkv(u, adjs);
        iter = graph.insert(iter, gkv);
    }
    vu_t& adjs = iter->second;
    adjs.push_back(edge[1]);
}

void GFK::init_edges(const au2_2u_t& ucapacity)
{
    for (const au2_2u_t::value_type& kv: ucapacity)
    {
        const au2_t& edge = kv.first;
        u_t capacity = kv.second;
        EdgeFlow ef(capacity, capacity, 0);
        eflows.insert(eflows.end(), au2_2ef_t::value_type(edge, ef));
    }
}

bool GFK::get_augment_path(u_t& flow, vu_t& path) const
{
    // BFS
    bool found = false;
    u2u_t parent;
    queue<u_t> q;
    q.push(source);
    while (!(q.empty() || found))
    {
        u_t node = q.front();
        q.pop();
        u2vu_t::const_iterator i_adjs = graph.find(node);
        static const vu_t empty_adjs;
        const vu_t& adjs = i_adjs == graph.end() ? empty_adjs : i_adjs->second;
        for (vu_t::const_iterator ai = adjs.begin(), ae = adjs.end();
            (ai != ae) && !found; ++ai)
        {
            const u_t a = *ai;
            if (parent.find(a) == parent.end())
            {
                const au2_t edge{node, a};
                const EdgeFlow& ef = eflows.find(edge)->second;
                if (ef.residual_capcity > 0)
                {
                    found = (a == sink);
                    parent.insert(parent.end(), u2u_t::value_type(a, node));
                    q.push(a);
                }
            }
        }
    }
    path.clear();
    if (found)
    {
        flow = u_t(-1); // infinite
        vu_t revpath;
        u_t v = sink;
        revpath.push_back(v);
        while (revpath.back() != source)
        {
            u_t p = parent.find(v)->second;
            au2_t edge{p, v};
            u_t pu_residual = eflows.find(edge)->second.residual_capcity;
            if (flow > pu_residual)
            {
                flow = pu_residual;
            }
            v = p;
            revpath.push_back(v);
        }
        path.insert(path.end(), revpath.rbegin(), revpath.rend());
    }
    return found;
}

void GFK::augment_flow(u_t flow, const vu_t& path)
{
    for (u_t i = 0, i1 = 1, e = path.size(); i1 != e; i = i1++)
    {
        const au2_t edge{path[i], path[i1]};
        EdgeFlow& ef = eflows.find(edge)->second;
        ef.residual_capcity -= flow;

        const au2_t redge{path[i1], path[i]};
        auto er = eflows.equal_range(redge);
        au2_2ef_t::iterator iter = er.first;
        if (er.first == er.second)
        {
            EdgeFlow ef_res(9, 9, 9, false);
            iter = eflows.insert(iter, au2_2ef_t::value_type(redge, ef_res));
            add_edge(redge);
        }
        EdgeFlow& rev_ef = iter->second;

        EdgeFlow* edge_orig = ef.original ? &ef : &rev_ef;
        edge_orig->flow += flow;
    }
}

bool max_flow(
    u_t& total_flow,
    au2_2u_t& result_flow,
    const au2_2u_t& flow,
    u_t source,
    u_t sink)
{
    GFK gfk(flow, source, sink);
    bool ret = gfk.ford_fulkerson(total_flow, result_flow);
    return ret;
}
