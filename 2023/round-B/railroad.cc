// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_set>
#include <utility>
#include <set>
#include <tuple>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef deque<u_t> dqu_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef set<u_t> setu_t;
typedef unordered_set<u_t> hsetu_t;
typedef vector<hsetu_t> vhsetu_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;

static unsigned dbg_flags;

class GraphBase
{
 public:
    virtual void clear() {}
    u_t size() const { return n_vertices(); }
    virtual u_t n_vertices() const = 0;
    virtual u_t n_adjacents(u_t vertex) const = 0;
    virtual void reset_usage() = 0;
    u_t vertex_adj_index_begin(u_t vertex) const
    {
        u_t ai = 0;
        vertex_adj_index_advance(vertex, ai);
        return ai;
    }
    u_t vertex_adj_index_end(u_t vertex) const
    {
        return n_adjacents(vertex);
    }
    virtual u_t vertex_adj_index_to(
        u_t vertex,
        u_t index,
        bool set_used=true) = 0;
    void vertex_adj_index_next(u_t vertex, u_t& adj_index)
    {
        const u_t e = vertex_adj_index_end(vertex);
        if (adj_index < e)
        {
            ++adj_index;
            vertex_adj_index_advance(vertex, adj_index);
        }
    }
 protected:
    virtual bool vertex_adj_index_used(u_t vertex, u_t adj_index) const = 0;
    void vertex_adj_index_advance(u_t vertex, u_t& adj_index) const
    {
        const u_t e = vertex_adj_index_end(vertex);
        for (; (adj_index < e) && vertex_adj_index_used(vertex, adj_index);
            ++adj_index) {}
    }
};

class ToUsed
{
 public:
    ToUsed(u_t _to=0, u_t _ai_back=0, bool _used=false) :
        to(_to), ai_back(_ai_back), used(_used) {}
    u_t to;
    u_t ai_back;
    bool used;
};
typedef vector<ToUsed> vtoused_t;
typedef vector<vtoused_t> vvtoused_t;

class Graph : public GraphBase
{
 public:
    void clear() { v_adjs.clear(); }
    u_t n_vertices() const { return v_adjs.size(); }
    u_t n_adjacents(u_t vertex) const { return v_adjs[vertex].size(); }
    void reset_usage();
    u_t vertex_adj_index_to(u_t vertex, u_t adj_index, bool set_used=true);
    bool vertex_adj_index_used(u_t vertex, u_t adj_index) const
    {
        return v_adjs[vertex][adj_index].used;
    }
    vvtoused_t v_adjs;
    void show() const;
};

void Graph::reset_usage()
{
    for (vtoused_t& adjs: v_adjs)
    {
        for (ToUsed& a: adjs)
        {
            a.used = false;
        }
    }
}

u_t Graph::vertex_adj_index_to(u_t vertex, u_t index, bool set_used)
{
    ToUsed& a = v_adjs[vertex][index];
    if (set_used)
    {
        a.used = true;
        vtoused_t& to_adjs = v_adjs[a.to];
        ToUsed& toa = to_adjs[a.ai_back];
        toa.used = true;
    }
    return a.to;
}

void Graph::show() const
{
    for (u_t v = 0; v < n_vertices(); ++v) {
         cerr << "  " << v << ":";
         for (const ToUsed& tu: v_adjs[v]) {
             cerr << ' ' << tu.to << " (<@" << tu.ai_back << ")";
         }
         cerr << '\n';
    }
    cerr << "}\n";
}

class ToLine : public ToUsed
{
 public:
    ToLine(u_t _to=0, u_t _line=0, u_t _ai_back=0, bool _used=false) :
        ToUsed(_to, _ai_back, _used), line(_line) {}
    tuple<u_t, u_t> as_tuple3() const
    {
       return make_tuple(to, line);
    }
    u_t line;
};
typedef vector<ToLine> vtoline_t;
typedef vector<vtoline_t> vvtoline_t;

class GraphMultiEdge : public GraphBase
{
 public:
    void clear() { v_adjs.clear(); }
    u_t n_vertices() const { return v_adjs.size(); }
    u_t n_adjacents(u_t vertex) const { return v_adjs[vertex].size(); }
    void reset_usage();
    u_t vertex_adj_index_to(u_t vertex, u_t index, bool set_used=true);
    bool vertex_adj_index_used(u_t vertex, u_t adj_index) const
    {
        return v_adjs[vertex][adj_index].used;
    }
    vvtoline_t v_adjs;
};

