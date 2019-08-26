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
    // bool any_reduce(u_t gi, u_t ogi) const;
    void set_back_age(vull_t& back_age, const vullu_t& pg_back) const;
    void set_solution(const vullu_t& pg_fore, const vull_t& back_age);
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

#if 0
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
#endif

void Lguests::solve()
{
    ull_t mmn = m % n;
    ull_t clock_delta = mmn;
    ull_t anti_delta = n - mmn;
    solution = vull_t(vu_t::size_type(g), ull_t(-1));
    for (ull_t cwi = 0; cwi != 2; ++cwi)
    {
        const bool clockwise = (cwi == 0);
        vullu_t pg_fore, pg_back;
        for (u_t gi = 0; gi < g; ++gi)
        {
            ull_t hgi = (clockwise ? h[gi] : (n - h[gi]) % n);
            if (cw[gi] == clockwise)
            {
                u_t p = (hgi + clock_delta) % n;
                pg_fore.push_back(ullu_t(p, gi));
            }
            else
            {
                u_t p = (hgi + anti_delta) % n;
                pg_back.push_back(ullu_t(p, gi));
            }
        }
        sort(pg_fore.begin(), pg_fore.end());
        sort(pg_back.begin(), pg_back.end());
        vull_t back_age;
        set_back_age(back_age, pg_back);
        set_solution(pg_fore, back_age);
    }
}

void Lguests::set_back_age(vull_t& back_age, const vullu_t& pg_back) const
{
    back_age = vull_t(vull_t::size_type(n), ull_t(-1));
    u_t sz = pg_back.size();
    if (sz > 0)
    {
        if (pg_back.front().first == pg_back.back().first)
        {
            u_t age_max = min(m, n - 1);
            for (ull_t hi = pg_back[0].first, age = 0; age <= age_max;
                 hi = (hi + 1) % n, ++age)
            {
                back_age[hi] = age;
            }        
        }
        else
        {
            bool loop = true;           
            for (u_t i = 0, inext = 0; loop; i = inext)
            {
                i = inext;
                ull_t hi = pg_back[i].first;
                while (pg_back[inext].first == hi)
                {
                    ++inext;
                    if (inext == pg_back.size())
                    {
                        inext = 0;
                        loop = false;
                    }
                }
                ull_t hnext = pg_back[inext].first;
                ull_t age = 0;
                for ( ; (hi != hnext) && (age <= m); hi = (hi + 1) % n, ++age)
                {
                    back_age[hi] = age;
                }
            }
        }
    }
}

void Lguests::set_solution(const vullu_t& pg_fore, const vull_t& back_age)
{
    u_t sz = pg_fore.size();
    ull_t h_behind = n - 1, h_last = n - 1;
    bool single = (sz == 0) || (pg_fore.front().first == pg_fore.back().first);
    const u_t age_max = min(m, n - 1);
    for (u_t i = 0; i != sz; ++i)
    {
        ull_t count = 0;
        ull_t hi = pg_fore[i].first;
        if (h_last != hi)
        {
            h_behind = h_last;
        }
        for (ull_t age = 0; (single || (hi != h_behind)) && (age <= age_max);
            hi = (hi == 0 ? n - 1 : hi - 1), ++age)
        {
            if (age <= back_age[hi])
            {
                ++count;
            }
        }
        ull_t gi = pg_fore[i].second;
        solution[gi] = count;
        h_last = pg_fore[i].first;
    }    
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
