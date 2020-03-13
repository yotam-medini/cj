// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <array>
#include <set>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<u_t, 2> au2_t;
typedef vector<u_t> vu_t;
typedef vector<bool> vb_t;
typedef vector<au2_t> vau2_t;
typedef vector<vau2_t> vvau2_t;

class BaseWHMatrix
{
  public:
    BaseWHMatrix(unsigned _w, unsigned _h) : w(_w), h(_h) {}
    const unsigned w; // width
    const unsigned h; // hight
    unsigned size() const { return w*h; }
  protected:
    unsigned xy2i(unsigned x, unsigned y) const 
    {
        unsigned ret = h*x + y;
        return ret;
    }
    void i2rc(unsigned &x, unsigned &y, unsigned i) const 
    {
        x = i / h;
        y = i % h;
    }
};

template <class T>
class WHMatrix : public BaseWHMatrix
{
  public:
    WHMatrix(unsigned _w=0, unsigned _h=0) : 
        BaseWHMatrix(_w, _h), _a(w*h > 0 ? new T[w*h] : 0) {}
    virtual ~WHMatrix() { delete [] _a; }
    const T& get(unsigned x, unsigned y) const { return _a[xy2i(x, y)]; }
    const T& get(const unsigned* xy) const { return get(xy[0], xy[1]); }
    void put(unsigned x, unsigned y, const T &v) { _a[xy2i(x, y)] = v; }
    void put(const unsigned* xy, const T &v) { put(xy[0], xy[1], v); }
    const T* raw() const { return _a; }
  private:
    T *_a;
};
typedef WHMatrix<u_t> dists_t;

class Edge
{
 public:
   Edge(u_t v0=0, u_t v1=1, u_t _l=0) : v{v0, v1}, l(_l) {}
   au2_t v;
   u_t l;
};
typedef vector<Edge> vedge_t;

static unsigned dbg_flags;

class SpecialVillages
{
 public:
    SpecialVillages(istream& fi);
    ~SpecialVillages()
    {
        delete pdist;
    }
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    static void debug_test();
 private:
    SpecialVillages() : solution(0) {}
    void build_graph(const vedge_t& cedges);
    void compute_dists()
    {
        pdist = new dists_t(V, V);
        for (u_t s = 0; s != V; ++s) { dijkstra(s); }
    }
    void dijkstra(u_t s);
    void get_short_edges(vedge_t& short_edges);
    void solutions_dump(const vu_t& solution_masks);
    u_t V, E;
    vedge_t edges;
    u_t solution;
    vvau2_t graph; // adjs with dist
    dists_t* pdist;
};

SpecialVillages::SpecialVillages(istream& fi) : solution(0), pdist(0)
{
    fi >> V >> E;
    edges.reserve(E);
    while (edges.size() < E)
    {
        u_t v0, v1, l;
        fi >> v0 >> v1 >> l;
        edges.push_back(Edge(v0 - 1, v1 - 1, l));
    }
}

void SpecialVillages::solve_naive()
{
    build_graph(edges);
    compute_dists();
    const dists_t& dist = *pdist;
    const u_t infty = u_t(-1);
    u_t dsum_min = infty;
    vu_t solution_masks;
    for (u_t fruit_mask = 1, mask_max = (1u << V) - 1; 
        fruit_mask < mask_max; ++fruit_mask)
    {
        u_t dsum = 0;
        for (u_t s = 0; (s != V) && (dsum != infty); ++s)
        {
            u_t ds_min = u_t(-1);
            bool s_fruit = (fruit_mask & (1u << s)) != 0;
            for (u_t t = 0; t != V; ++t)
            {
                bool t_fruit = (fruit_mask & (1u << t)) != 0;
                if (s_fruit != t_fruit)
                {
                    u_t dfv = dist.get(s, t);
                    if (ds_min > dfv)
                    {
                        ds_min = dfv;
                    }
                }
            }
            dsum = (ds_min == infty ? infty : dsum + ds_min);
        }
        if (dsum <= dsum_min)
        {
            if (dsum < dsum_min)
            {
                solution = 0;
                dsum_min = dsum;
                solution_masks.clear();
            }
            ++solution;
            solution_masks.push_back(fruit_mask);
        }
    }
    if (dbg_flags & 0x2) { solutions_dump(solution_masks); }
}

void SpecialVillages::solutions_dump(const vu_t& solution_masks)
{
    const dists_t& dist = *pdist;
    u_t ns = solution_masks.size();
    cerr << "V="<<V << ", #solutions="<<ns << "{\n";
    for (u_t mi = 0; mi < ns; ++mi)
    {
        u_t mask = solution_masks[mi];
        u_t lmax = 0;
        cerr << "  ["<< mi << " / " << ns << "]\n";
        for (u_t fv = 0; fv != 2; ++fv)
        {
            bool fruit_pass = (fv == 0);
            cerr << "  " << (fruit_pass ? "Fruits:" : "Vegies") << ":\n";
            for (u_t s = 0; s != V; ++s)
            {
                u_t ds_min = u_t(-1);
                bool s_fruit = (mask & (1u << s)) != 0;
                if (s_fruit == fruit_pass)
                {
                    u_t tmin = V;
                    for (u_t t = 0; t != V; ++t)
                    {
                        bool t_fruit = (mask & (1u << t)) != 0;
                        if (s_fruit != t_fruit)
                        {
                            u_t dfv = dist.get(s, t);
                            if (ds_min > dfv)
                            {
                                ds_min = dfv;
                                tmin = t;
                            }
                        }
                    }
                    if (lmax < ds_min) { lmax = ds_min; }
                    cerr << "  ("<<s << ", "<<tmin <<") = " << ds_min << "\n";
                }
            }
        }
        cerr << "  Small edges:\n ";
        for (u_t l = 1; l <= lmax; ++l) {
            for (const Edge& e: edges) {
                if (e.l == l) {
                     cerr<<" [l="<<l << "] ("<< e.v[0] << ", "<<e.v[1]<<"),";
                }
            }
        }
        cerr << "\n";
    }
    cerr << "}\n";
}