void GraphMultiEdge::reset_usage()
{
   for (vtoline_t& adjs: v_adjs)
   {
       for (ToLine& a: adjs)
       {
            a.used = false;
       }
   }
}

u_t GraphMultiEdge::vertex_adj_index_to(u_t vertex, u_t index, bool set_used)
{
    ToLine& a = v_adjs[vertex][index];
    if (set_used)
    {
        a.used = true;
        vtoline_t& to_adjs = v_adjs[a.to];
        ToLine& toa = to_adjs[a.ai_back];
        toa.used = true;
    }
    return a.to;
}

class Railroad;
using p_add_bridge_t = void (Railroad::*)(u_t p, u_t v, u_t ai); 
using p_add_cut_t = void (Railroad::*)(u_t v); 

class Railroad
{
 public:
    Railroad(istream& fi);
    Railroad(u_t _N, const vvu_t& _S) :
        N(_N), L(_S.size()), S(_S), solution(0),
        n_unique_vertices(0), n_bridges(0)
    {
        K.reserve(L);
        for (const vu_t& line: S) { K.push_back(line.size()); }
    }
    void solve_naive();
    void solve_v1();
    void solve();
    void print_solution(ostream&) const;
    u_t get_solution() const { return solution; }
 private:
    // typedef void (Railroad::*p_add_bridge_t)(u_t p, u_t v); 
    void build_graph_without_line(vvu_t& g, u_t l);
    bool is_graph_connected(const vvu_t& g);
    void add_essential_via_unique_stations();
    void add_essential_via_edges();
    void add_essential_via_lines();
    void dfs_get_bridges(
        GraphBase& graph,
        p_add_bridge_t add_bridge,
        p_add_cut_t add_cut);
    void essential_add_via_edge(u_t p, u_t v, u_t pai);
    void essential_add_via_line(u_t l);
    void essential_add_via_node_line(u_t l)
    {
        if (l < L)
        {
            essential_lines.insert(l);
        }
    }
    void set_rich_lines();

    u_t N, L;
    vu_t K;
    vvu_t S;
    u_t solution;

    // solve_v1
    vector<bool> v_unique;
    vu_t rich_lines; // with more than one non-unique station
    hsetu_t essential_lines;
    GraphMultiEdge graph_vertices_lines;
    Graph graph_lines;
    // stats
    u_t n_unique_vertices;
    u_t n_bridges;;

    // solve
    Graph graph_lines_nodes;
};

Railroad::Railroad(istream& fi) :
    solution(0),
    n_unique_vertices(0), n_bridges(0)
{
    fi >> N >> L;
    K.reserve(L);
    S.reserve(L);
    while (K.size() < L)
    {
        u_t k;
        fi >> k;
        K.push_back(k);
        vu_t s;
        copy_n(istream_iterator<u_t>(fi), k, back_inserter(s));
        S.push_back(s);
    }
}

void Railroad::solve_naive()
{
    for (u_t i = 0; i < L; ++i)
    {
        vvu_t graph;
        build_graph_without_line(graph, i);
        if (!is_graph_connected(graph))
        {
            ++solution;
        }
    }
}

bool Railroad::is_graph_connected(const vvu_t& graph)
{
    // BFS    
    vector<bool> visited(N, false);
    dqu_t q;
    q.push_back(0); 
    visited[0] = true;
    u_t n_visited = 1;
    while (!q.empty())
    {
        u_t station = q.front();
        q.pop_front();
        for (u_t nbr: graph[station])
        {
            if (!visited[nbr])
            {
                q.push_back(nbr);
                visited[nbr] = true;
                ++n_visited;
            }
        }
    }
    bool connected = (n_visited == N);
    return connected;
}

void Railroad::build_graph_without_line(vvu_t& graph, u_t skip_line)
{
    vhsetu_t station_stations(N, hsetu_t());
    for (u_t l = 0; l < L; ++l)
    {
        const vu_t& line = S[l];
        for (u_t i = (l != skip_line ? 0 : N); i < line.size(); ++i)
        {
            const u_t s0 = line[i] - 1;
            for (u_t j = i + 1; j < line.size(); ++j)
            {
                const u_t s1 = line[j] - 1;
                station_stations[s0].insert(s1);
                station_stations[s1].insert(s0);
            }
        }
    }
    graph.reserve(N);
    for (u_t i = 0; i < N; ++i)
    {
         const hsetu_t& hnbrs = station_stations[i];
         vu_t nbrs(hnbrs.begin(), hnbrs.end());
         sort(nbrs.begin(), nbrs.end());
         graph.push_back(nbrs);
    }
    if (dbg_flags & 0x1) {
        cerr << "graph: {\n";
        for (u_t s = 0; s < L; ++s)
        {
             cerr << "  " << s << ":";
             for (u_t ns: graph[s]) { cerr << ' ' << ns; }
             cerr << '\n';
        }
        cerr << "}\n";
    }
}

