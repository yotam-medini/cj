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
    ToUsed(u_t _to=0, bool _used=false) : to(_to), used(_used) {}
    virtual tuple<u_t, bool> as_tuple() const { return make_tuple(to, used); }
    u_t to;
    bool used;
};
bool operator<(const ToUsed& uu0, const ToUsed& uu1)
{
    return uu0.as_tuple() < uu1.as_tuple();
}
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
};
Graph g_dummy1;

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
        // must be found in sorted to_adjs
        vtoused_t::iterator iter =
            lower_bound(to_adjs.begin(), to_adjs.end(), vertex,
            [](const ToUsed& tu, u_t v) -> bool
            {
                bool lt = tu.to < v;
                return lt;
            });
        ToUsed& toa = *iter;
        toa.used = true;
    }
    return a.to;
}

class ToLine : public ToUsed
{
 public:
    ToLine(u_t _to=0, u_t _line=0, bool _used=false) :
        ToUsed(_to, _used), line(_line) {}
    tuple<u_t, u_t, bool> as_tuple3() const
    {
       return make_tuple(to, line, used);
    }
    u_t line;
};
bool operator<(const ToLine& tl0, const ToLine& tl1)
{
    return tl0.as_tuple3() < tl1.as_tuple3();
}
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
        // must be found in sorted to_adjs
        vtoline_t::iterator iter =
            lower_bound(to_adjs.begin(), to_adjs.end(), vertex,
            [](const ToLine& tl, u_t v) -> bool
            {
                bool lt = tl.to < v;
                return lt;
            });
        ToLine& toa = *iter;
        toa.used = true;
    }
    return a.to;
}

class Railroad;
using p_add_bridge_t = void (Railroad::*)(u_t p, u_t v); 

class Railroad
{
 public:
    Railroad(istream& fi);
    Railroad(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    // typedef void (Railroad::*p_add_bridge_t)(u_t p, u_t v); 
    void build_graph_without_line(vvu_t& g, u_t l);
    void build_graph_line_edges(vvtoline_t& g) const;
    bool is_graph_connected(const vvu_t& g);
    // void build_graph();
    void add_essential_via_unique_stations();
    void add_essential_via_edges();
    void build_lines_graph();
    void dfs_get_bridges(const vvu_t& graph, p_add_bridge_t add_edge);
    void dfs_get_bridges(GraphBase& graph, p_add_bridge_t add_edge);
    void essential_add(u_t p, u_t v);
    u_t N, L;
    vu_t K;
    vvu_t S;
    u_t solution;
    hsetu_t essential_lines;
    GraphMultiEdge graph_vertices_lines;
#if 0
    vvtoline_t graph_line_edges;
    vector<bool> visited;
#endif
};

Railroad::Railroad(istream& fi) : solution(0)
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

void Railroad::build_lines_graph()
{
    vvu_t lgraph;
    vhsetu_t station_lines(N + 1, hsetu_t());
    for (u_t l = 0; l < L; ++l)
    {
        const vu_t& line = S[l];
        for (u_t station: line)
        {
            station_lines[station].insert(l);
        }
    }
    lgraph.reserve(L);
    for (u_t l = 0; l < L; ++l)
    {
        setu_t nbrs;
        const vu_t& line = S[l];
        for (u_t station: line)
        {
            const hsetu_t& slines = station_lines[station];
            for (u_t nl: slines)
            {
                nbrs.insert(nl);
            }
        }
        lgraph.push_back(vu_t(nbrs.begin(), nbrs.end()));
    }
    if (dbg_flags & 0x1) {
        cerr << "Lines graph: {\n";
        for (u_t l = 0; l < L; ++l)
        {
             cerr << "  " << l << ":";
             for (u_t nl: lgraph[l]) { cerr << ' ' << nl; }
             cerr << '\n';
        }
        cerr << "}\n";
    }
    solution = essential_lines.size();
}

void Railroad::solve()
{
    add_essential_via_unique_stations();
    add_essential_via_edges();
#if 0
    build_graph_line_edges(graph_line_edges);
  vvu_t lg;
  for (const vtoline_t& le: graph_line_edges) {
      setu_t adjs_set;
      for (const ToLine& tl: le) {
          adjs_set.insert(tl.to);
      }
      vu_t adjs(adjs_set.begin(), adjs_set.end());
      lg.push_back(adjs);
  }
    dfs_get_bridges(lg, &Railroad::essential_add);
#endif
    
    solution = essential_lines.size();
}

void Railroad::add_essential_via_unique_stations()
{
    vvu_t station_lines(N, vu_t());
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
            graph.v_adjs[s0].push_back(ToLine(s1, l));
            graph.v_adjs[s1].push_back(ToLine(s0, l));
        }
    }
    for (vtoline_t& adjs: graph.v_adjs)
    {
        sort(adjs.begin(), adjs.end());
    }
    if (dbg_flags & 0x1) {
        cerr << "Lines graph: {\n";
        for (u_t v = 0; v < N; ++v) {
             cerr << "  " << v << ":";
             for (const ToLine& tl: graph.v_adjs[v]) {
                 cerr << " (" << tl.to << ", @" << tl.line << ")";
             }
             cerr << '\n';
        }
        cerr << "}\n";
    }
    dfs_get_bridges(graph, &Railroad::essential_add);
}

