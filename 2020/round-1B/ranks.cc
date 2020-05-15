// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<u_t, 2> u2_t;
typedef vector<u_t> vu_t;
typedef vector<u2_t> vu2_t;
typedef vector<u2_t> vu2_t;
typedef set<vu2_t> set_vu2_t;
typedef map<vu2_t, u2_t> vu2_to_u2_t;
typedef queue<vu2_t> qvu2_t;

static unsigned dbg_flags;

class Ranks
{
 public:
    Ranks(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void adv(vu2_t& past);
    void bfs(vu2_t& past);
    bool is_final(const vu2_t& xdeck) const;
    bool good_rank(u_t ri) const;
    u_t grade(const vu2_t& xdeck) const;
    void op(vu2_t& rot, u_t a, u_t b, const vu2_t& src) const;
    u_t r, s;
    vu2_t solution;
    vu2_t deck;
    set_vu2_t visited;
};

Ranks::Ranks(istream& fi)
{
    fi >> r >> s;
}

void Ranks::solve_naive() // DFS
{
    deck.reserve(r*s);
    for (u_t si = 0; si < s; ++si)
    {
        for (u_t ri = 0; ri < r; ++ri)
        {
            u2_t card({ri, si});
            deck.push_back(card);
        }
    }

    vu2_t deck1, deck2;
    qvu2_t q;
    vu2_to_u2_t parent;
    vu2_to_u2_t::value_type v0{deck, u2_t{0, 0}};
    parent.insert(parent.end(), vu2_to_u2_t::value_type{deck, u2_t{0, 0}});
    q.push(deck);
    while (solution.empty())
    {
        const vu2_t xdeck = q.front();
        if (is_final(xdeck))
        {
            vu2_t tdeck(xdeck);
            vu2_t rev_solution;
            while (tdeck != deck)
            {
                 const u2_t& ab = parent.find(tdeck)->second;
                 vu2_t bdeck;
                 op(bdeck, ab[1], ab[0], tdeck);
                 swap(bdeck, tdeck);
                 rev_solution.push_back(ab);
            }
            solution.insert(solution.end(),
                rev_solution.rbegin(), rev_solution.rend());
        }
        else
        {
            q.pop();
            // cerr << "Q.size=" << q.size() << '\n';
            const u_t rs = r*s;
            for (u_t a = 1; a + 1 < rs; ++a)
            {
                for (u_t b = 1; a + b < rs; ++b)
                {
                    vu2_t tdeck;
                    op(tdeck, a, b, xdeck);
                    if ((xdeck == deck) && (a==3) && (b==2)) 
                    {
                        deck1 = tdeck;
                    }
                    if ((xdeck == deck1) && (a==2) && (b == 1))
                    {
                        deck2 == tdeck;
                    }
                    auto er = parent.equal_range(tdeck);
                    if (er.first == er.second)
                    {
                        vu2_to_u2_t::value_type v{tdeck, u2_t{a, b}};
                        parent.insert(er.first, v);
                        q.push(tdeck);
                    }
                }
            }
        }
    }
}

void Ranks::solve()
{
    deck.reserve(r*s);
    for (u_t si = 0; si < s; ++si)
    {
        for (u_t ri = 0; ri < r; ++ri)
        {
            u2_t card({ri, si});
            deck.push_back(card);
        }
    }
    
    vu2_t xdeck(deck);
    const u_t rs = r*s;
    const u_t target = r*(s - 1);
    u_t cgrade = 0;
    while (cgrade != target)
    {
        vu2_t tdeck;
        u_t best = cgrade;
        u2_t op_best;
        for (u_t apb = 2; apb < rs; ++apb)
        {
            for (u_t a = 1; a < apb; ++a)
            {
                u_t b = apb - a;
                u_t lose1 = (xdeck[a][0] == xdeck[a - 1][0] ? 1 : 0);
                u_t lose2 = (xdeck[apb][0] == xdeck[apb - 1][0] ? 1 : 0);
                u_t add1 = (xdeck[0][0] == xdeck[apb - 1][0] ? 1 : 0);
                u_t add2 = (xdeck[a - 1][0] == xdeck[apb][0] ? 1 : 0);
                u_t add = add1 + add2;
                u_t lose = lose1 + lose2;
                u_t g = (cgrade + add) - lose;
                if (best < g)
                {
                    best = g;
                    op_best = u2_t{a, b};
                }
            }
        }
        if (cgrade == best)
        {
            cerr << "no progress cgrade="<< cgrade << '\n';
            exit(1);
        }
        cgrade = best;
        op(tdeck, op_best[0], op_best[1], xdeck);
        solution.push_back(op_best);
        swap(xdeck, tdeck);
        
    }
}

u_t Ranks::grade(const vu2_t& xdeck) const
{
    u_t g = 0;
    for (u_t i = 1, e = xdeck.size(); i < e; ++i)
    {
        g += (xdeck[i][0] == xdeck[i - 1][0] ? 1 : 0);
    }
    return g;
}

bool Ranks::is_final(const vu2_t& xdeck) const
{
    bool good = true;
    for (u_t ri = 0; good && (ri < r); ++ri)
    {
        for (u_t si = 0; good && (si < s); ++si)
        {
            u_t ci = ri*s + si;
            good = xdeck[ci][0] == ri;
        }
    }
    return good;
}

bool Ranks::good_rank(u_t ri) const
{
    bool good = true;
    for (u_t si = 0; good && (si < s); ++si)
    {
        u_t ci = ri*s + si;
        good = deck[ci][0] == ri;
    }
    return good;
}

void Ranks::op(vu2_t& rot, u_t a, u_t b, const vu2_t& src) const
{
    rot = src;
    const u_t apb = a + b;
    for (u_t n = 0; n < apb; ++n)
    {
        rot[(n + b) % apb] = src[n];
    }
}

void Ranks::print_solution(ostream &fo) const
{
    fo << ' ' << solution.size();
    for (const u2_t& ab: solution)
    {
        fo << '\n' << ab[0] << ' ' << ab[1];
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

    string ignore;
    unsigned n_cases;
    *pfi >> n_cases;
    getline(*pfi, ignore);

    void (Ranks::*psolve)() =
        (naive ? &Ranks::solve_naive : &Ranks::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Ranks ranks(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (ranks.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        ranks.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