void Railroad::solve_v1()
{
    v_unique.assign(N, false);
    add_essential_via_unique_stations();
    if (dbg_flags & 0x4) { cerr << "#(unique(V)="<<n_unique_vertices <<
        ", #(essentials)=" << essential_lines.size() << '\n'; }
    add_essential_via_edges();
    if (dbg_flags & 0x4) { cerr << "#(bridges(V)="<<n_bridges <<
        ", #(essentials)=" << essential_lines.size() << '\n'; }

    set_rich_lines();
    add_essential_via_lines();
    solution = essential_lines.size();
}

void Railroad::add_essential_via_unique_stations()
{
    vvu_t station_lines(N, vu_t()); // 0-based
    for (u_t l = 0; l < L; ++l)
    {
        const vu_t& line = S[l];
        for (u_t station1: line)
        {
            const u_t station = station1 - 1;
            station_lines[station].push_back(l);
        }
    }
    for (u_t station = 0; station < N; ++station)
    {
        const vu_t& lines = station_lines[station];
        if (lines.size() == 1)
        {
            essential_lines.insert(lines[0]);
            v_unique[station] = true;
            ++n_unique_vertices;
        }
    }
}

void Railroad::add_essential_via_edges()
{
    GraphMultiEdge& graph = graph_vertices_lines;
    graph.v_adjs.assign(N, vtoline_t());
    for (u_t l = 0; l < L; ++l)
    {
        const vu_t& line = S[l];
        for (u_t i = 0, j = 1; j  < line.size(); i = j++)
        {
            const u_t s0 = line[i] - 1, s1 = line[j] - 1;
            const u_t sz0 = graph.v_adjs[s0].size();
            const u_t sz1 = graph.v_adjs[s1].size();
            graph.v_adjs[s0].push_back(ToLine(s1, l, sz1));
            graph.v_adjs[s1].push_back(ToLine(s0, l, sz0));
        }
    }
    if (dbg_flags & 0x1) {
        cerr << "Edge Lines graph: {\n";
        for (u_t v = 0; v < N; ++v) {
             cerr << "  " << v << ":";
             for (const ToLine& tl: graph.v_adjs[v]) {
                 cerr << " (" << tl.to << ", @" << tl.line << 
                     " <@" << tl.ai_back << ")";
             }
             cerr << '\n';
        }
        cerr << "}\n";
    }
    dfs_get_bridges(graph, &Railroad::essential_add_via_edge, nullptr);
}

void Railroad::add_essential_via_lines()
{
    vvu_t station_lines(N, vu_t()); // 0 based
    for (u_t l = 0; l < L; ++l)
    {
        const vu_t& line = S[l];
        for (u_t station1: line)
        {
            const u_t station = station1 - 1;
            if (!v_unique[station])
            {
                station_lines[station].push_back(l);
            }
        }
    }
    graph_lines.v_adjs.assign(L, vtoused_t());
    for (u_t l = 0; l < L; ++l)
    {
        hsetu_t all_adjs;
        const vu_t& line = S[l];
        for (u_t station1: line)
        {
            const u_t station = station1 - 1;
            if (!v_unique[station])
            {
                const vu_t& slines = station_lines[station];
                all_adjs.insert(slines.begin(), slines.end());
            }
        }
        vtoused_t& adjs = graph_lines.v_adjs[l];
        for (u_t to: all_adjs)
        {
            if (l < to)
            {
                vtoused_t& adjs_to = graph_lines.v_adjs[to];
                const u_t ai = adjs.size();
                const u_t ai_back = adjs_to.size();
                adjs.push_back(ToUsed(to, ai_back));
                adjs_to.push_back(ToUsed(l, ai));
            }
        }
    }
    if (dbg_flags & 0x2) {
        cerr << "Lines graph: {\n";
        graph_lines.show();
    }
    dfs_get_bridges(graph_lines, nullptr, &Railroad::essential_add_via_line);
}

