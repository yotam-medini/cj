// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <array>
#include <utility>

#include <cstdlib>
#include <cstring>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;
typedef array<int, 2> ai2_t;
typedef array<u_t, 2> au2_t;
typedef vector<u_t> vu_t;
typedef vector<bool> vb_t;
typedef vector<ai2_t> vai2_t;
typedef set<u_t> setu_t;
typedef pair<ll_t, u_t> llu_t;
typedef pair<ai2_t, vb_t> visit_key_t;
typedef map<visit_key_t, ll_t> visit_map_t;

class BaseMatrix
{
  public:
    BaseMatrix(u_t _m, u_t _n) : m(_m), n(_n) {}
    const u_t m; // rows
    const u_t n; // columns
  protected:
    u_t rc2i(u_t r, u_t c) const
    {
        u_t ret = r*n + c;
        return ret;
    }
    void i2rc(u_t &r, u_t &c, u_t i) const
    {
        r = i / n;
        c = i % n;
    }
};

template <class T>
class Matrix : public BaseMatrix
{
  public:
    typedef Matrix<T> self_t;
    Matrix(u_t _m, u_t _n) : BaseMatrix(_m, _n), _a(m * n, T()) {}
    const T& get(u_t r, u_t c) const { return _a[rc2i(r, c)]; }
    T& get(u_t r, u_t c) { return _a[rc2i(r, c)]; }
    T& get(const au2_t& rc) { return get(rc[0], rc[1]); }
    T& get(const ai2_t& rc) { return get(rc[0], rc[1]); }
    T cget(const ai2_t& rc) { return _a[rc2i(rc[0], rc[1])]; }
    void put(u_t r, u_t c, const T &v) { _a[rc2i(r, c)] = v; }
    void put(const ai2_t&  rc, const T &v) { put(u_t(rc[0]), u_t(rc[1]), v); }
    const vector<T>& geta() const { return _a; }
  private:
    vector<T> _a;
};
template <class T>
bool operator<(const Matrix<T>& m0, const Matrix<T>& m1)
{
    bool lt = m0.geta() < m1.geta();
    return lt;
}

static unsigned dbg_flags;
static const ai2_t steps[4] = {{-1, 0}, {0, -1}, {0, 1}, {1, 0}};

class Cell
{
 public:
    Cell(int _v=0, int _comp=(-1), u_t _clr=0) :
        v(_v), component(_comp), color(_clr) {}
    int v;
    int component;
    u_t color;
};
typedef Matrix<Cell> mtxcell_t;
typedef Matrix<bool> mtxb_t;

class State
{
 public:
    State(ll_t e, u_t m, u_t n) : energy(e), visited(new mtxb_t(m, n)) {}
    ~State() { delete visited; }
    ll_t energy;
    mtxb_t* visited;
};

class RState
{
 public:
    RState(ll_t e = -1, u_t ta=0, size_t n=0) :
        energy(e), trap_adjs(ta), nodes_used(n, false) {}
    ll_t energy;
    u_t trap_adjs;
    vb_t nodes_used;
};
typedef map<u_t, RState> ut2rstate_t;

class Node
{
 public:
    typedef pair<u_t, setu_t> mask_comps_t;
    Node(ll_t e=0, int ci=(-1), int ti=(-1)) :
        energy(e), component(ci), trap(ti), trap_adjs(0) {}
    ll_t energy;   // or negative trap strength
    int component; // -1 if trap
    int trap; // -1 if component
    vu_t adjs;
    u_t trap_adjs; // bits
    setu_t adjs_temp;
};
typedef vector<Node> vnode_t;

#if 0
static unsigned nbits(unsigned x)
{
    unsigned n = 0;
    while (x)
    {
        n += (x & 1u);
        x /= 2;
    }
    return n;
}

typedef vector<vu_t> vvu_t;
typedef vector<vvu_t> vvvu_t;
static const u_t NB = 15;
static vvvu_t range_nbits_numbers{size_t(NB + 1), vvu_t()};

static void compute_range_nbits_numbers()
{
    u_t n = 0;
    for (u_t r = 0; r <= NB; ++r)
    {
        range_nbits_numbers[r] = vvu_t{size_t{r + 1}, vu_t()};
    }
    for (u_t r = 0; r <= NB; ++r)
    {
        for (u_t ne = (1u << r); n < ne; ++n)
        {
            u_t nb = nbits(n);
            for (u_t ri = r; ri <= NB; ++ri)
            {
                range_nbits_numbers[ri][nb].push_back(n);                
            }
        }
    }
}
#endif

