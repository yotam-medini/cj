#include <array>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <vector>

using namespace std;

typedef unsigned int u_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;

u_t dbg_flags = 0;

// Implementing: 
// https://en.wikipedia.org/wiki/Hopcroft%E2%80%93Karp_algorithm#Pseudocode
class HopcroftKarp
{
 public:
    HopcroftKarp(const vau2_t& _edges) : caller_edges(_edges) {}
    const vau2_t& solve();
 private:
    typedef unordered_map<u_t, u_t> unord_u2u_t;
    static const u_t infty;
    void init();
    void init_indices(u_t ei, vu_t& indices, unord_u2u_t& xmap);
    bool bfs();
    bool dfs(u_t u);
    bool dfs_recursive(u_t u);
    const vau2_t& caller_edges;
    vau2_t edges;
    vvu_t adjs;
    vau2_t match;
    vu_t u_indices, v_indices;
    u_t usize, vsize;
    vu_t upair, vpair;
    u_t unil, vnil;
    vu_t dist;
};
const u_t HopcroftKarp::infty = u_t(-1);

const vau2_t& HopcroftKarp::solve()
{
    init();
    while (bfs())
    {
        for (u_t u = 0; u < usize; ++u)
        {
            if (upair[u] == vnil)
            {
                (void)(dbg_flags & 0x2 ? dfs_recursive(u) : dfs(u));
            }
        }
    }
    for (u_t u = 0; u < u_indices.size(); ++u)
    {
        const u_t v = upair[u];
        if (v != vnil)
        {
            match.push_back(au2_t{u_indices[u], v_indices[v]});
        }
    }
    return match;
}

void HopcroftKarp::init()
{
    unord_u2u_t umap, vmap;
    init_indices(0, u_indices, umap);
    init_indices(1, v_indices, vmap);
    unil = u_indices.size();
    vnil = v_indices.size();
    usize = u_indices.size(), vsize = v_indices.size(); // 'const' now on
    upair = vu_t(usize, vnil);
    vpair = vu_t(vsize, unil);
    dist = vu_t(usize + 1, infty);
    edges.reserve(caller_edges.size());
    adjs = vvu_t(usize, vu_t());
    for (const au2_t& ce: caller_edges)
    {
        const u_t u = umap[ce[0]], v = vmap[ce[1]];
        edges.push_back(au2_t{u, v});
        adjs[u].push_back(v);
    }
}

void HopcroftKarp::init_indices(u_t ei, vu_t& indices, unord_u2u_t& xmap)
{
    for (const au2_t& ce: caller_edges)
    {
        u_t x = ce[ei];
        unord_u2u_t::iterator iter = xmap.find(x);
        if (iter == xmap.end())
        {
            xmap.insert(iter, unord_u2u_t::value_type(x, indices.size()));
            indices.push_back(x);
        }        
    }
}

bool HopcroftKarp::bfs()
{
    deque<u_t> q;
    fill(dist.begin(), dist.end(), infty);
    for (u_t u = 0; u < usize; ++u)
    {
        if (upair[u] == vnil)
        {
            dist[u] = 0;
            q.push_back(u);
        }
    }
    while (!q.empty())
    {
        const u_t u = q.front(); q.pop_front();
        if (dist[u] < dist[unil])
        {
            const vu_t& u_adjs = adjs[u];
            for (u_t v: u_adjs)
            {
                const u_t u_next = vpair[v];
                if (dist[u_next] == infty)
                {
                    dist[u_next] = dist[u] + 1;
                    q.push_back(u_next);
                }
            }
        }
    }
    bool found = (dist[unil] != infty);
    return found;
}

bool HopcroftKarp::dfs(u_t u)
{
    bool found = false;
    vau2_t stack; // (u, i_adj)
    stack.push_back(au2_t{u, 0});
    while (!stack.empty())
    {
        u = stack.back()[0];
        if (u == unil)
        {
            found = true;
            stack.pop_back();
            while (!stack.empty())
            {
                const u_t usb = stack.back()[0];
                const u_t i_adj = stack.back()[1];
                const u_t vsb = adjs[usb][i_adj];
                upair[usb] = vsb;
                vpair[vsb] = usb;
                stack.pop_back();
                found = true;
            }
        }
        else
        {
            u_t& i_adj = stack.back()[1];
            const vu_t& u_adjs = adjs[u];
            if (i_adj < u_adjs.size())
            {
                const u_t v = u_adjs[i_adj];
                const u_t u_next = vpair[v];
                if (dist[u_next] == dist[u] + 1)
                {
                    stack.push_back(au2_t{u_next, 0});
                }
                else
                {
                    ++i_adj;     
                }
            }
            else
            {
                dist[u] = infty;
                stack.pop_back();
            }
        }
    }
    return found;
}

bool HopcroftKarp::dfs_recursive(u_t u)
{
    bool found = (u == unil);
    if (!found)
    {
        const vu_t& u_adjs = adjs[u];
        const u_t na = u_adjs.size();
        for (u_t vi = 0; (vi < na) && !found; ++vi)
        {
            const u_t v = u_adjs[vi];
            const u_t u_next = vpair[v];
            if (dist[u_next] == dist[u] + 1)
            {
                found = dfs_recursive(u_next);
                if (found)
                {
                    upair[u] = v;
                    vpair[v] = u;
                }
            }
        }
        if (!found)
        {
            dist[u] = infty;
        }
    }
    return found;
}

void hopcroft_karp(vau2_t& maximal_match, const vau2_t& edges)
{
    HopcroftKarp hk(edges);
    maximal_match = hk.solve();
}

#if defined(HOPCROFT_KARP_TEST)

#include <iostream>
#include <cstdlib>
using namespace std;

static void usage(const char *pn)
{
    cerr << "Usage: " << pn << " <l0 r0> <l1 r1> ...\n";
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && ((argc -1) % 2) == 0) ? 0 : 1;
    if (rc == 0)
    {
        int ai = 1;
        if (string(argv[ai]) == string("-debug"))
        {
            dbg_flags = strtoul(argv[++ai], nullptr, 0);
            ++ai;
        }
        vau2_t edges;
        for ( ; ai < argc; ai += 2)
        {
            u_t l = strtoul(argv[ai + 0], 0, 0);
            u_t r = strtoul(argv[ai + 1], 0, 0);
            au2_t e{l, r};
            edges.push_back(e);
        }
        vau2_t match;
        hopcroft_karp(match, edges);
        cout << "match[" << match.size() << "]:\n";
        for (au2_t& m: match)
        {
            cout << "  " << m[0] << " " << m[1] << "\n";
            
        }
    }
    if (rc) { usage(argv[0]); }
    return rc;
}
#endif
