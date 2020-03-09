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
// typedef map<llu_t, ll_t> llu2ll_t;
// typedef map<vb_t, ll_t> vb2ll_t;
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

class GState
{
 public:
    GState(ll_t e, u_t ta, u_t nc, u_t nt) : 
        energy(e), 
        ncomps(nc),
        traps_allowed(ta), 
        nodes_used(size_t(nc + nt), false)
        {}
    void trap_add(u_t ti)
    {
        if (trap_may_add(ti))
        {
            potentials.push_back(ti);
        }
    }
    bool trap_may_add(u_t ti) const
    {
        bool may = ((!nodes_used[ncomps + ti]) &&
            ((traps_allowed & (1u << ti)) != 0));
        return may;
    }
    ll_t energy;
    u_t ncomps;
    u_t traps_allowed; // mask
    vb_t nodes_used;
    vu_t potentials;
};

class Node
{
 public:
    typedef pair<u_t, setu_t> mask_comps_t;
    Node(ll_t e=0, int ci=(-1), int ti=(-1)) :
        energy(e), component(ci), trap(ti) {}
    ll_t energy;   // or negative trap strength
    int component; // -1 if trap
    int trap; // -1 if component
    vu_t adjs;
    setu_t adjs_temp;
};
typedef vector<Node> vnode_t;

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
    bool collect_traps(const GState& state);
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
    const Node& snode = nodes[ni0];
    solution = (ni0 == ni_source_target[1] ? E + snode.energy : -1);
    const u_t mask_max = (1u << traps.size());
    u_t mask_must = mask_max - 1;
    const vvu_t& nb_masks = range_nbits_numbers[ntraps];
    for (u_t nb = ntraps; (nb > 0) && (mask_must != 0); --nb)
    {
        const vu_t& masks = nb_masks[nb];
        for (int mi = masks.size() - 1; (mi >= 0) && (mask_must != 0); --mi)
        {
            const u_t trap_mask = masks[mi];
            if ((trap_mask & mask_must) != 0)
            {
                GState state(E + snode.energy, trap_mask, ncomps, ntraps);
                state.nodes_used[ni0] = true;
                for (u_t ni: snode.adjs) // must be traps
                {
                    state.trap_add(ni - ncomps);
                }
                if (collect_traps(state))
                {
                    mask_must &= ~trap_mask;
                }
            }
        }
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
                    }
                }
                else if (cell.v != OBSTACLE)
                {
                    u_t aci = lower_bound(tb, te, arc) - tb;
                    aci += ncomps;
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

bool Cave::collect_traps(const GState& state)
{
    bool done = state.traps_allowed == 0;
    const u_t ncomps = component_value.size();
    for (u_t pi = 0, pe = state.potentials.size(); (pi != pe) && !done; ++pi)
    {
        u_t ti = state.potentials[pi];
        bool may = state.trap_may_add(ti);
        const Node* pnode = 0;
        if (may)
        {
            const Node& trap_node = nodes[ncomps + ti];
            if (state.energy + trap_node.energy >= 0)
            {
                pnode = &trap_node;
            }
        }
        if (pnode)
        {
            GState sub_state{state};
            sub_state.potentials[pi] = sub_state.potentials.back();
            sub_state.potentials.pop_back();
            sub_state.energy += pnode->energy;
            sub_state.nodes_used[ncomps + ti] = true;
            sub_state.traps_allowed &= ~(1u << ti);
            for (u_t ni: pnode->adjs)
            {
                if (!state.nodes_used[ni])
                {
                    const Node& node = nodes[ni];
                    if (node.component >= 0)
                    {
                        sub_state.energy += node.energy;
                        sub_state.nodes_used[ni] = true;
                        for (u_t a: node.adjs) // must be traps
                        {
                            sub_state.trap_add(a - ncomps);
                        }
                    }
                    else 
                    {  
                        sub_state.trap_add(node.trap);
                    }
                }
            }
            done = (sub_state.traps_allowed == 0);
            if (done)
            {
                if (sub_state.nodes_used[ni_source_target[1]] &&
                    (solution < sub_state.energy))
                {
                    solution = sub_state.energy;
                }
            }
            else
            {
                done = collect_traps(sub_state);
            }
        }
    }
    return done;
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

    if (!naive)
    {
        compute_range_nbits_numbers();
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