void Railroad::dfs_get_bridges(
    GraphBase& graph,
    p_add_bridge_t add_bridge,
    p_add_cut_t add_cut)
{
    const u_t V = graph.size();
    vector<bool> dfs_visited(V, false);
    vu_t parent(V, u_t(-1));
    vu_t parent_ai(V, u_t(-1)); // adjacent index with parent
    u_t timer = 0;
    vu_t dtime(V, u_t(-1));
    vu_t etime(V, u_t(-1));
    vu_t low(V, u_t(-1));
    vu_t children(V, 0);

    // Non-recursive DFS
    vau2_t v_ai_stack;
    v_ai_stack.push_back(au2_t{0, 0});
    dfs_visited[0] = true;
    while (!v_ai_stack.empty())
    {
        const u_t v = v_ai_stack.back()[0];
        u_t ai = v_ai_stack.back()[1];
        if (ai == 0)
        {
            ai = graph.vertex_adj_index_begin(v);
            dtime[v] = low[v] = timer++;
        }
        const u_t ai_end = graph.vertex_adj_index_end(v);
        if (ai < ai_end)
        {
            graph.vertex_adj_index_next(v, v_ai_stack.back()[1]);
            const u_t a = graph.vertex_adj_index_to(v, ai);
            if (dfs_visited[a])
            {
                low[v] = min(low[v], dtime[a]);
            }
            else
            {
                parent[a] = v;
                parent_ai[a] = ai;
                v_ai_stack.push_back(au2_t{a, 0});
                dfs_visited[a] = true;
                ++children[v];
            }
        }
        else 
        {
            const u_t p = parent[v];
            if (p != u_t(-1))
            {
                low[p] = min(low[p], low[v]);
                if (add_bridge && (dtime[p] < low[v]))
                {
                    (this->*add_bridge)(p, v, parent_ai[v]);
                }
                if (add_cut && (parent[p] != u_t(-1)) && (dtime[p] <= low[v]))
                {
                    (this->*add_cut)(p);
                }
            }
            v_ai_stack.pop_back();
            etime[v] = timer++;
        }
    }
    if (add_cut && (children[0] > 1))
    {
        (this->*add_cut)(0);
    }
}

void Railroad::essential_add_via_edge(u_t p, u_t v, u_t ai)
{
    const vtoline_t& adjs = graph_vertices_lines.v_adjs[p];
    u_t line = adjs[ai].line;
    essential_lines.insert(line);
    ++n_bridges;
}

void Railroad::essential_add_via_line(u_t l)
{
    essential_lines.insert(l);
}

void Railroad::set_rich_lines()
{
    rich_lines.clear();
    for (u_t l = 0; l < L; ++l)
    {
        const vu_t& line = S[l];
        u_t n_non_unique = 0;
        for (u_t station1: line)
        {
            const u_t station = station1 - 1;
            if (!v_unique[station])
            {
                ++n_non_unique;
            }
        }
        if (n_non_unique > 1)
        {
            rich_lines.push_back(l);
        }
    }
}

void Railroad::solve()
{
    Graph& g = graph_lines_nodes;
    g.v_adjs.assign(L + N, vtoused_t());
    for (u_t l = 0; l < L; ++l)
    {
        const vu_t& line = S[l];
        g.v_adjs[l].reserve(line.size());
        for (u_t s1: line)
        {
            const u_t s = L + (s1 - 1);
            const u_t lsz = g.v_adjs[l].size();
            const u_t ssz = g.v_adjs[s].size();
            g.v_adjs[l].push_back(ToUsed(s, ssz));
            g.v_adjs[s].push_back(ToUsed(l, lsz));       
        }
    }
    if (dbg_flags & 0x2) {
        cerr << "Lines graph: {\n";
        g.show();
    }
    dfs_get_bridges(g, nullptr, &Railroad::essential_add_via_node_line);
    solution = essential_lines.size();
}

