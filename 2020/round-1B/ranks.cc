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
    void solve_naive0();
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void adv(vu2_t& past);
    void bfs(vu2_t& past);
    bool is_final(const vu2_t& xdeck) const;
    bool good_rank(u_t ri) const;
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

void Ranks::solve_naive0()
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
    vu2_t past;
    adv(past);
}

void Ranks::adv(vu2_t& past)
{
    // find last rank
    int ri = r - 1;
    while ((ri >= 0) && good_rank(ri))
    {
        --ri;
    }
    if (ri < 0)
    {
        if (solution.empty() || (past.size() < solution.size()))
        {
            solution = past;
        }
    }
    else
    {
        const u_t uri = ri;
        int ci = (ri + 1)*s - 1;
        while (deck[ci][0] == uri)
        {
            --ci;
        }
        u_t ci_bad = ci;
        vu_t cands;
        for (; ci >= 0; --ci)
        {
            if (deck[ci][0] == uri)
            {
                cands.push_back(ci);
            }
        }
        for (u_t gi: cands)
        {
            vu2_t save = deck;
            u_t a = gi + 1, b = ci_bad - gi;
            vu2_t adeck(deck.begin(), deck.begin() + a);
            for (u_t i = 0; i < b; ++i)
            {
                deck[i] = deck[a + i];
            }
            for (u_t i = 0; i < a; ++i)
            {
                deck[b + i] = adeck[i];
            }
            past.push_back(u2_t({a, b}));
            adv(past);
            past.pop_back();
            deck = save;
        }
    }
}

void Ranks::solve_naive()
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
    map<vu2_t, u_t> depth;

    qvu2_t q;
    vu2_to_u2_t parent;
    // vu2_to_u2_t::value_type v0{deck, u2_t{0, 0}};
    vu2_to_u2_t::value_type v0{deck, u2_t{0, 0}};
    parent.insert(parent.end(), vu2_to_u2_t::value_type{deck, u2_t{0, 0}});
    q.push(deck);
    depth.insert(depth.end(), map<vu2_t, u_t>::value_type(deck, 0));
    while (solution.empty())
    {
        const vu2_t xdeck = q.front();
        if (depth.find(xdeck) == depth.end()) {
            ;  // cerr << "depth not found\n";
        }
        u_t xdepth = depth.find(xdeck)->second;
        if (xdepth > 6) {
            cerr << "xdepth="<<xdepth << '\n';
            exit(1);
        }
        
        if (xdeck == deck2)
        {
            ; // cerr << "Is it ???\n";
        }
        if (is_final(xdeck))
        {
            vu2_t tdeck(xdeck);
            vu2_t rev_solution;
            while (tdeck != deck)
            {
                 if (depth.find(tdeck) == depth.end()) {
                     cerr << "depth not found\n";
                 }
                 // cerr << "tdeck:depth=" << depth[tdeck] << '\n';
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
                        depth.insert(depth.end(), 
                          map<vu2_t, u_t>::value_type(tdeck, xdepth + 1));
                    }
                }
            }
        }
    }
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

void Ranks::solve()
{
    solve_naive();
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