void SpecialVillages::solve()
{
    vedge_t short_edges;
    get_short_edges(short_edges);
    const bool zero_edge = (short_edges[0].l == 0);
    u_t n_zero_adjs = 0;
    build_graph(short_edges);
    if (zero_edge)
    {
        const au2_t& e0v = short_edges[0].v;
        n_zero_adjs = graph[e0v[0]].size() + graph[e0v[1]].size() - 2;
    }

    vu_t v2c{vu_t(size_t(V), 0)}; // white
    u_t vi = 0, ncomps = 0;
    while (vi < V)
    {
        for (; (vi < V) && v2c[vi] != 0; ++ vi) {}
        if (vi < V)
        {
            vu_t q;
            q.push_back(vi);
            u_t comp_size = 0;
            while (!q.empty())
            {
               u_t v = q.back();
               q.pop_back();
               const vau2_t& adjs = graph[v];
               ++comp_size;
               for (const au2_t ad: adjs)
               {
                   u_t a = ad[0];
                   if (v2c[a] == 0)
                   {
                       q.push_back(a);
                       v2c[a] = 1; // gray
                   }
               }
               v2c[v] = 2; // black
            }
            ++ncomps;
        }
    }
    solution = (1u << ncomps) * (1u << n_zero_adjs);    
}

void SpecialVillages::build_graph(const vedge_t& cedges)
{
    graph = vvau2_t{size_t(V), vau2_t()};
    for (const Edge& edge: cedges)
    {
        graph[edge.v[0]].push_back(au2_t{edge.v[1], edge.l});
        graph[edge.v[1]].push_back(au2_t{edge.v[0], edge.l});
    }
    for (vau2_t& adjs: graph)
    {
        sort(adjs.begin(), adjs.end());
    }
}

void SpecialVillages::dijkstra(u_t s)
{
    dists_t& dist = *pdist;
    for (u_t t = 0; t != V; ++t)
    {
        dist.put(s, t, u_t(-1));
    }
    dist.put(s, s, 0);
    set<au2_t> qdv;
    qdv.insert(au2_t{0, s});    
    while (!qdv.empty())
    {
        au2_t dv = *qdv.begin();
        u_t dcurr = dv[0];
        u_t vcurr = dv[1];
        qdv.erase(qdv.begin());
        for (const au2_t& ad: graph[vcurr])
        {
            u_t a = ad[0];
            u_t dalt = dcurr + ad[1];
            if (dalt < dist.get(s, a))
            {
                dist.put(s, a, dalt);
                qdv.insert(au2_t{dalt, a});
            }
        }
    }
}

void SpecialVillages::get_short_edges(vedge_t& short_edges)
{
    // vb_t covered{size_t(V), false};
    vedge_t sedges(edges);
    sort(sedges.begin(), sedges.end(), 
        [](const Edge& e0, const Edge& e1) -> bool
        {
            return e0.l < e1.l;
        });
    vb_t covered(size_t(V), false);
    u_t n_covered = 0;
    for (u_t ei = 0; n_covered < V; ++ei)
    {
        const Edge& e = sedges[ei];
        bool add = false;
        for (u_t vi = 0; vi != 2; ++vi)
        {
            if (!covered[e.v[vi]])
            {
                covered[e.v[vi]] = true;
                ++n_covered;
                add = true;
            }
        }
        if (add)
        {
            short_edges.push_back(e);
        }
    }
}

void SpecialVillages::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

void SpecialVillages::debug_test()
{
    for (u_t ti = 0; ti != 100; ++ti)
    {
        SpecialVillages sv;
        sv.V = 4 + (rand() % 5);
        sv.E = (sv.V * (sv.V - 1))/2;
        vu_t lengths;
        for (u_t l = 0; l < sv.E; ++l) { lengths.push_back(l); }
        for (u_t s = 0; s < sv.V; ++s)
        {
            for (u_t t = s + 1; t < sv.V; ++t)
            {
                u_t li = rand() % lengths.size();
                u_t l = lengths[li];
                lengths[li] = lengths.back();
                lengths.pop_back();
                sv.edges.push_back(Edge(s, t, l));
            }
        }
        if (sv.edges.size() != sv.E) {
           cerr << "Something wrong\n";
        }
        sv.solve_naive();
        cout << "ti="<<ti<< ", V="<<sv.V << ", solution="<<sv.solution << '\n';
    }
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-debug"))
        {
            dbg_flags = strtoul(argv[++ai], 0, 0);
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

    if (dbg_flags & 0x1)
    {
        SpecialVillages::debug_test();
        exit(7);
    }

    string ignore;
    unsigned n_cases;
    *pfi >> n_cases;
    getline(*pfi, ignore);

    void (SpecialVillages::*psolve)() =
        (naive ? &SpecialVillages::solve_naive : &SpecialVillages::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        SpecialVillages specvill(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (specvill.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        specvill.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