class Cave
{
 public:
    Cave(istream& fi);
    virtual ~Cave() { delete pmtxcell; }
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    enum { OBSTACLE = -100000 };
    void get_traps();
    void set_components();
    ull_t dfs_component(const ai2_t& rc, u_t comp_id, ull_t energy);
    ll_t cell_solve(const ai2_t& rc, State& state, u_t depth);
    void build_graph();
    void rstate_anex(RState& rs, u_t ti);
    void show_graph() const;
    bool rc_inside(const ai2_t& rc) const
    {
        bool in = (0 <= rc[0]) && (rc[0] < N) && (0 <= rc[1]) && (rc[1] < M);
        return in;
    }
    int N, M;
    ll_t E;
    int Sr, Sc, Tr, Tc;
    ai2_t source, target;
    mtxcell_t *pmtxcell;
    visit_map_t visited_to_energy;
    vai2_t traps;
    vu_t component_value;
    vnode_t nodes;
    u_t ni_source_target[2];
    ll_t solution;
};

Cave::Cave(istream& fi) : pmtxcell(0), solution(0)
{
    fi >> N >> M >> E >> Sr >> Sc >> Tr >> Tc;
    source = ai2_t{Sr - 1, Sc - 1};
    target = ai2_t{Tr - 1, Tc - 1};
    pmtxcell = new mtxcell_t(N, M);
    for (int r = 0; r < N; ++r)
    {
        for (int c = 0; c < M; ++c)
        {
            int v;
            fi >> v;
            Cell& cell = pmtxcell->get(r, c);
            cell.v = v;
        }
    }
}

void Cave::solve_naive()
{
    State state{E, u_t(N), u_t(M)};
    solution = cell_solve(source, state, 0);
}

ll_t Cave::cell_solve(const ai2_t& rc, State& state, u_t depth)
{
    ll_t ret = -1;
    ll_t v = pmtxcell->get(rc).v;
    if (v != OBSTACLE)
    {
        const visit_key_t  key{rc, state.visited->geta()};
        auto er = visited_to_energy.equal_range(key);
        visit_map_t::iterator iter = er.first;
        if (er.first == er.second)
        {
            visit_map_t::value_type vmemo{key, -2}; // temporary value
            iter = visited_to_energy.insert(iter, vmemo);
            bool visited = state.visited->cget(rc);
            if (visited || (state.energy + v >= 0))
            {
                if (!visited)
                {
                    state.energy += v;
                    state.visited->put(rc, true);
                }
                if (state.visited->cget(target))
                {
                    ret = state.energy;
                }
                for (u_t si = 0; si != 4; ++si)
                {
                    const ai2_t& step = steps[si];
                    ai2_t arc{int(rc[0]) + step[0], int(rc[1]) + step[1]};
                    if (rc_inside(arc))
                    {
                        ll_t e = cell_solve(arc, state, depth + 1);
                        if (ret < e)
                        {
                            ret = e;
                        }
                    }
                }
                if (!visited)
                {
                    state.energy -= v;
                    state.visited->put(rc, false); // key restored
                }
            }
            iter->second = ret;
        }
        ret = iter->second;
    }
    return ret;
}

void Cave::solve()
{
    build_graph();
    const u_t ncomps = component_value.size();
    const u_t ntraps = traps.size();
    const u_t ni0 = ni_source_target[0];
    const u_t ni1 = ni_source_target[1];
    const Node& snode = nodes[ni0];
    RState rstate(E + snode.energy, snode.trap_adjs, nodes.size());
    solution = (ni0 == ni1 ? rstate.energy : -1);
    rstate.nodes_used[ni0] = true;
    ut2rstate_t mask2rs;
    mask2rs.insert(mask2rs.end(), ut2rstate_t::value_type(0, rstate));
    for (u_t nt = 1; (nt <= ntraps) && !mask2rs.empty(); ++nt)
    {
        ut2rstate_t mask2rs_next;
        for (const ut2rstate_t::value_type v: mask2rs)
        {
            const u_t mask0 = v.first;
            const RState& rs = v.second;
            for (u_t ti = 0; ti < ntraps; ++ti)
            {
                const u_t tbit = (1u << ti);
                if (((mask0 & tbit) == 0) && ((rs.trap_adjs & tbit) != 0))
                {
                    const Node& trap = nodes[ti + ncomps];
                    ll_t e = rs.energy + trap.energy;
                    if (e >= 0)
                    {
                        u_t key = mask0 | tbit;
                        auto er = mask2rs_next.equal_range(key);
                        if (er.first == er.second)
                        {
                            RState rs_next(rs);
                            rs_next.energy = e;
                            rstate_anex(rs_next, ti);
                            mask2rs_next.insert(er.first, 
                                ut2rstate_t::value_type(key, rs_next));
                        }
                    }
                }
            }  
        }
        swap(mask2rs, mask2rs_next);
    }
}

void Cave::rstate_anex(RState& rs, u_t ti)
{
    const u_t ncomps = component_value.size();
    const Node& trap = nodes[ti + ncomps];
    const vu_t& adjs = trap.adjs;
    for (u_t ai = 0, ae = adjs.size(); ai < ae; ++ai)
    {
        u_t ni = adjs[ai];
        if (!rs.nodes_used[ni])
        {
            const Node& adj = nodes[ni];
            if (ni < ncomps)
            {
                rs.energy += adj.energy;
                for (u_t aai: adj.adjs) // must all be traps
                {
                    u_t aati = aai - ncomps;
                    rs.trap_adjs |= (1u << aati);
                }
            }
            else
            {
                u_t ati = ni - ncomps;
                rs.trap_adjs |= (1u << ati);
            }
            rs.nodes_used[ni] = true;
        }
    }
    if (rs.nodes_used[ni_source_target[1]] && solution < rs.energy)
    {
        solution = rs.energy;
    }
}

