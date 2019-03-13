// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <set>
// #include <map>
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

static unsigned dbg_flags;

class Card
{
 public:
   Card(u_t vc=0, u_t vs=0, u_t vt=0) : c(vc), s(vs), t(vt) {}
   u_t c, s, t;
};
typedef vector<Card> vcard_t;

class Status
{
 public:
    Status(u_t t=1, u_t s=0, const setu_t& h=setu_t(), u_t dt=0) :
        turns(t), score(s), hand(h), deck_top(dt) {}
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
    u_t n, m;    
    vcard_t cards;
    u_t solution;
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
        for (setu_t::iterator i = hand.begin(), e = hand.end(); i != e; ++i)
        {
            u_t ci = *i;
            const Card &card = cards[ci];
            hand.erase(i);
            status.score += card.s;
            status.turns += card.t;
            u_t draw = min(card.c, m - status.deck_top);
            for (u_t di = 0; di < draw; ++di)
            {
                hand.insert(setu_t::value_type(n + status.deck_top + di));
            }
            status.deck_top += draw;
            traverse(status);
            status.deck_top -= draw;
            // restore
            for (u_t di = 0; di < draw; ++di)
            {
                hand.erase(setu_t::value_type(n + status.deck_top + di));
            }
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