#if 0
void Railroad::solve()
{
    build_graph_line_edges(graph_line_edges);
    visited.assign(N, false);
    vu_t parent(N, u_t(-1));
    u_t timer = 0;
    vu_t dtime(N, u_t(-1));
    vu_t etime(N, u_t(-1));
    vu_t low(N, u_t(-1));
    vu_t lineto(N, u_t(-1));

    // Non-recursive DFS
    vau2_t station_ai_stack;
    station_ai_stack.push_back(au2_t{0, 0});
    visited[0] = true;
    while (!station_ai_stack.empty())
    {
        const u_t station = station_ai_stack.back()[0];
        u_t ai = station_ai_stack.back()[1];
        if (ai == 0)
        {
            dtime[station] = low[station] = timer++;
        }
        const vtoline_t& adjs = graph_line_edges[station];
        const u_t na = adjs.size();
        for ( ; (ai < na) && visited[adjs[ai].to]; ++ai)
        {
            const u_t a = adjs[ai].to;
            // if (!((a == parent[station]) && (adjs[ai].line == lineto[a])))
            if (a != parent[station])
            {
                low[station] = min(low[station], dtime[a]);
            }
        }
        if (ai < na)
        {
            station_ai_stack.back()[1] = ai + 1;
            const u_t a = adjs[ai].to;
            parent[a] = station;
            lineto[a] = adjs[ai].line;
            station_ai_stack.push_back(au2_t{a, 0});
            visited[a] = true;
        }
        else 
        {
            const u_t p = parent[station];
            if ((p != u_t(-1)) && (dtime[p] < low[station]))
            {
                essential_add(p, station);
            }
            station_ai_stack.pop_back();
            etime[station] = timer++;
        }
    }
    solution = essential_lines.size();
}
#endif

void Railroad::build_graph_line_edges(vvtoline_t& g) const
{
    g.assign(N, vtoline_t());
    for (u_t l = 0; l < L; ++l)
    {
        const vu_t& line = S[l];
        for (u_t i = 0, j = 1; j  < line.size(); i = j++)
        {
            const u_t s0 = line[i] - 1, s1 = line[j] - 1;
            g[s0].push_back(ToLine(s1, l));
            g[s1].push_back(ToLine(s0, l));
        }
    }
    for (vtoline_t& nbrs: g)
    {
        sort(nbrs.begin(), nbrs.end());
    }
}

