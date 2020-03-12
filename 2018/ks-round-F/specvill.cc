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
// typedef vector<vu_t> vvu_t;
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
 private:
    void build_graph();
    void compute_dists()
    {
        pdist = new dists_t(V, V);
        for (u_t s = 0; s != V; ++s) { dijkstra(s); }
    }
    void dijkstra(u_t s);
    u_t V, E;
    vedge_t edges;
    u_t solution;
    vvau2_t graph; // adjes with dist
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
    build_graph();
    compute_dists();
    const dists_t& dist = *pdist;
    u_t dsum_min = u_t(-1);
    for (u_t fruit_mask = 1, mask_max = (1u << V) - 2; 
        fruit_mask < mask_max; ++fruit_mask)
    {
        u_t dsum = 0;
        for (u_t s = 0; s != V; ++s)
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
            dsum += ds_min;
        }
        if (dsum <= dsum_min)
        {
            if (dsum < dsum_min)
            {
                solution = 0;
                dsum_min = dsum;
            }
            ++solution;
        }
    }
}

void SpecialVillages::solve()
{
    solve_naive();
}

void SpecialVillages::build_graph()
{
    graph = vvau2_t{size_t(V), vau2_t()};
    for (const Edge& edge: edges)
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

void SpecialVillages::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
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
