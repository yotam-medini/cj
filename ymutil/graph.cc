// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

using namespace std;

typedef unsigned u_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;

class VertexCallback
{
 public:
    virtual void call(u_t v) {}
};

class PushCallback : public VertexCallback
{
 public:
    PushCallback(vu_t& _a) : a(_a) {}
    void call(u_t v) { a.push_back(v); }
 private:
    vu_t& a;
};

class Node
{
 public:
    Node(const vu_t& _adjs=vu_t()) : adjs(_adjs), color(0), data(0) {}
    vu_t adjs;
    u_t color;
    u_t data;
};
typedef vector<Node> vnode_t;

class Graph
{
 public:
    Graph(const vvu_t& _vadjs)
    {
        const u_t sz = _vadjs.size();
        nodes.reserve(sz);
        while (nodes.size() < sz)
        {
            nodes.push_back(Node(_vadjs[nodes.size()]));
        }
    }
    const vnode_t& get_nodes() const { return nodes; }
    void topological_sort(vu_t& vertices);
    void dag_get_strong_components(vvu_t& components);
 private:
#if 0
    typedef void (*vcall_t)(u_t v);
    void dfs(vcall_t vc=([](u_t v) -> void {}));
    void dfs_visit(u_t v, vcall_t vc);
#endif
    void dfs(VertexCallback* pvc=nullptr);
    void dfs_visit(u_t v, VertexCallback* pvc);
    void set_nodes_color(u_t color)
    {
        for (Node& node: nodes) { node.color = color; }
    }
    vnode_t nodes;
};

void Graph::topological_sort(vu_t& vertices)
{
    vertices.clear();
    vertices.reserve(nodes.size());
    // dfs([vertices](u_t v) { vertices.push_back(v); });
    
    PushCallback pusher(vertices);
    dfs(&pusher);
}

void::Graph::dfs(VertexCallback* pvc)
{
    set_nodes_color(0);
    const u_t sz = nodes.size();
    for (u_t v = 0; v < sz; ++v)
    {
        if (nodes[v].color == 0)
        {
            dfs_visit(v, pvc);
        }
    }
}

void::Graph::dfs_visit(u_t v, VertexCallback* pvc)
{
    Node& node = nodes[v];
    node.color = 1;
    for (u_t a: node.adjs)
    {
        if (nodes[a].color == 0)
        {
            dfs_visit(a, pvc);
        }
    }
    if (pvc) { pvc->call(v); }
    node.color = 2;
}

void::Graph::dag_get_strong_components(vvu_t& components)
{
    vu_t tsorted;
    topological_sort(tsorted);
    //cerr << "sorted:"; for (u_t v: tsorted) { cerr << ' ' << v; } cerr << '\n';

    const u_t sz =  nodes.size();
    vvu_t rev_adjs(sz, vu_t());
    for (u_t v = 0; v < sz; ++v)
    {
        for (u_t a: nodes[v].adjs)
        {
            rev_adjs[a].push_back(v);
        }
    }

    Graph rev_graph(rev_adjs);
    rev_graph.set_nodes_color(0);
    components.clear();
    for (u_t ti = sz; ti > 0; )
    {
        u_t v = tsorted[--ti];
        if (rev_graph.nodes[v].color == 0)
        {
            vu_t component;
            PushCallback pusher(component);
            rev_graph.dfs_visit(v, &pusher);
            sort(component.begin(), component.end());
            components.push_back(component);
        }
    }
    sort(components.begin(), components.end());
}

void dag_get_strong_components(const vvu_t& g, vvu_t& components)
{
    Graph graph(g);
    graph.dag_get_strong_components(components);
}

static void fget_vvu(istream& fin, vvu_t& aa)
{
    aa.clear();
    u_t sz;
    fin >> sz;
    aa.clear(); aa.reserve(sz);
    while (aa.size() < sz)
    {
        u_t na;
        fin >> na;
        vu_t a;
        copy_n(istream_iterator<u_t>(fin), na, back_inserter(a));
        aa.push_back(a);
    }
}

static int test_dag_get_strong_components(int argc, char** argv)
{
    int rc = 0;
    if (argc == 1)
    {
        ifstream fin(argv[0]);
        u_t n_tests;
        fin >> n_tests;
        for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
        {
            cerr << "tested: " << ti << '/' << n_tests << '\n';
            vvu_t g, comps, comps_expected;
            fget_vvu(fin, g);
            fget_vvu(fin, comps_expected);
            dag_get_strong_components(g, comps);
            if (comps == comps_expected)
            {
                cerr << "  ... #(g)=" << g.size() << " #(strong components)=" <<
                    comps.size() << '\n';
            }
            else
            {
                cerr << "  failed: unexpected result. "
                    "#(g)=" << g.size() << ", #(components)=" << comps.size() <<
                    ", expected: " << comps_expected.size() << '\n';
                rc = 1;
            }
        }
    }
    else
    {
        cerr << __func__ << " missing argument\n";
        rc = 1;
    }
    return rc;
}

static void usage(const char *p0)
{
    cerr << p0 << '\n' <<
        "  strong_components <in> <out>\n"
        ;
}

int main(int argc, char** argv)
{
    int rc = 0;
    const string cmd(1 < argc ? argv[1] : "");
    if (cmd == string("strong_components"))
    {
        rc = test_dag_get_strong_components(argc - 2, argv + 2);
    }
    else
    {
        usage(argv[0]);
        rc = 1;
    }
    return rc;
}
