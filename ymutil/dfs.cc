#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

typedef unsigned u_t;
typedef array<u_t, 2> au2_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<au2_t> vau2_t;

typedef void (*visitor_t)(u_t node);

class Graph
{
 public:
    Graph(const vvu_t& _v_adjs) : v_adjs(_v_adjs) {}
    Graph(istream& is);
    void show(ostream& os) const;
    void show_results(ostream& os) const;
    void dfs(visitor_t cb);
    void dfs_norec(visitor_t cb);
    vvu_t v_adjs;
    vector<bool> visited;
    u_t timer;
    vu_t dtime;
    vu_t etime;
    vu_t p;
 private:
    void dfs_init();
    void dfs_visit(u_t v, visitor_t cb);
};

Graph::Graph(istream& is)
{
    u_t V, E;
    is >> V >> E;
    v_adjs.assign(V, vu_t());
    for (u_t e = 0; e < E; ++e)
    {
        u_t i, j;
        is >> i >> j;
        v_adjs[i].push_back(j);
        v_adjs[j].push_back(i);
    }
    for (vu_t& adjs: v_adjs)
    {
        sort(adjs.begin(), adjs.end());
    }
}

void Graph::dfs(visitor_t cb)
{
    const u_t V = v_adjs.size();
    dfs_init();
    for (u_t v = 0; v < V; ++v)
    {
        if (!visited[v])
        {
            dfs_visit(v, cb);
        }
    }
}

void Graph::dfs_init()
{
    const u_t V = v_adjs.size();
    visited.assign(V, false);
    timer = 0;
    dtime.assign(V, u_t(-1));
    etime.assign(V, u_t(-1));
    p.assign(V, u_t(-1));
}

void Graph::dfs_visit(u_t v, visitor_t cb)
{
    cb(v);
    visited[v] = true;
    dtime[v] = timer++;
    for (u_t a: v_adjs[v])
    {
        if (!visited[a])
        {
            p[a] = v;
            dfs_visit(a, cb);
        }
    }
    etime[v] = timer++;
}

void Graph::dfs_norec(visitor_t cb)
{
    const u_t V = v_adjs.size();
    dfs_init();
    for (u_t root = 0; root < V; ++root)
    {
        if (!visited[root])
        {
            vau2_t stack;
            stack.push_back(au2_t{root, 0});
            visited[root] = true;
            while (!stack.empty())
            {
                const u_t v = stack.back()[0];
                u_t ai = stack.back()[1];
                if (ai == 0)
                {
                    cb(v);
                    dtime[v] = timer++;
                }
                const vu_t& adjs = v_adjs[v];
                const u_t na = adjs.size();
                for ( ; (ai < na) && visited[adjs[ai]]; ++ai) {}
                if (ai < na)
                {
                    stack.back()[1] = ai + 1;
                    const u_t a = adjs[ai];
                    p[a] = v;
                    stack.push_back(au2_t{a, 0});
                    visited[a] = true;
                }
                else 
                {
                    stack.pop_back();
                    etime[v] = timer++;
                }
            }
        }
    }
}

void Graph::show(ostream& os) const
{
    os << "{\n";
    for (u_t v = 0; v < v_adjs.size(); ++v)
    {
        const vu_t& adjs = v_adjs[v];
        os << "  " << v << " :";
        for (u_t a: adjs)
        {
            os << ' ' << a;
        }
        os << '\n';
    }
    os << "}\n";
}

void Graph::show_results(ostream& os) const
{
    const u_t V = v_adjs.size();
    for (u_t v = 0; v < V; ++v)
    {
        const u_t parent = p[v];
        os << "[" << v << "] = P=";
        if (parent == u_t(-1))
        {
            os << '-';
        }
        else
        {
            os << parent;
        }
        os << ", DE(" << dtime[v] << ", " << etime[v] << ")\n";
    }
}

static void visit_print(u_t i)
{
    cout << __func__ << "(" << i << ")\n";
}

static int test_specific(const char *fn)
{
    int rc = 0;
    ifstream f(fn);
    Graph g(f);
    g.show(cout);
    cout << "DFS:\n";
    g.dfs(visit_print);
    g.show_results(cout);

    cout << "\nDFS non-recursive:\n";
    g.dfs_norec(visit_print);
    g.show_results(cout);

    return rc;    
}

static int test_random(int argc, char **argv)
{
    int rc = 0;
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;
    const string cmd(argv[1]);
    if (cmd == string("specific"))
    {
        rc = test_specific(argv[2]);
    }
    else if (cmd == "random")
    {
        rc = test_random(argc + 2, argv -2);
    }
    return rc;    
}
