// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#undef TRACE

#include <fstream>
#include <iostream>
#include <array>
#include <unordered_map>
#include <string>
#include <map>
#include <set>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;

static unsigned dbg_flags;

class A2Hash
{
 public:
    A2Hash(u_t sz) : m(2*sz) {}
    size_t operator()(const au2_t& key) const
    {
        size_t i = m*key[0] + key[1];
        return i;
    }
 private:
    u_t m;
};

typedef unordered_map<au2_t, u_t, A2Hash> au2_to_u_t;

class StaticGraph
{
  public:
    StaticGraph(u_t s) : side_size(s), rp2i(s*s, A2Hash(s))
    {
        set_rp2i();
        set_adj();
    }
    u_t index(const au2_t& cell) const
    {
        au2_to_u_t::const_iterator iter = rp2i.find(cell);
        u_t i = (iter != rp2i.end() ? iter->second : u_t(-1));
        return i;
    }
    const vu_t& get_adjs(u_t icell) const { return adj[icell]; }
 private:
    void set_rp2i()
    {
        for (u_t r = 1, i = 0; r <= side_size; ++r)
        {
            const u_t np = 2*r - 1;
            for (u_t p = 1; p <= np; ++p, ++i)
            {
                const au2_t cell{r, p};
                au2_to_u_t::value_type v{cell, i};
                rp2i.insert(v);
            }
        }
    }
    void set_adj()
    {
        for (u_t r = 1, i = 0; r <= side_size; ++r)
        {
            const u_t np = 2*r - 1;
            for (u_t p = 1; p <= np; ++p, ++i)
            {
                const au2_t cell{r, p};
                vau2_t nbrs;
                get_neighbors(nbrs, cell);
                vu_t inbrs;
                for (const au2_t& nbr: nbrs)
                {
                    u_t inbr = index(nbr);
                    inbrs.push_back(inbr);
                }
                adj.push_back(inbrs); // @ [i]
            }
        }

    }
    void get_neighbors(vau2_t& nbrs, const au2_t& src) const;
    u_t side_size;
    au2_to_u_t rp2i;
    vvu_t adj;
};
typedef vector<StaticGraph> vsg_t;

void StaticGraph::get_neighbors(vau2_t& nbrs, const au2_t& src) const
{
    nbrs.clear();
    if (src[1] % 2 == 0)
    {
        nbrs.push_back(au2_t{src[0] - 1, src[1] - 1});
    }

    u_t max_col = 2*src[0] - 1;
    if (src[1] > 1)
    {
        nbrs.push_back(au2_t{src[0], src[1] - 1});
    }
    if (src[1] < max_col)
    {
        nbrs.push_back(au2_t{src[0], src[1] + 1});
    }

    if ((src[0] < side_size) && (src[1] % 2 == 1))
    {
        nbrs.push_back(au2_t{src[0] + 1, src[1] + 1});
    }
}

class State
{
 public:
   State() : turn_alma(true), dead_mask(0), alma_berthe{0, 0}, n{0, 0} {}
   bool avail(const u_t icell, const ull_t set_under) const
   {
       const ull_t bit = 1ull << icell;
       bool ret = true;
       ret = ret && ((set_under & bit) == 0);
       ret = ret && ((alma_berthe[0] & bit) == 0);
       ret = ret && ((alma_berthe[1] & bit) == 0);
       return ret;
   }
   void move_to(u_t player_idx, u_t icell)
   {
       pos[player_idx] = icell;
       alma_berthe[player_idx] |= (1ull << icell);
       ++n[player_idx];
#if defined(TRACE)
       trace[player_idx].push_back(icell);
#endif
   }
   int value() const
   {
       int a = n[0];
       int b = n[1];
       int ret = a - b;
       return ret;
   }
   bool turn_alma;
   u_t dead_mask;
   u_t pos[2];
   ull_t alma_berthe[2];
 private:
   int n[2];
#if defined(TRACE)
   vu_t trace[2];
#endif
};

bool operator<(const State& s0, const State& s1)
{
    bool lt = false;
    if (s0.turn_alma < s1.turn_alma)
    {
        lt = true;
    }
    else if (s0.turn_alma == s1.turn_alma)
    {
        if (s0.alma_berthe < s1.alma_berthe)
        {
            lt = true;
        }
        else if (s0.alma_berthe == s1.alma_berthe)
        {
            lt = (s0.pos < s1.pos);
        }
    }
    return lt;
}

typedef set<State> states_t;
typedef map<State, int> state2i_t;

