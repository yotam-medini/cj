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
typedef vector<u_t> vu_t;
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
    State(u_t vd=0, u_t vt=0, int c0=-1, int c1=-1,  int c2=-1) :
        _tup(vd, vt, c0, c1, c2) {}
    const u_t& drawn() const { return std::get<0>(_tup); }
    u_t& drawn() { return std::get<0>(_tup); }
    const u_t& turns() const { return std::get<1>(_tup); }
    u_t& turns() { return std::get<1>(_tup); }
    const int& c0_next() const { return std::get<2>(_tup); }
    int& c0_next() { return std::get<2>(_tup); }
    const int& c1_next() const { return std::get<3>(_tup); }
    int& c1_next() { return std::get<3>(_tup); }
    const int& c2_next() const { return std::get<4>(_tup); }
    int& c2_next() { return std::get<4>(_tup); }
    static bool eq(const State& s0, const State& s1)
    {
        return s0._tup == s1._tup;
    }
    static bool lt(const State& s0, const State& s1)
    {
        return s0._tup < s1._tup;
    }
  private:
    tuple<u_t, u_t, int, int, int> _tup;
};
bool operator==(const State& s0, const State& s1)
{
    return State::eq(s0, s1);
}
bool operator<(const State& s0, const State& s1)
{
    return State::lt(s0, s1);
}

class Edge
{
 public:
    Edge(u_t vscore=0) : score(vscore) {}
    u_t score;
    vector<State> to_states;
};

class Node
{
  public:
    enum colot_t {White, Gray, Black};
    Node() : color(White), score(0) {}
    colot_t color;
    u_t score;
    vector<Edge> edges;
};

typedef map<State, Node> state2node_t;

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
    int cx_first(u_t nc) const 
    {
        int ret = cx_cards[nc].empty() ? -1 : cx_cards[nc].front();
        return ret;
    }
    static const State state_final;
    u_t n, m;    
    vcard_t cards;
    u_t solution;
    vu_t t_cards;
    dqu_t cx_cards[3];
    State state_initial;
    state2node_t graph;
};
const State PMinion::state_final(~0, 0, -1. -1. -1);

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
    solve_naive();
    init_graph();
}

void PMinion::init_graph()
{
    for (u_t ci = 0; ci < n; ++ci)
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
    state_initial = State(0, 1, cx_first(0), cx_first(1), cx_first(2));
    state2node_t::value_type v(state_initial, Node());
    graph.insert(graph.end(), v);
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
