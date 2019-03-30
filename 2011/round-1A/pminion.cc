// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2019/03

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <set>
#include <map>
#include <queue>
#include <vector>
#include <deque>
#include <utility>
#include <tuple>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef pair<u_t, u_t> uu_t;
typedef vector<u_t> vu_t;
typedef vector<uu_t> vuu_t;
typedef deque<u_t> dqu_t;
typedef set<u_t> setu_t;
typedef pair<u_t, setu_t> usetu_t;
typedef map<usetu_t, u_t> usetu2u_t;

static unsigned dbg_flags;

class Card
{
 public:
   Card(u_t vc=0, u_t vs=0, u_t vt=0) : c(vc), s(vs), t(vt) {}
   bool zero() const { return (c + s + t) == 0; }
   u_t c, s, t;
};
typedef vector<Card> vcard_t;

bool operator<(const Card &c0, const Card &c1)
{
    bool lt = (c0.t < c1.t);
    if (c0.t == c1.t)
    {
        lt = (c0.c < c1.c) ||
            ((c0.c ==  c1.c) &&  (c0.s < c1.s));
    }
    return lt;
}

class State
{
  public:
    State(u_t vd=0, u_t vturn=0, u_t tc=0, u_t c0=0, u_t c1=0,  u_t c2=0) :
        _tup(vd, vturn, tc, c0, c1, c2) {}
    const u_t& drawn() const { return std::get<0>(_tup); }
    u_t& drawn() { return std::get<0>(_tup); }
    const u_t& turns() const { return std::get<1>(_tup); }
    u_t& turns() { return std::get<1>(_tup); }
    const u_t& t_played() const { return std::get<2>(_tup); }
    u_t& t_played() { return std::get<2>(_tup); }
    const u_t& c0_played() const { return std::get<3>(_tup); }
    u_t& c0_played() { return std::get<3>(_tup); }
    const u_t& c1_played() const { return std::get<4>(_tup); }
    u_t& c1_played() { return std::get<4>(_tup); }
    const u_t& c2_played() const { return std::get<5>(_tup); }
    u_t& c2_played() { return std::get<5>(_tup); }
    const u_t csz_played(u_t csz) const 
    {
        u_t ret = (csz == 0
            ? c0_played()
            : (csz == 1
                ? c1_played()
                : c2_played()));
        return ret;
    }
    u_t n_played() const 
    {
        u_t ret = t_played() + c0_played() + c1_played() + c2_played();
        return ret;
    }
    u_t hand_size(u_t n) const // n hand initial size
    {
        u_t ret = (n + drawn()) - n_played();
        return ret;
    }
    static bool eq(const State& s0, const State& s1)
    {
        return s0._tup == s1._tup;
    }
    static bool lt(const State& s0, const State& s1)
    {
        return s0._tup < s1._tup;
    }
  private:
    tuple<u_t, u_t, u_t, u_t, u_t, u_t> _tup;
};
bool operator==(const State& s0, const State& s1)
{
    return State::eq(s0, s1);
}
bool operator<(const State& s0, const State& s1)
{
    return State::lt(s0, s1);
}

typedef map<State, u_t> state2ut_t;

class Status
{
 public:
    Status(u_t t=1, u_t s=0, const setu_t& h=setu_t(), u_t dt=0) :
        turns(t), score(s), hand(h), deck_top(dt) {}
    void deck2hand(u_t draw, u_t deck_offset)
    {
        for (u_t di = 0; di < draw; ++di)
        {
            u_t ci = deck_offset + deck_top + di;
            hand.insert(hand.end(), ci);
        }
        deck_top += draw;
    }
    void hand2deck(u_t draw, u_t deck_offset)
    {
        deck_top -= draw;
        for (u_t di = 0; di < draw; ++di)
        {
            u_t ci = deck_offset + deck_top + di;
            hand.erase(setu_t::value_type(ci));
        }
    }
    u_t turns;
    u_t score;
    setu_t hand;
    u_t deck_top;
};

