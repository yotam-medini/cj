// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <set>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
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

class Status
{
 public:
    Status(u_t t=1, u_t s=0, const setu_t& h=setu_t(), u_t dt=0) :
        turns(t), score(s), hand(h), deck_top(dt) {}
    void deck2hand(u_t draw, u_t deck_offset)
    {
        for (u_t di = 0; di < draw; ++di)
        {
            u_t ci = deck_offset + di;
            hand.insert(hand.end(), ci);
        }
        deck_offset += draw;
    }
    void hand2deck(u_t draw, u_t deck_offset)
    {
        deck_offset -= draw;
        for (u_t di = 0; di < draw; ++di)
        {
            u_t ci = deck_offset + di;
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
    bool invest_turns(Status &status);
    u_t dynamic_best(const Status &status);
    void safe_play(Status &status);
    u_t n, m;    
    vcard_t cards;
    u_t solution;
    usetu2u_t memo;
};

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
        for (setu_t::iterator i = hand.begin(), i1 = i, e = hand.end(); i != e;
            i = i1)
        {
            ++i1;
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
    Status status;
    setu_t &hand = status.hand;
    for (u_t i = 0; i < n; ++i)
    {
        hand.insert(status.hand.end(), i);
    }
    
    // Play cards without losing turns;
    bool safe = true;
    while (safe && (status.turns > 0) && !hand.empty())
    {
        safe_play(status);
        safe = invest_turns(status);
    }
    solution = status.score;

    if ((status.turns > 0) && !hand.empty())
    {
        u_t score_add = dynamic_best(status);
        solution += score_add;
    }
}

void PMinion::safe_play(Status &status)
{
    bool safe = true;
    setu_t &hand = status.hand;
    while (safe)
    {
        safe = false;
        for (setu_t::iterator i = hand.begin(), i1 = i, e = hand.end();
            i != e; i = i1)
        {
            ++i1;
            u_t ci = *i;
            const Card &card = cards[ci];
            if (card.t >= 1)
            {
                safe = true;
                hand.erase(i);
                status.turns += (card.t - 1);
                status.score += card.s;
                u_t draw = min(card.c, m - status.deck_top);
                for (u_t di = 0; di < draw; ++di)
                {
                    u_t dci = n + status.deck_top + di;
                    hand.insert(setu_t::value_type(dci));
                }
                status.deck_top += draw;                
            }
        }
    }
}

bool PMinion::invest_turns(Status &status)
{
    vu_t invested;
    vu_t vhand(status.hand.begin(), status.hand.end());
    u_t turns = status.turns;
    u_t deck_top = status.deck_top;
    u_t score = 0;
    while ((turns > 0) && (!vhand.empty()) && (deck_top < m) &&
        (invested.empty() || (turns < status.turns)))
    {
        sort(vhand.begin(), vhand.end(), 
            [&](u_t i0, u_t i1) {
                return (cards[i0].c < cards[i1].c) ||
                    ((cards[i0].c == cards[i1].c) &&
                    (cards[i0].s < cards[i1].s)); });
        u_t ci = vhand.back();
        const Card &card = cards[ci];
        vhand.pop_back();
        --turns;
        turns += card.t;
        score += card.s;
        u_t draw = min(card.c, m - deck_top);
        for (u_t di = 0; di < draw; ++di, ++deck_top)
        {
            vhand.push_back(n + deck_top);
        }        
    }
    bool found = (deck_top > status.deck_top) && (turns >= status.turns);
    if (found)
    {
        status.turns = turns;
        status.score += score;
        status.hand = setu_t(vhand.begin(), vhand.end());
        status.deck_top = deck_top;
    }
    return found;
}

u_t PMinion::dynamic_best(const Status &status)
{
    u_t best = 0;
    usetu2u_t::key_type key(status.deck_top, status.hand);
    auto er = memo.equal_range(key);
    usetu2u_t::iterator i = er.first;
    if (er.first == er.second)
    {
        // hand cards are t=0, c in {0,1,2}
        u_t c_candidates[3] = {0, 0, 0};
        int c_scores[3] = {-1, -1, -1};
        for (u_t ci: status.hand)
        {
            const Card &card = cards[ci];
            if ((!card.zero()) && (int(card.s) >= c_scores[card.c]))
            {
                c_candidates[card.c] = ci;
                c_scores[card.c] = card.s;
            }
        }
        for (u_t si = 0; si < 3; ++si)
        {
            if (c_scores[si] >= 0)
            {
                u_t ci = c_candidates[si];
                const Card &card = cards[ci];
                u_t dscore = card.s;
                Status dstatus(status);
                dstatus.turns = (dstatus.turns + card.t) - 1;
                if (dstatus.turns > 0)
                {
                    dstatus.hand.erase(ci);
                    u_t draw = min(card.c, m - status.deck_top);
                    dstatus.deck2hand(draw, n);
                    dstatus.score = 0;
                    safe_play(dstatus);
                    dscore += dstatus.score;
                    u_t score_add = dynamic_best(dstatus);
                    dscore += score_add;
                }
                if (best < dscore)
                {
                    best = dscore;
                }
            }
        }
        memo.insert(i, usetu2u_t::value_type(key, best));
    }
    else
    {
        best = (*i).second;
    }
    return best;
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