class Duel
{
 public:
    Duel(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void solve_naive2();
    int grow(const State& state, u_t depth);
    static void init_static_graphs();
    static vsg_t static_graphs;
    u_t S, RA, PA, RB, PB, C;
    int solution;
    vau2_t under;
    ull_t set_under;
    state2i_t state2score;
};

vsg_t Duel::static_graphs;

Duel::Duel(istream& fi) : solution(0), set_under(0)
{
    fi >> S >> RA >> PA >> RB >> PB >> C;
    for (u_t i = 0; i != C; ++i)
    {
        au2_t uc;
        fi >> uc[0] >> uc[1];
        under.push_back(uc);
    }
    solution = -(2*S*S);
}

void Duel::solve_naive()
{
    if (S == 2)
    {
        solve_naive2();
    }
}

void Duel::solve_naive2()
{
    if ((RA == 2) && (PA == 2))
    {  // Alma @ center
        solution = (C == 2 ? 0 : 1);
    }
    else if ((RB == 2) && (PB == 2))
    {  // Berthe @ center
        solution = (C == 2 ? 0 : -1);
    }
    else
    {
        if (C == 0)
        {
            solution = 2;
        }
        else if (C == 1)
        {
            au2_t uc = under[0];
            if ((uc[0] == 2) && (uc[1] == 2))
            {
                solution = 0;
            }
            else
            {
                solution = 1;
            }
        }
        else // C == 2
        {
            solution = 0;
        }
    }
}

void Duel::solve()
{
    for (u_t gsz = static_graphs.size(); gsz <= S; ++gsz)
    {
        static_graphs.push_back(StaticGraph(gsz));
    }
    int ns = S;
    solution = -2*(ns+1)*(ns+1); // -infinity
    for (const au2_t& cell: under)
    {
        u_t icell = static_graphs[S].index(cell);
        set_under |= (1ull << icell);
    }
    State s0;
    s0.turn_alma = true;
    au2_t ab_pos[2] = {au2_t{RA, PA}, au2_t{RB, PB}};
    for (u_t pi: {0, 1})
    {
        u_t icell = static_graphs[S].index(ab_pos[pi]);
        s0.move_to(pi, icell);
    }
    solution = grow(s0, 0);
}

int Duel::grow(const State& state, u_t depth)
{
    static ull_t next_sz_show = 0;
    ull_t gsz = state2score.size();
    if ((gsz > next_sz_show) && ((gsz & (gsz - 1)) == 0))
    {
        if (dbg_flags & 0x1)
        {
            next_sz_show = gsz;
            cerr << "g.size=" << state2score.size() << '\n';
        }
    }
    u_t player_idx = state.turn_alma ? 0 : 1;
    int score = S*S;  if (player_idx == 0) { score = -score; }
    vu_t nbrs_avail;
    const vu_t nbrs = static_graphs[S].get_adjs(state.pos[player_idx]);
    for (u_t nbr: nbrs)
    {
        if (state.avail(nbr, set_under))
        {
            nbrs_avail.push_back(nbr);
        }
    }
    if (dbg_flags & 0x2) { cerr << "grow: d="<<depth << ", pi="<<player_idx <<
      ", #nbrs="<<nbrs.size() << ", nbrsAv="<<nbrs_avail.size() << '\n'; }
    if (nbrs_avail.empty())
    {
        State next_state(state);
        next_state.turn_alma = !state.turn_alma;
        next_state.dead_mask |= (1u << player_idx);
        auto er = state2score.equal_range(next_state);
        state2i_t::iterator iter = er.first;
        if (er.first == er.second)
        {
            if (next_state.dead_mask == 0x3)
            {
                score = state.value();
            }
            else
            {
                score = grow(next_state, depth + 1);
            }
            state2i_t::value_type v{next_state, score};
            state2score.insert(iter, v);
        }
        else
        {
            score = iter->second;
        }
    }
    else
    {
        for (u_t nbr: nbrs_avail)
        {
            int sub_score;
            State next_state(state);
            next_state.move_to(player_idx, nbr);
            u_t other = 1 - player_idx;
            if ((next_state.dead_mask & (1u << other)) == 0)
            {
                next_state.turn_alma = !state.turn_alma;
            }
            auto er = state2score.equal_range(next_state);
            state2i_t::iterator iter = er.first;
            if (er.first == er.second)
            {
                sub_score = grow(next_state, depth + 1);
                state2i_t::value_type v{next_state, score};
                state2score.insert(iter, v);
            }
            else
            {
                sub_score = iter->second;
            }
            if (player_idx == 0)
            {
                if (score < sub_score)
                {
                    score = sub_score;
                }
            }
            else
            {
                if (score > sub_score)
                {
                    score = sub_score;
                }
            }
        }
    }
    return score;
}

void Duel::print_solution(ostream &fo) const
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

    void (Duel::*psolve)() =
        (naive ? &Duel::solve_naive : &Duel::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Duel duel(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (duel.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        duel.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