class PMinion
{
 public:
    PMinion(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void traverse(Status &status);
    void init_graph();
    u_t best_score(const State &state_from);
    u_t best_score_with_turns(const State &state_from);
    static const State state_final;
    static const state2ut_t::value_type state_value_final;
    u_t n, m;    
    vcard_t cards;
    u_t solution;
    vu_t t_cards;
    vu_t cx_cards[3];
    State state_initial;
    state2ut_t state2score;
};
const State PMinion::state_final(~0, 0, 0, 0, 0);
const state2ut_t::value_type PMinion::state_value_final(state_final, 0);

PMinion::PMinion(istream& fi) : solution(0)
{
    u_t c, s, t;
    u_t nm[2];
    for (u_t nmi = 0; nmi < 2; ++nmi)
    {
        fi >> nm[nmi];
        for (u_t i = 0; i < nm[nmi]; ++i)
        {
            fi >> c >> s >> t;
            cards.push_back(Card(c, s, t));
        }
    }
    n = nm[0];
    m = nm[1];
}

void PMinion::solve_naive()
{
    Status status;
    for (u_t i = 0; i < n; ++i)
    {
        status.hand.insert(status.hand.end(), i);
    }
    traverse(status);
}

void PMinion::traverse(Status &status)
{
    if (solution < status.score)
    {
        solution = status.score;
    }
    if (status.turns > 0)
    {
        --status.turns;
        setu_t &hand = status.hand;
        for (setu_t::iterator i = hand.begin(), e = hand.end(); i != e; ++i)
        {
            u_t ci = *i;
            const Card &card = cards[ci];
            hand.erase(i);
            status.score += card.s;
            status.turns += card.t;
            u_t draw = min(card.c, m - status.deck_top);
            status.deck2hand(draw, n);
            traverse(status);
            status.hand2deck(draw, n);
            status.turns -= card.t;
            status.score -= card.s;
            i = hand.insert(hand.end(), setu_t::value_type(ci));
        }
        ++status.turns;
    }
}

void PMinion::solve()
{
    init_graph();
    state_initial = State(0, 1, 0, 0, 0, 0);
    solution = best_score(state_initial);    
}

u_t PMinion::best_score(const State &state)
{
    u_t ret = ((state.turns() == 0) || (state.hand_size(n) == 0)
        ? 0 : best_score_with_turns(state));
    return ret;
}

static void maximize_by(u_t &m, u_t x)
{
    if (m < x)
    {
        m = x;
    }
}

u_t PMinion::best_score_with_turns(const State &state)
{
    u_t best = 0;
    auto er = state2score.equal_range(state);
    if (er.first == er.second)
    {
        u_t hand_end = n + state.drawn();
        u_t score;

        // T play
        u_t nt = 0, tc_bonus = 0, ts_bonus = 0, tt_bonus = 0;
        for (u_t ti = state.t_played(); 
            ti < t_cards.size() && t_cards[ti] < hand_end; ++ti, ++nt)
        {
            u_t ci = t_cards[ti];
            const Card &card = cards[ci];
            tc_bonus += card.c;
            ts_bonus += card.s;
            tt_bonus += card.t;
        }
        if (nt > 0)
        {
            score = ts_bonus + best_score_with_turns(
                State(
                    min(state.drawn() + tc_bonus, m),
                    state.turns() + tt_bonus - nt, 
                    state.t_played() + nt,
                    state.c0_played(), state.c1_played(), state.c2_played()));
            maximize_by(best, score);
        }

        // c0 & C1 & C2 play
        for (u_t csz = 0; csz <= 2; ++csz)
        {
            const vu_t &csz_cards = cx_cards[csz];
            if (!csz_cards.empty())
            {
                vu_t cscores;
                for (u_t cci = 0; 
                    (cci < csz_cards.size()) && (csz_cards[cci] < hand_end);
                    ++cci)
                {
                    u_t ci = csz_cards[cci];
                    const Card &card = cards[ci];
                    cscores.push_back(card.s);
                }
                // could user nth_element instead ... but n + m is small
                sort(cscores.begin(), cscores.end(), greater<u_t>());
                u_t csz_played = state.csz_played(csz);
                score = cscores[csz_played] + best_score(
                    State(
                        min(state.drawn() + csz, m),
                        state.turns() - 1,
                        state.t_played(),
                        state.c0_played() + (csz == 0 ? 1 : 0),
                        state.c1_played() + (csz == 1 ? 1 : 0),
                        state.c2_played() + (csz == 2 ? 1 : 0)));
                maximize_by(best, score);
            }
        }
        state2ut_t::value_type v(state, best);
        state2score.insert(er.first, v);
    }
    else
    {
        best = (*er.first).second;
    }
    return best;
}

void PMinion::init_graph()
{
    for (u_t ci = 0; ci < n + m; ++ci)
    {
        const Card &card = cards[ci];
        if (card.t > 0)
        {
            t_cards.push_back(ci);
        }
        else
        {
            cx_cards[card.c].push_back(ci);
        }
    }
}

void PMinion::print_solution(ostream &fo) const
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

    void (PMinion::*psolve)() =
        (naive ? &PMinion::solve_naive : &PMinion::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        PMinion PMinion(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (PMinion.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        PMinion.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