void Railroad::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int real_main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    u_t solve_ver = 0;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-solve1"))
        {
            solve_ver = 1;
        }
        else if (opt == string("-debug"))
        {
            dbg_flags = strtoul(argv[++ai], nullptr, 0);
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

    void (Railroad::*psolve)() =
        (naive ? &Railroad::solve_naive : &Railroad::solve);
    if (solve_ver == 1) { psolve = &Railroad::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Railroad railroad(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (railroad.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        railroad.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static u_t rand_range(u_t nmin, u_t nmax)
{
    u_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

static void save_case(const char* fn, u_t N, const vvu_t& S)
{
    ofstream f(fn);
    const u_t L = S.size();
    f << "1\n" << N << ' ' << L << '\n';
    for (const vu_t& line: S)
    {
        f << line.size();
        char sep = '\n';
        for (u_t station: line)
        {
            f << sep << station;
            sep = ' ';
        }
        f << '\n';
    }
    f.close();
}

static int test_case(u_t N, const vvu_t& S)
{
    int rc = 0;
    const u_t L = S.size();
    u_t solution(-1), solution_naive(-1);
    bool small = (N + L < 0x20);
    if (dbg_flags & 0x100) { save_case("railroad-curr.in", N, S); }
    if (small)
    {
        Railroad p(N, S);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Railroad p(N, S);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("railroad-fail.in", N, S);
    }
    if (rc == 0) { cerr << "  N="<<N << ", L=" << L <<
        (small ? " (small) " : " (large) ") << " --> " <<
        solution << '\n'; }
    return rc;
}

static void lines_to_graph(vvu_t& graph, vu_t& unused, u_t N, const vvu_t& S)
{
    vhsetu_t v_adjs(N, hsetu_t());
    for (const vu_t& line: S)
    {
        for (u_t i = 0, j = 1; j < line.size(); i = j++)
        {
            u_t s0 = line[i] - 1;
            u_t s1 = line[j] - 1;
            v_adjs[s0].insert(s1);
            v_adjs[s1].insert(s0);
        }
    }
    for (u_t station = 0; station < N; ++station)
    {
        const hsetu_t& adjs_set =  v_adjs[station];
        if (adjs_set.empty())
        {
            unused.push_back(station + 1); // 1-based
        }
        else
        {
            vu_t& adjs =  graph[station];
            adjs.insert(adjs.end(), adjs_set.begin(), adjs_set.end());
            sort(adjs.begin(), adjs.end());
        }
    }
}

static void get_components(
    vvu_t& components,
    vu_t& unused,
    u_t N,
    const vvu_t& S)
{
    vhsetu_t v_adjs(N, hsetu_t());
    vvu_t graph(N, vu_t()); // -> zero-based
    lines_to_graph(graph, unused, N, S);
    vector<bool> visited(N, false);
    components.clear();
    for (u_t root = 0; root < N; ++root)
    {
        if (!visited[root])
        {
            components.push_back(vu_t());
            vu_t& component = components.back();
            dqu_t q;
            q.push_back(root);
            visited[root] = true;
            while (!q.empty())
            {
                u_t v = q.front();
                q.pop_front();
                component.push_back(v + 1); // 1-based
                for (u_t a: graph[v])
                {
                    if (!visited[a])
                    {
                        visited[a] = true;
                        q.push_back(a);
                    }
                }
            }
        }
    }
}

static void complete_connect(u_t N, vvu_t& S)
{
    vvu_t components;
    vu_t unused;
    get_components(components, unused, N, S);
    if ((components.size() > 1) || !unused.empty())
    {
        cerr << " ... connecting\n";
        vu_t connection;
        for (const vu_t& component: components)
        {
            const u_t i = rand() % component.size();
            const u_t station = component[i];
            connection.push_back(station);
        }
        S.push_back(connection);
    }
}
 
static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    if (string(argv[ai]) == string("-debug"))
    {
        dbg_flags = strtoul(argv[ai + 1], nullptr, 0);
        ai += 2;
    }
    const u_t n_tests = strtoul(argv[ai++], nullptr, 0);
    const u_t Nmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Nmax = strtoul(argv[ai++], nullptr, 0);
    const u_t Lmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Lmax = strtoul(argv[ai++], nullptr, 0);
    const u_t Kmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Kmax = strtoul(argv[ai++], nullptr, 0);
    cerr << "n_tests=" << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        ", Lmin=" << Lmin << ", Lmax=" << Lmax <<
        ", Kmin=" << Kmin << ", Kmax=" << Kmax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        const u_t N = rand_range(Nmin, Nmax);
        const u_t L = rand_range(Lmin, Lmax);
        vvu_t S; S.reserve(L);
        vu_t all_stations; all_stations.reserve(N);
        for (u_t station = 1; station <= N; ++station)
        {
            all_stations.push_back(station);
        }
        while (S.size() < L)
        {
            const u_t K = min(N, rand_range(Kmin, Kmax));
            vu_t line; line.reserve(K);
            vu_t available(all_stations);
            while (line.size() < K)
            {
                const u_t si = rand() % available.size();
                const u_t station = available[si];
                line.push_back(station);
                available[si] = available.back();
                available.pop_back();
            }
            S.push_back(line);
        }
        complete_connect(N, S);
        
        rc = test_case(N, S);
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test_random(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
