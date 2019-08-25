// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <algorithm>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef set<u_t> setu_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;
typedef vector<bool> vb_t;
typedef vector<setu_t> vsetu_t;
typedef pair<ull_t, u_t> ullu_t;
typedef vector<ullu_t> vullu_t;

static unsigned dbg_flags;

class Lguests
{
 public:
    Lguests(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool any_reduce(u_t gi, u_t ogi) const;
    ull_t n, g, m;
    vb_t cw;
    vull_t h;
    vull_t solution;
    vsetu_t last;
};

Lguests::Lguests(istream& fi)
{
    fi >> n >> g >> m;
    cw.reserve(g);
    h.reserve(g);
    u_t x;
    for (u_t i = 0; i < g; ++i)
    {
        char c;
        fi >> x >> c;
        // fi.getline() ?
        if (c != 'C' && c != 'A')
        {
             cerr << "bad c="<<c << "\n";
             exit(1);
        }
        cw.push_back(c == 'C');
        h.push_back(x - 1);
    }
}

void Lguests::solve_naive()
{
    last = vsetu_t(vsetu_t::size_type(n), setu_t());
    for (u_t step = 0; step <= m; ++step)
    {
        vsetu_t occ = vsetu_t(vsetu_t::size_type(n), setu_t());
        for (u_t gi = 0; gi < g; ++gi)
        {
            u_t hgi = (h[gi] + (step % n) * (cw[gi] ? 1 : n - 1)) % n;
            occ[hgi].insert(gi);
        }
        for (u_t ci = 0; ci < n; ++ci)
        {
            if (!occ[ci].empty())
            {
                swap(last[ci], occ[ci]);
            }
        }
    }
    solution.reserve(g);
    for (u_t gi = 0; gi < g; ++gi)
    {
        u_t remember = 0;
        for (const setu_t& s: last)
        {
            if (s.find(gi) != s.end())
            {
                ++remember;                
            }
        }
        solution.push_back(remember);
    }
}

static void minimize(ull_t& v, ull_t x)
{
    if (v > x)
    {
        v = x;
    }
}

static void maximize(ull_t& v, ull_t by)
{
    if (v < by)
    {
        v = by;
    }
}

void Lguests::solve()
{
    ull_t mmn = m % n;
    ull_t clock_delta = mmn;
    ull_t anti_delta = n - mmn;
    ull_t bound = min(m + 1, n);
    solution = vull_t(vu_t::size_type(g), bound);
    for (ull_t cwi = 0; cwi != 2; ++cwi)
    {
        const bool clockwise = (cwi == 0);
        vullu_t pg_clock, pg_anti;
        for (u_t gi = 0; gi < g; ++gi)
        {
            ull_t hgi = (clockwise ? h[gi] : (n - h[gi]) % n);
            if (cw[gi] == clockwise)
            {
                u_t p = (hgi + clock_delta) % n;
                pg_clock.push_back(ullu_t(p, gi));
            }
            else
            {
                u_t p = (hgi + anti_delta) % n;
                pg_anti.push_back(ullu_t(p, gi));
            }
        }
        sort(pg_clock.begin(), pg_clock.end());
        sort(pg_anti.begin(), pg_anti.end());
        const u_t clock_sz = pg_clock.size();
        const u_t anti_sz = pg_anti.size();
        for (u_t pgi = 0; pgi != clock_sz; ++pgi)
        {
            ull_t s = 0;
            const ullu_t& pg = pg_clock[pgi];
            u_t p = pg.first;
            u_t gi = pg.second;
            u_t ppgi = (pgi + clock_sz - 1) % clock_sz;
            while ((pg_clock[ppgi].first == p) && (ppgi != pgi))
            {
                ppgi = (ppgi + (clock_sz - 1)) % clock_sz;
            }
            s = ((ppgi == pgi) ? n : 
                (p + (pgi == 0 ? n : 0)) - pg_clock[ppgi].first);
            minimize(s, bound);
            if (!pg_anti.empty())
            {
                auto er = equal_range(pg_anti.begin(), pg_anti.end(), pg);
                if (er.first == er.second)
                {
                    u_t apgi = er.first - pg_anti.begin();
                    apgi = (apgi + (anti_sz - 1)) % anti_sz;
                    const ullu_t& apg = pg_anti[apgi];
                    if (any_reduce(pg.second, apg.second))
                    {
                        ull_t reduce = (n - 1)/2;
                        ull_t dist = (p + (n - apg.first)) % n;
                        if (dist > 0)
                        {
                            minimize(reduce, (min(dist, m) + 1)/2);
                        }
                        s = (s > reduce ? s - reduce : 0);
                        maximize(s, 1);
                    }
                }
            }
            minimize(solution[gi], s);
        }
    }
}

bool Lguests::any_reduce(u_t gi, u_t ogi) const
{
    ull_t m2 = 2*m;
    bool any = m2 > n;
    if (!any)
    {
        ull_t x = h[gi];
        ull_t ox = h[ogi];
        if (cw[gi])
        {
            any = ((x < ox) && (ox - x < m2)) ||
                ((ox < x) && ((ox + n) - x < m2));
        }
        else
        {
            any = ((x < ox) && ((x + n) - ox < m2)) ||
                ((ox < x) && (x - ox < m2));
        }
    }
    return any;
}

void Lguests::print_solution(ostream &fo) const
{
    for (u_t remember: solution)
    {
        fo << ' ' << remember;
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

    void (Lguests::*psolve)() =
        (naive ? &Lguests::solve_naive : &Lguests::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Lguests lguests(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (lguests.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        lguests.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
