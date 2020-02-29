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

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;
typedef array<int, 2> ai2_t;
typedef array<u_t, 2> au2_t;
typedef vector<u_t> vu_t;
typedef vector<au2_t> vau2_t;
typedef vector<ai2_t> vai2_t;
typedef set<u_t> setu_t;
typedef pair<ll_t, u_t> llu_t;
typedef map<llu_t, ll_t> llu2ll_t;

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
    Matrix(u_t _m, u_t _n) : BaseMatrix(_m, _n), _a(new T[m *n]) {}
    virtual ~Matrix() { delete [] _a; }
    const T& get(u_t r, u_t c) const { return _a[rc2i(r, c)]; }
    T& get(u_t r, u_t c) { return _a[rc2i(r, c)]; }
    T& get(const au2_t& rc) { return get(rc[0], rc[1]); }
    T& get(const ai2_t& rc) { return get(rc[0], rc[1]); }
    void put(u_t r, u_t c, const T &v) const { _a[rc2i(r, c)] = v; }
  private:
    T *_a;
};

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
    llu2ll_t etraps2energy; // traps-destroyed - mask to energy to target (-1)
};
typedef Matrix<Cell> mtxcell_t;

class State
{
 public:
    State(ll_t e=0) : energy(e), traps_mask(0) {}
    ll_t energy;
    u_t traps_mask;
    setu_t components;
};

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
    u_t N, M;
    ll_t E;
    int Sr, Sc, Tr, Tc;
    void cell_solve(const ai2_t& rc, const State& state, u_t depth);
    ai2_t source, target;
    mtxcell_t *pmtxcell;
    vai2_t traps;
    vu_t component_value;
    ll_t solution;
};

Cave::Cave(istream& fi) : pmtxcell(0), solution(0)
{
    fi >> N >> M >> E >> Sr >> Sc >> Tr >> Tc;
    source = ai2_t{Sr - 1, Sc - 1};
    target = ai2_t{Tr - 1, Tc - 1};
    pmtxcell = new mtxcell_t(N, M);
    for (ull_t r = 0; r < N; ++r)
    {
        for (ull_t c = 0; c < M; ++c)
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
    get_traps();
    set_components();
    solution = -1;
    cell_solve(source, State{E}, 0);
}

void Cave::solve()
{
    solve_naive();
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
    for (u_t r = 0; r < N; ++r)
    {
        for (u_t c = 0; c < M; ++c)
        {
            Cell& cell = pmtxcell->get(r, c);
            if (cell.v >= 0)
            {
                cell.color = 0; // white
            }
        }
    }
    for (u_t r = 0; r < N; ++r)
    {
        for (u_t c = 0; c < M; ++c)
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
    for (u_t si = 0; si != 4; ++si)
    {
        const ai2_t& step = steps[si];
        ai2_t arc{rc[0] + step[0], rc[1] + step[1]};
        if ((0 <= arc[0]) && (arc[0] < int(N)) &&
            (0 <= arc[1]) && (arc[1] < int(M)))
        {
            const Cell& adj = pmtxcell->get(arc[0], arc[1]);
            if ((adj.color == 0) && (adj.v >= 0))
            {
                energy = dfs_component(arc, comp_id, energy);
            }
        }
    }
    cell.color = 2; // black
    return energy;
}

void Cave::cell_solve(const ai2_t& rc, const State& state, u_t depth)
{
    if (dbg_flags & 0x1) { cerr << string(depth, ' ') << 
      "("<<rc[0]<<","<<rc[1]<<") e="<<state.energy << ", depth="<<depth<<'\n';}
    Cell& cell = pmtxcell->get(rc);
    if ((cell.v != OBSTACLE) && (state.energy + cell.v >= 0))
    {
        llu_t key{state.energy, state.traps_mask};
        auto er = cell.etraps2energy.equal_range(key);
        llu2ll_t::iterator iter = er.first;
        if (er.first == er.second) // not yet
        {
            llu2ll_t::value_type v{key, -1};
            iter = cell.etraps2energy.insert(iter, v);
            State sub_state(state);
            int comp = cell.component;
            setu_t& comps = sub_state.components;
            if ((comp >= 0) && (comps.find(comp) == comps.end()))
            {
                comps.insert(comps.end(), comp);
                sub_state.energy += component_value[comp]; // increase
            }
            int itrap = (cell.v >= 0 ? -1 :
                lower_bound(traps.begin(), traps.end(), rc) - traps.begin());
            if ((itrap >= 0) && ((sub_state.traps_mask & (1u << itrap)) == 0))
            {
                sub_state.traps_mask |= (1u << itrap);
                sub_state.energy += cell.v; // decrease
            }
            if (dbg_flags & 0x2) { cerr << string(depth, ' ') << "-> " << 
               "("<<rc[0]<<","<<rc[1]<<") e="<<sub_state.energy << 
               ", depth="<<depth<<'\n';}
            if ((rc == target) && (solution < sub_state.energy))
            {
                solution = state.energy;
            }
            for (u_t si = 0; si != 4; ++si)
            {
                const ai2_t& step = steps[si];
                ai2_t arc{int(rc[0]) + step[0], int(rc[1]) + step[1]};
                if ((0 <= arc[0]) && (arc[0] < int(N)) &&
                    (0 <= arc[1]) && (arc[1] < int(M)))
                {
                    cell_solve(arc, sub_state, depth + 1);
                }
            }
        }
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