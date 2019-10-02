// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <map>
#include <queue>
#include <set>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef array<u_t, 2> au2_t;
typedef pair<u_t, u_t> uu_t;
typedef vector<au2_t> vau2_t;
typedef set<au2_t> setau2_t;
typedef vector<vu_t> vvu_t;

static unsigned dbg_flags;

class Cherries
{
 public:
    Cherries(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void set_graph();
    u_t n, m;
    vau2_t bstrands;
    u_t solution;
    vvu_t graph;
};

Cherries::Cherries(istream& fi) : solution(0)
{
    fi >> n >> m;
    for (u_t i = 0; i < m; ++i)
    {
        u_t c1, c2;
        fi >> c1 >> c2;
        bstrands.push_back(au2_t({c1, c2}));
    }
}

void Cherries::solve_naive()
{
    vau2_t all_edges;
    setau2_t black_edges;
    for (const au2_t& cc: bstrands)
    {
        u_t c0 = cc[0] - 1;
        u_t c1 = cc[1] - 1;
        if (c0 > c1) { swap(c0, c1); }
        black_edges.insert(black_edges.end(), au2_t({c0, c1}));
    }
    for (u_t c0 = 0; c0 < n; ++c0)
    {
        for (u_t c1 = c0 + 1; c1 < n; ++c1)
        {
            all_edges.push_back(au2_t({c0, c1}));
        }
    }
    solution = 2*n; // infinite
    u_t bits_max = (1u << all_edges.size());
    for (u_t b = 0; b < bits_max; ++b)
    {
        // active edges masked by b. See if all connected.
        vu_t concan;
        for (u_t v = 0; v < n; ++v) { concan.push_back(v); }
        bool all_zero = true;
        for (bool connecting = true; connecting; )
        {
            connecting = false;
            all_zero = true;
            for (u_t ei = 0, en = all_edges.size(); ei < en; ++ei)
            {
                const au2_t& e = all_edges[ei];
                if ((1u << ei) & b)
                {
                    for (u_t i = 0; i != 2; ++i)
                    {
                        if (concan[e[1 - i]] > concan[e[i]])
                        {
                            concan[e[1 - i]] = concan[e[i]];
                            connecting = true;
                        }
                    }
                }
                all_zero = all_zero && (concan[e[1]] == 0);
            }
        }
        if (all_zero) // all connected!
        {
            u_t sugar = 0;
            for (u_t ei = 0, en = all_edges.size(); ei < en; ++ei)
            {
                if ((1u << ei) & b)
                {
                    const au2_t& e = all_edges[ei];
                    bool black = (black_edges.find(e) != black_edges.end());
                    sugar += (black ? 1 : 2);
                }
            }
            if (solution > sugar)
            {
                solution = sugar;
            }
        }
    }
}

void Cherries::solve()
{
    set_graph();
    u_t ccc = 0;
    typedef enum { White, Gray, Black } color_t;
    typedef vector<color_t> vcolor_t;
    vcolor_t vcolor(vcolor_t::size_type(n), White);
    for (u_t wi = 0; wi < n; ++wi)
    {
        for (; (wi < n) && vcolor[wi] != White; ++wi) {}
        if (wi < n)
        {
            ++ccc;
            queue<u_t> q;
            q.push(wi);
            while (!q.empty())
            {
                u_t v = q.front();
                q.pop();
                vcolor[v] = Gray;
                const vu_t& adjs = graph[v];
                for (u_t a: adjs)
                {
                    if (vcolor[a] == White)
                    {
                        q.push(a);
                    }
                }
                vcolor[v] = Black;
            }
        }
    }
    solution = (n - 1) + (ccc - 1);
}

void Cherries::set_graph()
{
    graph = vvu_t(vvu_t::size_type(n), vu_t());
    for (const au2_t& cc: bstrands)
    {
        for (u_t i = 0; i != 2; ++i)
        {
            u_t c0 = cc[i] - 1;
            u_t c1 = cc[1 - i] - 1;
            vu_t& a = graph[c0];
            a.push_back(c1);
        }
    }
}

void Cherries::print_solution(ostream &fo) const
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

    void (Cherries::*psolve)() =
        (naive ? &Cherries::solve_naive : &Cherries::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Cherries cherries(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (cherries.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        cherries.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