void Cave::get_traps()
{
    for (int r = 0; r < int(N); ++r)
    {
        for (int c = 0; c < int(M); ++c)
        {
            int v = pmtxcell->get(r, c).v;
            if ((v != OBSTACLE) && (v < 0))
            {
                traps.push_back(ai2_t{r, c}); // trivially sorted
            }
        }
    }
}

void Cave::set_components()
{
    // init
    for (int r = 0; r < N; ++r)
    {
        for (int c = 0; c < M; ++c)
        {
            Cell& cell = pmtxcell->get(r, c);
            if (cell.v >= 0)
            {
                cell.color = 0; // white
            }
        }
    }
    for (int r = 0; r < N; ++r)
    {
        for (int c = 0; c < M; ++c)
        {
            const Cell& cell = pmtxcell->get(r, c);
            if ((cell.v >= 0) && (cell.color == 0))
            {
                const ai2_t rc{int(r), int(c)};
                ull_t energy = dfs_component(rc, component_value.size(), 0);
                component_value.push_back(energy);
            }
        }
    }
}

ull_t Cave::dfs_component(const ai2_t& rc, u_t comp_id, ull_t energy)
{
    Cell& cell = pmtxcell->get(rc);
    cell.color = 1; // gray
    energy += cell.v;
    cell.component = comp_id;
    for (const ai2_t& step: steps)
    {
        ai2_t arc{rc[0] + step[0], rc[1] + step[1]};
        if (rc_inside(arc))
        {
            const Cell& adj = pmtxcell->get(arc);
            if ((adj.color == 0) && (adj.v >= 0))
            {
                energy = dfs_component(arc, comp_id, energy);
            }
        }
    }
    cell.color = 2; // black
    return energy;
}

void Cave::build_graph()
{
    get_traps();
    set_components();
    const u_t ncomps = component_value.size();
    for (u_t ci = 0; ci != ncomps; ++ci)
    {
        nodes.push_back(Node(component_value[ci], ci));
    }
    const vai2_t::const_iterator tb = traps.begin(), te = traps.end();
    for (u_t ti = 0, tn = traps.size(), ni = ncomps; ti != tn; ++ti, ++ni)
    {
        const ai2_t& trap = traps[ti];
        nodes.push_back(Node(pmtxcell->get(trap).v, -1, ti));
        Node& node = nodes.back();
        for (const ai2_t& step: steps)
        {
            ai2_t arc{trap[0] + step[0], trap[1] + step[1]};
            if (rc_inside(arc))
            {
                const Cell& cell = pmtxcell->get(arc);
                if (cell.v >= 0)
                {
                    auto iter_new = node.adjs_temp.insert(cell.component);
                    if (iter_new.second)
                    {
                        nodes[cell.component].adjs_temp.insert(ni);
                        nodes[cell.component].trap_adjs |= (1u << ti);
                    }
                }
                else if (cell.v != OBSTACLE)
                {
                    u_t ati = lower_bound(tb, te, arc) - tb;
                    node.trap_adjs |= (1u << ati);
                    u_t aci = ati + ncomps;
                    node.adjs_temp.insert(aci); 
                }
            }            
        }   
    }
    for (Node& node: nodes)
    {
        setu_t& at = node.adjs_temp;
        node.adjs.insert(node.adjs.end(), at.begin(), at.end());
        at.clear();
    }
    for (u_t ni = 0; ni != 2; ++ni)
    {
        const ai2_t& rc = ((ni == 0) ? source : target);
        const Cell& cell = pmtxcell->get(rc);
        ni_source_target[ni] = ((cell.component >= 0)
            ? cell.component
            : ncomps + lower_bound(tb, te, rc) - tb);
    }
    if (dbg_flags & 0x1) { show_graph(); }
}

void Cave::show_graph() const
{
    const vai2_t::const_iterator tb = traps.begin(), te = traps.end();
    for (int r = 0; r < N; ++r)
    {
        for (int c = 0; c < M; ++c)
        {
            const Cell& cell = pmtxcell->get(r, c);
            char b[0x10];
            if (cell.v == OBSTACLE)
            {
                strcpy(b, "   ");
            }
            else if (cell.component >= 0)
            {
                sprintf(b, "C%-2d", cell.component);
            }
            else
            {
                u_t ti = lower_bound(tb, te, ai2_t{r, c}) - tb;
                sprintf(b, "T%-2d", ti);
            }
            cerr << " " << b;
        }
        cerr << '\n';
    }
}

void Cave::print_solution(ostream &fo) const
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

    void (Cave::*psolve)() =
        (naive ? &Cave::solve_naive : &Cave::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Cave cave(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (cave.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        cave.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