void Railroad::dfs_get_bridges(const vvu_t& graph, p_add_bridge_t add_edge)
{
    const u_t V = graph.size();
    vector<bool> dfs_visited(V, false);
    vu_t parent(V, u_t(-1));
    u_t timer = 0;
    vu_t dtime(V, u_t(-1));
    vu_t etime(V, u_t(-1));
    vu_t low(V, u_t(-1));

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
            dtime[v] = low[v] = timer++;
        }
        const vu_t& adjs = graph[v];
        const u_t na = adjs.size();
        for ( ; (ai < na) && dfs_visited[adjs[ai]]; ++ai)
        {
            const u_t a = adjs[ai];
            if (a != parent[v])
            {
                low[v] = min(low[v], dtime[a]);
            }
        }
        if (ai < na)
        {
            v_ai_stack.back()[1] = ai + 1;
            const u_t a = adjs[ai];
            parent[a] = v;
            v_ai_stack.push_back(au2_t{a, 0});
            dfs_visited[a] = true;
        }
        else 
        {
            const u_t p = parent[v];
            if ((p != u_t(-1)) && (dtime[p] < low[v]))
            {
                (this->*add_edge)(p, v);
            }
            v_ai_stack.pop_back();
            etime[v] = timer++;
        }
    }
} 

void Railroad::dfs_get_bridges(GraphBase& graph, p_add_bridge_t add_edge)
{
    const u_t V = graph.size();
    vector<bool> dfs_visited(V, false);
    vu_t parent(V, u_t(-1));
    u_t timer = 0;
    vu_t dtime(V, u_t(-1));
    vu_t etime(V, u_t(-1));
    vu_t low(V, u_t(-1));

    // Non-recursive DFS
    vau2_t v_ai_stack;
    // u_t ai0 = graph.vertex_adj_index_begin(0);
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
        for (u_t ami = ai; (ami < ai_end); graph.vertex_adj_index_next(v, ami))
        {
            const u_t a = graph.vertex_adj_index_to(v, ami, false);
            low[v] = min(low[v], dtime[a]);
        }

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
                // ai0 = graph.vertex_adj_index_begin(a);
                v_ai_stack.push_back(au2_t{a, 0});
                dfs_visited[a] = true;
            }
        }
        else 
        {
            const u_t p = parent[v];
            if (p != u_t(-1))
            {
                low[p] = min(low[p], low[v]);
                if (dtime[p] < low[v])
                {
                    (this->*add_edge)(p, v);
                }
            }
            v_ai_stack.pop_back();
            etime[v] = timer++;
        }
    }
}

class _CompToList
{
 public:
     bool operator()(const ToLine& tl0, const ToLine& tl1)
     {
         return tl0.to < tl1.to;
     }
     bool operator()(const ToLine& tl, const u_t& x)
     {
         return tl.to < x;
     }
     bool operator()(const u_t& x, const ToLine& tl)
     {
         return x < tl.to;
     }
};

void Railroad::essential_add(u_t p, u_t v)
{
    const vtoline_t& adjs = graph_vertices_lines.v_adjs[p];
    auto er = equal_range(adjs.begin(), adjs.end(), v, _CompToList());
    u_t n_found = er.second - er.first;
    if (n_found != 1) {
        cerr << "n_found="<<n_found << ", p="<<p << ", v="<<v << " not found\n";
    }
    u_t line = er.first->line;
    essential_lines.insert(line);
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

static void save_case(const char* fn)
{
    ofstream f(fn);
    f << "1\n";
    f.close();
}

static int test_case(int argc, char ** argv)
{
    int rc = rand_range(0, 1);
    ull_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (dbg_flags & 0x100) { save_case("railroad-curr.in"); }
    if (small)
    {
        Railroad p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Railroad p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("railroad-fail.in");
    }
    if (rc == 0) { cerr << "  ..." <<
        (small ? " (small) " : " (large) ") << " --> " <<
        solution << '\n'; }
    return rc;
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
    cerr << "n_tests=" << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        rc = test_case(argc, argv);
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
