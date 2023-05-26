// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <array>
#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>
// #include <iterator>
// #include <map>
// #include <set>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;
typedef unordered_set<u_t> uosetu_t;

static unsigned dbg_flags;

class Bus
{
 public:
   Bus(u_t u=0, u_t v=0, u_t _k=0) : uv{u, v}, k(_k) {}
   au2_t uv;
   u_t k;
};
typedef vector<Bus> vbus_t;
typedef vector<vbus_t> vvbus_t;

class Component
{
 public:
    vu_t vertices;
    uosetu_t clubs;
};
typedef vector<Component> vcomp_t;

class Graph
{
 public:
    Graph() {}
    Graph(u_t v, const vbus_t& busses); // bus-edges 1-based, internally 0-based
    vvbus_t vadjs; // adjs[i] are adjacents of vertex i
    vcomp_t components;
    vu_t vcomp; // vertex vertex[i] belongs to component vcomp[i]
    u_t vsize() const { return vadjs.size(); }
 private:
    void build_components();
};

Graph::Graph(u_t nv, const vbus_t& buses)
{
    vadjs.assign(nv, vbus_t());
    vcomp.assign(nv, u_t(-1));
    for (const Bus& bus: buses)
    {
        Bus bus0(bus);
        const u_t u = --bus0.uv[0];
        const u_t v = --bus0.uv[1];
        vadjs[u].push_back(bus0);
        vadjs[v].push_back(bus0);
    }
    build_components();
}
typedef vector<Graph> vgraph_t;

void Graph::build_components()
{
    const u_t nv = vsize();
    vu_t vcolor(nv, 0);
    for (u_t v = 0; v < nv; ++v)
    {
        if (vcolor[v] != 2)
        {
            const u_t icomp = components.size();
            components.push_back(Component());
            Component& component = components.back();
            // BFS
            deque<u_t> q;
            q.push_back(v);
            vcolor[v] = 1;
            while (!q.empty())
            {
                u_t curr = q.front();
                q.pop_front();
                component.vertices.push_back(curr);
                vcomp[curr] = icomp;
                vcolor[curr] = 1;
                for (const Bus& bus: vadjs[curr])
                {
                    component.clubs.insert(bus.k);
                    // the other vertex in bus.uv
                    const u_t a = bus.uv[bus.uv[0] == curr ? 1 : 0];
                    if (vcolor[a] == 0)
                    {
                        vcolor[a] = 1;
                        q.push_back(a);
                    }
                }
                vcolor[curr] = 2;
            }
            sort(component.vertices.begin(), component.vertices.end()); // ??
            if ((dbg_flags & 0x2) && (component.vertices.size() > 1)) {
               cerr << "  comp:";
               for (u_t u: component.vertices) { cerr << ' ' << u; }
               cerr << '\n'; }
        }
    }
}

class Contests
{
 public:
    Contests(istream& fi);
    Contests(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    void set_clubs();
    void build_graphs();
    u_t N, M, Q;
    vbus_t buses;
    vau2_t contests;
    u_t solution;

    vu_t clubs;    
    Graph graph;
    vgraph_t club_graphs;
};

Contests::Contests(istream& fi) : solution(0)
{
    fi >> N >> M >> Q;
    buses.reserve(M);
    while (buses.size() < M)
    {
        u_t u, v, k;
        fi >> u >> v >> k;
        buses.push_back(Bus(u, v, k));
    }
    contests.reserve(Q);
    while (contests.size() < Q)
    {
        u_t p, c;
        fi >> p >> c;
        contests.push_back(au2_t{c, p});
    }
}

void Contests::solve_naive()
{
    set_clubs();
    build_graphs();
    for (const au2_t& pc: contests)
    {
        const u_t p = pc[0] - 1;
        const u_t c = pc[1] - 1;
        u_t ip_comp = graph.vcomp[p];
        u_t ic_comp = graph.vcomp[c];
        if (ip_comp == ic_comp)
        {
            const Component& component = graph.components[ip_comp];
            u_t nclubs = component.clubs.size();
            if (nclubs % 2 == 1)
            {
                if (dbg_flags & 0x4) { cerr << "main: p="<<p << ", c=" <<c <<
                    ", nclubs=" << nclubs << '\n'; }
                ++solution;
            }
            else
            {
                bool any_odd = false;
                const u_t nc = club_graphs.size();
                for (u_t ci = 0; (ci < nc) && !any_odd; ++ ci)
                {
                    const Graph& cg = club_graphs[ci];
                    ip_comp = cg.vcomp[p];
                    ic_comp = cg.vcomp[c];
                    any_odd = (ip_comp == ic_comp);
                    if (any_odd)
                    {
                        if (dbg_flags & 0x4) { cerr << "p="<<p << ", c=" <<c <<
                            ", ci=" << ci << '\n'; }
                        ++solution;
                    }
                }
            }
        }
    }
}

void Contests::set_clubs()
{
    uosetu_t clubs_set;
    for (const Bus& bus: buses)
    {
        clubs_set.insert(bus.k);
    }
    clubs.reserve(clubs_set.size());
    for (u_t club: clubs_set)
    {
        clubs.push_back(club);
    }
    sort(clubs.begin(), clubs.end()); // ??
}

void Contests::build_graphs()
{
    if (dbg_flags & 0x1) { cerr << "busild main graph\n"; }
    graph = Graph(N, buses);
    const u_t nc = clubs.size();
    club_graphs.reserve(nc);
    for (u_t ci = 0; ci < nc; ++ci)
    {
        const u_t club = clubs[ci];
        vbus_t club_buses;
        for (const Bus& bus: buses)
        {
            if (bus.k == club)
            {
                club_buses.push_back(bus);
            }
        }
        if (dbg_flags & 0x1) {
            cerr << "busild club[" << ci << "]=" << club <<
            " graph #(E)=" << club_buses.size() << '\n'; }
        club_graphs.push_back(Graph(N, club_buses));
    }
}

void Contests::solve()
{
    solve_naive();
}

void Contests::print_solution(ostream &fo) const
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

    void (Contests::*psolve)() =
        (naive ? &Contests::solve_naive : &Contests::solve);
    if (solve_ver == 1) { psolve = &Contests::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Contests contests(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (contests.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        contests.print_solution(fout);
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
    if (dbg_flags & 0x100) { save_case("contests-curr.in"); }
    if (small)
    {
        Contests p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Contests p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("contests-fail.in");
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
